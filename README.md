# NTFSTool (open-source edition)

Free, open-source read-write NTFS for macOS, built on Apple's **FSKit** and NTFSTool's own NTFS engine (**libntfstool-driver**).

Plug in a Windows (NTFS) drive and macOS mounts it writable through NTFSTool's FSKit driver module — no kernel
extension, no macFUSE, no reduced security settings, no admin password. The app itself is fully sandboxed.

[简体中文说明](README-CN.md) · [Architecture guide](docs/ARCHITECTURE.md)

![NTFSTool](Resources/Icon/preview.png)

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
