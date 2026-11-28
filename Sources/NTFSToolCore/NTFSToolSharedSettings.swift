// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolSharedSettings.swift
//
// Settings the app writes and the FSKit module reads when it mounts a volume.
// The app is sandboxed and never mounts anything itself (DiskArbitration starts
// the module), so mount options cannot be passed along; they go through a small
// JSON file in the App Group container both processes share.
//
// Without an App Group (unsigned or ad-hoc builds) there is no shared file: the
// app hides the settings and the module uses the defaults.
//
// Foundation only: this file is compiled into the FSKit module too (scripts/build-fskit.sh).

import Foundation

public struct SharedSettings: Equatable {
    /// Match names ignoring case, like Windows. Off by default (POSIX, case-sensitive).
    public var caseInsensitive = false

    public init(caseInsensitive: Bool = false) { self.caseInsensitive = caseInsensitive }

    /// `Library/Application Support/settings.json` in the App Group container, or nil
    /// when `group` is empty or not available to this process.
    public static func groupFile(group: String) -> URL? {
        group.isEmpty ? nil : FileManager.default.containerURL(forSecurityApplicationGroupIdentifier: group)?
            .appendingPathComponent("Library/Application Support/settings.json")
    }

    /// Reads the settings; a missing or unreadable file gives the defaults.
    public static func load(_ url: URL?) -> SharedSettings {
        guard let url = url, let data = try? Data(contentsOf: url),
              let d = (try? JSONSerialization.jsonObject(with: data)) as? [String: Any] else { return SharedSettings() }
        return SharedSettings(caseInsensitive: d["case_insensitive"] as? Bool ?? false)
    }

    /// Writes the settings atomically.
    public func save(_ url: URL) throws {
        try FileManager.default.createDirectory(at: url.deletingLastPathComponent(), withIntermediateDirectories: true)
        let data = try JSONSerialization.data(withJSONObject: ["case_insensitive": caseInsensitive], options: [.sortedKeys])
        try data.write(to: url, options: .atomic)
    }
}
