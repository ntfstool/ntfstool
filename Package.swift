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
    ]
)
