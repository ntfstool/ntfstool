// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolLanguage.swift
//
// Part of NTFSToolCore: pure logic with no AppKit dependency.
//
// The list of the 20 interface languages the app ships, and the rule for
// picking one from the user's preferred languages. The actual string tables
// live in NTFSToolApp/Resources/<lproj>/Localizable.strings and are loaded by
// `L10n` in the app layer.

import Foundation

/// One supported interface language.
public struct AppLanguage: Equatable {
    /// Name of the resource folder (without `.lproj`), e.g. `zh-Hans`.
    public let lproj: String
    /// The language's name written in that language, e.g. "Deutsch".
    public let nativeName: String

    /// All supported languages, English first.
    /// (CJK names are written as Unicode escapes so the source stays ASCII-friendly:
    /// they read "Simplified Chinese", "Traditional Chinese" and "Japanese".)
    public static let all: [AppLanguage] = [
        .init(lproj: "en", nativeName: "English"),
        .init(lproj: "zh-Hans", nativeName: "\u{7B80}\u{4F53}\u{4E2D}\u{6587}"),
        .init(lproj: "zh-Hant", nativeName: "\u{7E41}\u{9AD4}\u{4E2D}\u{6587}"),
        .init(lproj: "ja", nativeName: "\u{65E5}\u{672C}\u{8A9E}"),
        .init(lproj: "ko", nativeName: "한국어"),
        .init(lproj: "de", nativeName: "Deutsch"),
        .init(lproj: "fr", nativeName: "Français"),
        .init(lproj: "es", nativeName: "Español"),
        .init(lproj: "it", nativeName: "Italiano"),
        .init(lproj: "pt", nativeName: "Português"),
        .init(lproj: "ru", nativeName: "Русский"),
        .init(lproj: "tr", nativeName: "Türkçe"),
        .init(lproj: "ar", nativeName: "العربية"),
        .init(lproj: "fa", nativeName: "فارسی"),
        .init(lproj: "hi", nativeName: "हिन्दी"),
        .init(lproj: "bn", nativeName: "বাংলা"),
        .init(lproj: "id", nativeName: "Bahasa Indonesia"),
        .init(lproj: "vi", nativeName: "Tiếng Việt"),
        .init(lproj: "th", nativeName: "ไทย"),
        .init(lproj: "sw", nativeName: "Kiswahili"),
    ]

    /// Arabic and Persian are written right to left; the UI is mirrored for them.
    public var isRTL: Bool { ["ar", "fa"].contains(lproj) }

    /// Looks a language up by its resource folder name.
    public static func byLproj(_ code: String) -> AppLanguage? { all.first { $0.lproj == code } }

    /// Picks the best supported language for a list of BCP 47 tags such as
    /// `Locale.preferredLanguages` (e.g. `["es-AR", "en-US"]`).
    ///
    /// Only the primary subtag is compared (`es-AR` -> Spanish), except for
    /// Chinese, where the script/region decides between Simplified and
    /// Traditional. Falls back to English when nothing matches.
    public static func match(preferred: [String]) -> AppLanguage {
        for raw in preferred {
            let tag = raw.replacingOccurrences(of: "_", with: "-")
            let parts = tag.split(separator: "-").map(String.init)
            guard let primary = parts.first?.lowercased() else { continue }
            if primary == "zh" {
                let rest = parts.dropFirst().map { $0.lowercased() }
                if rest.contains("hant") || rest.contains("tw") || rest.contains("hk") || rest.contains("mo") { return byLproj("zh-Hant")! }
                return byLproj("zh-Hans")!
            }
            if let l = byLproj(primary) { return l }
        }
        return byLproj("en")!
    }
}
