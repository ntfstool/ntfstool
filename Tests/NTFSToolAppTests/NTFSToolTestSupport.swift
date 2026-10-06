// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolTestSupport.swift
//
// Test doubles shared by the NTFSToolApp tests. Disks and the extension status
// are faked with `DemoDiskSystem` / `DemoDriverStatus` from the app itself;
// this file adds a scripted alert presenter and a recording notifier.

import Foundation
@testable import NTFSToolApp
@testable import NTFSToolCore
import XCTest

/// Records every alert / URL / reveal and answers alerts from a script.
final class ScriptedPresenter: Presenting {
    struct Alert {
        let title: String
        let message: String
        let buttons: [String]
    }

    var alerts: [Alert] = []
    /// Decides which button index each alert "clicks" (default: the first).
    var answer: (Alert) -> Int = { _ in 0 }
    var opened: [URL] = []
    var revealed: [String] = []

    func alert(title: String, message: String, buttons: [String], style: AlertStyle) -> Int {
        let a = Alert(title: title, message: message, buttons: buttons)
        alerts.append(a)
        return answer(a)
    }

    func open(_ url: URL) { opened.append(url) }
    func reveal(_ path: String) { revealed.append(path) }

    /// The fixed part of a localized string before its first `{placeholder}`,
    /// so alerts whose text contains a drive name can still be matched.
    static func stem(_ key: String) -> String { String(L(key).split(separator: "{", maxSplits: 1, omittingEmptySubsequences: false).first ?? "") }

    /// Answers alerts whose title or message contains the text of `key` with button `index`.
    func on(_ key: String, press index: Int) {
        let prev = answer
        let text = ScriptedPresenter.stem(key)
        answer = { a in a.message.contains(text) || a.title.contains(text) ? index : prev(a) }
    }

    var messages: [String] { alerts.map(\.message) }
    /// True if an alert with the text of `key` was shown.
    func shown(_ key: String) -> Bool {
        let t = ScriptedPresenter.stem(key)
        return alerts.contains { $0.message.contains(t) || $0.title.contains(t) }
    }
}

/// Records notifications instead of posting them.
final class TestNotifier: Notifying {
    var sent: [(id: String, title: String, body: String)] = []
    func notify(id: String, title: String, body: String) { sent.append((id, title, body)) }
}

/// Snapshot PNGs from view tests go to .build/snapshots (ignored by git) for manual review.
enum SnapshotTests {
    static let outDir: URL = {
        let u = URL(fileURLWithPath: #filePath).deletingLastPathComponent().deletingLastPathComponent().deletingLastPathComponent()
            .appendingPathComponent(".build/snapshots")
        try? FileManager.default.createDirectory(at: u, withIntermediateDirectories: true)
        return u
    }()
}
