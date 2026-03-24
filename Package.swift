// swift-tools-version:5.9
// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
import PackageDescription

let package = Package(
    name: "NTFSTool",
    defaultLocalization: "en",
    platforms: [.macOS(.v12)],
    targets: [
        // Core logic (no AppKit): volume model, languages, event log shared with the driver.
        .target(name: "NTFSToolCore", path: "Sources/NTFSToolCore"),
        // Prebuilt libntfstool-driver (ntfstool-driver release build): static library + headers.
        .binaryTarget(name: "NTFSToolNTFS", path: "Vendor/NTFSToolNTFS.xcframework"),
        // Inode-based C bridge on top of libntfstool-driver.
        .target(name: "NTFSToolBridge", dependencies: ["NTFSToolNTFS"], path: "Sources/NTFSToolBridge",
                sources: ["ntfstool_bridge.c"], publicHeadersPath: "include",
                cSettings: [.define("HAVE_CONFIG_H")],
                linkerSettings: [.linkedFramework("CoreFoundation")]),
        // Swift core of the FSKit driver (no FSKit dependency, so it builds and tests on macOS 12).
        .target(name: "NTFSToolDriver", dependencies: ["NTFSToolBridge"], path: "Sources/NTFSToolDriver"),
    ]
)
