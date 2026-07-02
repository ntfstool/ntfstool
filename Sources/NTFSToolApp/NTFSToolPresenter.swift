// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolPresenter.swift
//
// Part of NTFSToolApp. Everything the model shows outside its own window goes
// through `Presenting`: modal alerts, opening a URL in the browser and
// revealing a folder in Finder. The model depends only on the protocol, so
// tests inject a scripted presenter that records alerts and picks answers.

import AppKit
import NTFSToolCore

/// Icon / severity of an alert.
public enum AlertStyle {
    case info, warning, critical
}

/// Side effects that leave the main window.
public protocol Presenting: AnyObject {
    /// Shows a modal alert and returns the index of the clicked button
    /// (0 = the first button, which is the default one).
    func alert(title: String, message: String, buttons: [String], style: AlertStyle) -> Int
    /// Opens a URL (web page or System Settings pane).
    func open(_ url: URL)
    /// Opens a folder in Finder.
    func reveal(_ path: String)
}

/// The real presenter based on `NSAlert` and `NSWorkspace`.
/// May be called from any thread: alerts hop to the main thread synchronously.
public final class AlertPresenter: Presenting {
    /// Runs the alert modally (replaceable in tests).
    public var runModal: (NSAlert) -> NSApplication.ModalResponse = { alert in
        NSApp.activate(ignoringOtherApps: true)
        return alert.runModal()
    }
    /// Opens URLs (replaceable in tests).
    public var workspaceOpen: (URL) -> Void = { url in NSWorkspace.shared.open(url) }
    public init() {}

    static func onMain<T>(_ block: () -> T) -> T {
        Thread.isMainThread ? block() : DispatchQueue.main.sync(execute: block)
    }

    static func makeAlert(_ title: String, _ message: String, _ style: AlertStyle) -> NSAlert {
        let a = NSAlert()
        a.messageText = title
        a.informativeText = message
        switch style {
        case .info: a.alertStyle = .informational
        case .warning: a.alertStyle = .warning
        case .critical: a.alertStyle = .critical
        }
        return a
    }

    public func alert(title: String, message: String, buttons: [String], style: AlertStyle) -> Int {
        AlertPresenter.onMain {
            let a = AlertPresenter.makeAlert(title, message, style)
            for b in buttons { a.addButton(withTitle: b) }
            let r = runModal(a)
            return r.rawValue - NSApplication.ModalResponse.alertFirstButtonReturn.rawValue
        }
    }

    public func open(_ url: URL) {
        let o = workspaceOpen
        DispatchQueue.main.async { o(url) }
    }

    /// Opening a folder URL with NSWorkspace shows it in Finder; this works in
    /// the sandbox for mount points under /Volumes.
    public func reveal(_ path: String) { open(URL(fileURLWithPath: path)) }
}
