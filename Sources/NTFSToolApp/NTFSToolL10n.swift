// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolL10n.swift
//
// Part of NTFSToolApp. Localization and byte formatting.
//
// Strings are looked up with `L("key")` from Resources/<lproj>/Localizable.strings
// (20 languages). Instead of relying on the system's AppleLanguages mechanism,
// `L10n` loads the chosen table itself, which makes it possible to pick the
// language at runtime (e.g. `NTFSTOOL_DEMO_LANG=ja` for screenshots) and in tests.
// Missing keys fall back to English, then to the key itself.

import Foundation
import NTFSToolCore

/// The active string table.
public final class L10n {
    public static let shared = L10n()

    private var table: [String: String] = [:]
    private var fallback: [String: String] = [:]
    /// The language currently in use.
    public private(set) var language: AppLanguage = AppLanguage.byLproj("en")!

    init() {
        fallback = L10n.load("en")
        table = fallback
    }

    /// Loads `<lproj>.lproj/Localizable.strings` from the SwiftPM resource bundle.
    static func load(_ lproj: String) -> [String: String] {
        guard let url = Bundle.module.url(forResource: "Localizable", withExtension: "strings", subdirectory: nil, localization: lproj)
            ?? Bundle.module.path(forResource: lproj, ofType: "lproj").map({ URL(fileURLWithPath: $0).appendingPathComponent("Localizable.strings") }),
            let dict = NSDictionary(contentsOf: url) as? [String: String] else { return [:] }
        return dict
    }

    /// Switches the language. Views created afterwards use the new strings.
    public func use(_ lang: AppLanguage) {
        language = lang
        table = lang.lproj == "en" ? fallback : L10n.load(lang.lproj)
    }

    /// True if the key exists in the current or the English table.
    public func has(_ key: String) -> Bool { table[key] != nil || fallback[key] != nil }
    /// All keys known in the current or the English table.
    public var allKeys: [String] { Array(Set(table.keys).union(fallback.keys)) }

    /// Looks up `key` and substitutes `{name}`-style placeholders from `args`.
    public func string(_ key: String, _ args: [String: CustomStringConvertible] = [:]) -> String {
        var s = table[key] ?? fallback[key] ?? key
        for (k, v) in args { s = s.replacingOccurrences(of: "{\(k)}", with: v.description) }
        return s
    }

    /// Languages that actually have a non-empty table in the bundle.
    public static func availableLocalizations() -> [String] {
        AppLanguage.all.map(\.lproj).filter { !load($0).isEmpty }
    }
}

/// Shorthand for `L10n.shared.string(key, args)`.
public func L(_ key: String, _ args: [String: CustomStringConvertible] = [:]) -> String {
    L10n.shared.string(key, args)
}

/// Formats byte counts the way Finder does (decimal units: 1 GB = 10^9 bytes).
public enum ByteFormat {
    public static func string(_ bytes: Int64) -> String {
        let f = ByteCountFormatter()
        f.countStyle = .file
        f.allowedUnits = [.useKB, .useMB, .useGB, .useTB]
        return f.string(fromByteCount: bytes)
    }
}
