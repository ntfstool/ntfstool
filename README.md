# NTFSTool (open-source edition)

Free, open-source read-write NTFS for macOS, built on Apple's **FSKit** and NTFSTool's own NTFS engine (**libntfstool-driver**).

Plug in a Windows (NTFS) drive and macOS mounts it writable through NTFSTool's FSKit driver module — no kernel
extension, no macFUSE, no reduced security settings, no admin password. The app itself is fully sandboxed.

**English** · [简体中文](README-CN.md) · [繁體中文](docs/readme/README.zh-Hant.md) · [日本語](docs/readme/README.ja.md) · [한국어](docs/readme/README.ko.md) · [Deutsch](docs/readme/README.de.md) · [Français](docs/readme/README.fr.md) · [Español](docs/readme/README.es.md) · [Italiano](docs/readme/README.it.md) · [Português](docs/readme/README.pt.md) · [Русский](docs/readme/README.ru.md) · [العربية](docs/readme/README.ar.md) · [فارسی](docs/readme/README.fa.md) · [Türkçe](docs/readme/README.tr.md) · [हिन्दी](docs/readme/README.hi.md) · [বাংলা](docs/readme/README.bn.md) · [Bahasa Indonesia](docs/readme/README.id.md) · [Kiswahili](docs/readme/README.sw.md) · [ไทย](docs/readme/README.th.md) · [Tiếng Việt](docs/readme/README.vi.md)

[Architecture guide](docs/ARCHITECTURE.md)

<p align="center">
  <img src="docs/screenshots/overview.png" alt="NTFSTool main window: a read-write NTFS drive with capacity bar and details" width="760">
</p>

<p align="center"><i>If this project is useful or interesting to you, a ⭐ on GitHub helps others find it.</i></p>

## Screenshots

Real screenshots of the app built from this repository (`make app`). The drives shown are the sample drives of the
built-in demo mode (`make demo`), so you can try every screen without a physical disk.

### Main window

One window for all your NTFS drives. Each volume shows its state at a glance — **Read & Write**, **Read-Only** or
**Not Mounted** — with a capacity bar, mount point, bus (USB / Thunderbolt) and device details, plus *Show in
Finder* and *Eject*.

![Main window: a read-write USB drive](docs/screenshots/overview.png)

| A Thunderbolt SSD, read & write | A drive that is not mounted |
| --- | --- |
| ![Thunderbolt drive](docs/screenshots/thunderbolt-drive.png) | ![Not mounted drive](docs/screenshots/not-mounted.png) |

### Guided setup: no password, no kernel extension

NTFSTool is a regular FSKit file system module. The only thing macOS asks for is one switch in
**System Settings → General → Login Items & Extensions → File System Extensions**. The app explains it step by
step and notices the change by itself.

![Turn on the file system extension](docs/screenshots/enable-extension.png)

### Safe by default

If Windows was hibernated or *Fast Startup* is on, writing would corrupt the disk. NTFSTool mounts such a drive
**read-only** and tells you exactly how to fix it, instead of risking your data.

| Drive left hibernated by Windows | What the app tells you |
| --- | --- |
| ![Hibernated drive](docs/screenshots/hibernated-windows.png) | ![Shut Down Windows Completely](docs/screenshots/dialog-shut-down-windows.png) |

### Plugging a drive in again

macOS only lets the system mount physical drives when they are plugged in, so the app safely ejects the drive and
asks you to plug it back in; it then mounts read-write.

![Plug the drive in again](docs/screenshots/dialog-plug-in-again.png)

### Dark mode

| Main window | Extension guide |
| --- | --- |
| ![Dark mode, main window](docs/screenshots/dark-overview.png) | ![Dark mode, extension guide](docs/screenshots/dark-enable-extension.png) |

### Compact layout, empty state and About

| Sidebar hidden | Waiting for a drive |
| --- | --- |
| ![Sidebar hidden](docs/screenshots/sidebar-hidden.png) | ![No drive connected](docs/screenshots/no-drive.png) |

![About NTFSTool](docs/screenshots/about.png)

The interface is available in 20 languages, including right-to-left layouts for Arabic and Persian: English,
简体中文, 繁體中文, 日本語, 한국어, Deutsch, Français, Español, Italiano, Português, Русский, العربية, فارسی,
Türkçe, हिन्दी, বাংলা, Bahasa Indonesia, Kiswahili, ไทย and Tiếng Việt.

## What this repository is

This is the **open-source learning / reference edition** of NTFSTool. It is derived from the free NTFSTool on the
Mac App Store and keeps the same architecture, but it is deliberately kept small and readable:

- every source file is commented in English, so you can study how an FSKit file system module, a C bridge on top
  of the NTFS engine, and a sandboxed SwiftUI/AppKit front end fit together;
- the driver uses the simple, direct I/O path (no caching layer or platform-specific performance tuning);
- the app only does the basics (no telemetry, no diagnostics upload, no network access at all);
- there is no licensing, activation or trial code.

If you just want the best experience as a user, install the official builds from [ntfstool.com](https://ntfstool.com).
NTFSTool Pro (closed source) adds a second driver for older macOS versions, disk repair and other tools.

## Features

- Read **and write** NTFS volumes; macOS mounts them automatically through the FSKit driver module
- Sandboxed app; the driver runs as a sandboxed FSKit module in user space
- Shows each NTFS volume's state: read-write, read-only (Windows hibernated / Fast Startup), read-only by
  Apple's built-in driver, not mounted
- Guides you to turn the file system extension on in System Settings
- Open in Finder, eject, disk information
- 20 UI languages
- Extended attributes stored as NTFS named streams (no `._` files); optional `windows_names` mount option
  refuses file names Windows cannot use

## Requirements

- macOS 15.6 or later to run; **write support needs macOS 26 or later** (on macOS 15 the system keeps using
  Apple's read-only NTFS driver for physical disks)
- To build: Xcode 16.3 or later (FSKit SDK) with its command line tools, Swift 5.9+

## Build — one command

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

Other targets:

| Command | What it does |
| --- | --- |
| `make run` | build and open the app |
| `make demo` | run the UI with in-memory demo disks — no driver and no Apple Developer account needed |
| `make test` | run the unit tests |
| `make app UNIVERSAL=1` | build arm64 + x86_64 |
| `make scan` | check that no signing material or private identifiers are in the tree |

The driver tests format and check real NTFS disk images with the `mkntfs` and `ntfsfix` subcommands of the `ntfstool-driver` binary. They are skipped
unless both tools are found in Homebrew's `bin`/`sbin` or in the directory given by `NTFSTOOL_NTFS_TOOLS=<dir>`.

## Running the driver (needs your own Apple Developer account)

The FSKit module uses the restricted entitlement `com.apple.developer.fskit.fsmodule`. macOS only loads a module
whose signature is backed by a provisioning profile that grants it. An ad-hoc build shows the full UI, but the
system will not load the driver. This is an Apple platform rule that applies to every FSKit project.

1. In your Apple Developer account create two App IDs, e.g. `com.yourname.ntfs` and `com.yourname.ntfs.fskit`.
   Enable the **FSKit Module** capability on the second one, and optionally an App Group
   (e.g. `TEAMID.com.yourname.ntfs`) on both so the app can read the driver's log.
2. Create development (or Developer ID) provisioning profiles for both and download them.
3. Build with your identity:

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. Copy `build/NTFSTool.app` to `/Applications`, open it once, then turn the driver on in
   **System Settings → General → Login Items & Extensions → File System Extensions**.
5. Re-plug the NTFS drive. `mount` should now list it with the type `ntfstooloss`.

All build options are documented at the top of [`scripts/build-app.sh`](scripts/build-app.sh).
Provisioning profiles and signing keys never belong in this repository; `make scan` checks for them.

## Layout

| Path | What it is |
| --- | --- |
| `Sources/NTFSTool` | App entry point (`main.swift`) |
| `Sources/NTFSToolApp` | AppKit shell, SwiftUI views, the app model, system services, 20 localizations |
| `Sources/NTFSToolCore` | Pure logic without AppKit: disk model, language list, settings store, local event log |
| `Sources/NTFSToolBridge` | C bridge (`ntfstool_bridge.c`) exposing inode-based operations on top of `libntfstool-driver` |
| `Sources/NTFSToolDriver` | Swift core of the driver (no FSKit dependency, unit-testable on any macOS) |
| `FSKitModule/NTFSToolFS` | The FSKit module (`NTFSToolFS.appex`) that macOS loads to mount NTFS volumes |
| `Tests/` | Unit tests for the app model and for the driver (real NTFS images) |
| `scripts/` | `build-app.sh`, `build-fskit.sh`, `scan-secrets.sh` |

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for how a write travels from Finder to the disk.

## Contributing

Issues and pull requests are welcome. Please sign off your commits (`git commit -s`) to certify the
[Developer Certificate of Origin](https://developercertificate.org/) — see [CONTRIBUTING.md](CONTRIBUTING.md).

## License

MIT License — see [LICENSE](LICENSE) and [NOTICE](NOTICE).
The bundled NTFS engine is NTFSTool's own `libntfstool-driver`, shipped as a prebuilt static library (see
[Vendor/README.md](Vendor/README.md)).

Earlier versions of this repository (the Electron app) remain available in the git history.

## Trademarks

The MIT License gives you the code, not the brand. "NTFSTool" and the NTFSTool icons and logos are trademarks of the
NTFSTool project and are **not** licensed under the MIT License.

- You may build and use this source for yourself, and refer to it as "based on NTFSTool".
- If you **distribute** a modified version or a fork (including on any app store), you must give it a different
  name and icon, remove the NTFSTool logos, and use your own bundle identifier — it must not look like, or claim
  to be, an official NTFSTool release.
- Unmodified builds of this repository may only be redistributed under the NTFSTool name with prior permission.

NTFS and Windows are trademarks of Microsoft Corporation; macOS and FSKit are trademarks of Apple Inc. This
project is not affiliated with Microsoft or Apple.
