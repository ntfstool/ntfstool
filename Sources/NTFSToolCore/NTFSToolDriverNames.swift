// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolDriverNames.swift
//
// Part of NTFSToolCore: pure logic with no AppKit dependency.
//
// When macOS mounts a volume through an FSKit module, the file system type that
// `statfs` / DiskArbitration report for that mount is the module's *short name*
// (configured in the module's Info.plist). That is how the app tells "mounted
// by our read-write driver" apart from "mounted by Apple's built-in read-only
// NTFS driver", whose type is plain `ntfs`.

import Foundation

/// Short names under which our FSKit NTFS module can appear in the mount table.
public enum NTFSToolDriverNames {
    /// The short name of the module built from this repository.
    public static let openSource = "ntfstooloss"

    /// All names that identify a volume mounted by our read-write NTFS module.
    ///
    /// - `ntfstooloss`: the module built from this open-source repository.
    /// - `ntfstool`: the App Store build of the same driver.
    /// - `ntfspro`: the NTFSTool Pro build of the same driver.
    ///
    /// Each build uses its own name so they can be installed side by side
    /// without their extensions colliding; for this app any of them means
    /// "mounted by our driver".
    public static let fskitNTFSNames: Set<String> = [openSource, "ntfstool", "ntfspro"]
}
