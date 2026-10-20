# NTFSTool architecture

This document explains how the pieces of the open-source edition fit together. Every source file also starts
with a header comment describing its role, so you can read this page first and then jump into the code.

## The big picture

```
 Finder / any app
        │  POSIX file calls (open, write, rename, …)
        ▼
 macOS VFS  ──►  fskitd  ──►  NTFSToolFS.appex            (FSKitModule/NTFSToolFS)
                               │  FSUnaryFileSystem / FSVolume operations
                               ▼
                         NTFSVolume (Swift)                (Sources/NTFSToolDriver)
                               │  inode-number based calls, Swift errors
                               ▼
                         ntfstool_bridge.c (C)             (Sources/NTFSToolBridge)
                               │  ntfs_inode_open / ntfs_attr_pread / …
                               ▼
                         libntfstool-driver                        (Vendor/NTFSToolNTFS.xcframework, prebuilt)
                               │  pread / pwrite through a custom device
                               ▼
                         FSBlockDeviceResource (raw disk handed over by FSKit)

 NTFSTool.app (sandboxed UI)                               (Sources/NTFSTool, NTFSToolApp, NTFSToolCore)
   lists NTFS volumes via DiskArbitration, shows their state, guides the user to enable the extension,
   opens in Finder, ejects. It never touches the raw disk: mounting is done by macOS itself.
```

The app and the driver are two separate processes. They share no code at runtime except the small,
Foundation-only `EventLog` that is compiled into both.

## Who mounts the disk?

Nobody in this repository calls `mount`. When a disk is plugged in, `diskarbitrationd` looks at the partition
type, finds file system modules whose `FSMediaTypes` (written into the module's Info.plist by
`scripts/build-fskit.sh`) match, and asks each module to `probe` the device. Apple's built-in NTFS driver (read
only) is always a candidate; NTFSTool's module wins once the user has enabled it in
**System Settings → Login Items & Extensions → File System Extensions**. That is why the app's main job is to
detect that state and guide the user there (`NTFSToolModel`, `LiveDriverStatus`, `NTFSToolGuideCard`).

A disk that was already mounted by Apple's driver before the extension was enabled stays read-only until it is
re-attached; a sandboxed app is not allowed to remount physical disks, so the app asks the user to eject and
reconnect.

## Driver layers

### FSKit module — `FSKitModule/NTFSToolFS/NTFSToolFS.swift`

- `NTFSToolFileSystem` (`FSUnaryFileSystem`): `probeResource` reads the boot sector through the bridge and
  reports whether this is NTFS; `loadResource` mounts it and returns an `NTFSToolVolume`.
- `NTFSToolVolume` (`FSVolume` + operation protocols): translates each FSKit callback (lookup, create, read,
  write, rename, xattrs, …) into one `NTFSVolume` call and maps errors to `errno` values FSKit understands.
- `ResourceDevice`: adapts the `FSBlockDeviceResource` handed over by FSKit to the `BlockDevice` protocol.
- FSKit item identifiers are derived from NTFS MFT record numbers (`NTFSToolFSKit.itemID(forIno:)`), so an item
  keeps the same identity for as long as the file exists.

The module is compiled by `scripts/build-fskit.sh` with `swiftc` directly (not SwiftPM), because it has to link
against the FSKit SDK and is packaged as an `.appex` inside the app.

### Swift driver core — `Sources/NTFSToolDriver`

`NTFSVolume` wraps the C bridge with Swift types (`NTFSAttributes`, `NTFSDirEntry`, `NTFSError`) and serializes
all calls on one lock: libntfstool-driver is not thread safe. It has no FSKit dependency, so it is unit-tested against
real NTFS images (`Tests/NTFSToolDriverTests`, formatted with the driver's `mkntfs` and verified afterwards with
`ntfsfix`).

### C bridge — `Sources/NTFSToolBridge/ntfstool_bridge.c`

The engine's natural API is path- and inode-pointer based (it was written for FUSE). FSKit is item based. The
bridge exposes "do X to inode number N" functions (`ntfstool_read`, `ntfstool_create`, `ntfstool_rename`, …)
and follows the same rules as common FUSE NTFS drivers:

- **one `ntfs_inode` per MFT record at a time** — every operation opens the inode, works, and closes it. Holding
  two in-memory copies of the same record would let one overwrite the other's directory index changes;
- rename is implemented as link + unlink (with a temporary name for case-only renames), and a directory can never
  be moved into its own subtree (`is_descendant`);
- names Windows cannot use are refused when the volume is mounted with `-o windows_names` (the same rule set as the engine's `ntfsdrv_forbidden_names`);
- extended attributes are stored as NTFS named data streams (the usual `user_xattr` scheme);
- a hibernated or "Fast Startup" Windows volume is mounted read-only instead of failing.

Device I/O is plain direct, write-through I/O with block alignment. The official builds add a caching layer and
other tuning on top; this edition intentionally keeps the simplest correct path so it is easy to follow.

## App layers

| Layer | Files | Notes |
| --- | --- | --- |
| Entry point | `Sources/NTFSTool/main.swift` | calls `NTFSToolAppMain.run()` |
| App shell | `NTFSToolApp.swift`, `NTFSToolWindow.swift`, `NTFSToolAbout.swift` | AppKit: menu, window with sidebar + toolbar, About |
| Model | `NTFSToolModel.swift` | every user flow; UI only calls the model |
| Views | `NTFSToolViews.swift`, `NTFSToolComponents.swift` | SwiftUI sidebar, detail page, guide card |
| System | `NTFSToolSystem.swift`, `NTFSToolDiskInfo.swift`, `NTFSToolFSKitSystem.swift` | DiskArbitration, mount table, FSKit extension status |
| Services | `NTFSToolPresenter.swift`, `NTFSToolSystemServices.swift`, `NTFSToolL10n.swift` | alerts, notifications, runtime language switching |
| Core | `Sources/NTFSToolCore` | volume model, language list, driver short names, event log |

All side effects go through protocols (`NTFSToolDiskSystem`, `NTFSToolDriverStatusProviding`, `Presenting`,
`Notifying`). Real implementations talk to macOS; the demo implementations (`DemoDiskSystem`,
`DemoDriverStatus`) keep disks in memory. Tests drive the model synchronously with the demo implementations.

### Volume state

`NTFSToolStateClassifier` decides a volume's state from the mount table entry alone:

| Mounted by | Writable? | State |
| --- | --- | --- |
| our module (`ntfstooloss`; also `ntfstool` / `ntfspro` from the official builds) | yes | read-write |
| our module | no | read-only: Windows hibernated / Fast Startup |
| Apple's `ntfs` | — | read-only by the system driver → enable the extension |
| not mounted | — | not mounted |

## Demo mode

`NTFSTOOL_DEMO=1` (or `make demo`) replaces the real system services with in-memory disks, so you can work on
the UI without a driver, without real disks and without an Apple Developer account.

| Variable | Meaning |
| --- | --- |
| `NTFSTOOL_DEMO=1` | use demo disks |
| `NTFSTOOL_DEMO_SCENE` | `guide`, `readonly` or `empty` (default: three drives) |
| `NTFSTOOL_DEMO_SELECT` | select a volume by name |
| `NTFSTOOL_DEMO_LANG` | UI language by lproj name, e.g. `zh-Hans`, `ja`, `ar` |
| `NTFSTOOL_SNAPSHOT=<file.png>` | render the window to a PNG and quit |

## Localization

User-visible text is looked up with `L("key")` from `Sources/NTFSToolApp/Resources/<lang>.lproj/Localizable.strings`.
English is the fallback for missing keys. The UI language follows the system's preferred languages
(`NTFSTOOL_DEMO_LANG` overrides it for screenshots).

## Privacy

The open-source edition makes no network requests. Links (website, GitHub, NTFSTool Pro) open in the browser
only when clicked. The app's entitlements contain only the App Sandbox.
