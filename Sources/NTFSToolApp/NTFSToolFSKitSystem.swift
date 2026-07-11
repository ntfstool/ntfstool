// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolFSKitSystem.swift
//
// Part of NTFSToolApp. The only place that calls the FSKit framework.
//
// FSKit is the user-space file system framework macOS uses to load our NTFS
// module. Apps can use `FSClient` to list installed file system extensions
// (including whether the user has enabled them) and, on newer systems, to
// open the settings page where the switch lives. The calls are guarded with
// `#available` and `canImport` so the package still builds against older SDKs.

import Foundation
#if canImport(FSKit)
import FSKit
#endif
import NTFSToolCore

/// Thin wrappers around `FSClient`. Only meaningful on a real Mac; tests and
/// demo mode use `DemoDriverStatus` instead.
enum FSKitSystem {
    /// Opens System Settings directly at the File System Extensions list.
    /// Returns false when this macOS version has no API for that, in which case
    /// the caller opens the general Login Items & Extensions pane by URL.
    static func openExtensionsSettings() -> Bool {
        #if canImport(FSKit)
        if #available(macOS 27, *) { return FSClient.shared.openFileSystemExtensionsSettings() }
        #endif
        return false
    }

    /// Whether the extension with `bundleID` is enabled, or nil if that cannot
    /// be determined (macOS before 26, extension not found, or no answer within
    /// `timeout`).
    ///
    /// `fetchInstalledExtensions` is asynchronous; this waits for it with a
    /// semaphore. Its completion may be delivered on the main thread, so waiting
    /// on the main thread could deadlock: in that case the function returns nil
    /// immediately. The model always calls it from a background flow.
    static func isEnabled(bundleID: String, timeout: TimeInterval = 3) -> Bool? {
        if Thread.isMainThread { return nil }
        #if canImport(FSKit)
        if #available(macOS 26, *) {
            let sem = DispatchSemaphore(value: 0)
            var found: Bool?
            FSClient.shared.fetchInstalledExtensions { list, _ in
                found = list?.first { $0.bundleIdentifier == bundleID }?.isEnabled
                sem.signal()
            }
            guard sem.wait(timeout: .now() + timeout) == .success else { return nil }
            return found
        }
        #endif
        return nil
    }
}
