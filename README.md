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
