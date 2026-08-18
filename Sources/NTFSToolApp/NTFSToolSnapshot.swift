// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolSnapshot.swift
//
// Part of NTFSToolApp. Window screenshots for checking the UI.
//
// `NTFSTOOL_SNAPSHOT=<file.png>` makes the app render its main window into a
// PNG and quit. The window is drawn into a bitmap with AppKit's own caching
// API, so no Screen Recording permission is needed. Combine it with demo mode
// (`NTFSTOOL_DEMO=1`, `NTFSTOOL_DEMO_SCENE`, `NTFSTOOL_DEMO_LANG`) to capture
// any state in any language without real disks.

import AppKit

/// Renders a window to PNG.
public enum Snapshot {
    /// PNG data of the window including its title bar and toolbar
    /// (the frame view is the content view's superview).
    public static func render(_ window: NSWindow) -> Data? {
        guard let view = window.contentView?.superview ?? window.contentView else { return nil }
        guard let rep = view.bitmapImageRepForCachingDisplay(in: view.bounds) else { return nil }
        view.cacheDisplay(in: view.bounds, to: rep)
        return rep.representation(using: .png, properties: [:])
    }

    /// After `delay` seconds (time for the first refresh and layout), writes the
    /// PNG to `path`, then calls `then`, or exits the process when `then` is nil
    /// (exit code 0 on success, 3 if the window could not be rendered).
    public static func schedule(window: NSWindow, path: String, delay: TimeInterval, then: (() -> Void)?) {
        DispatchQueue.main.asyncAfter(deadline: .now() + delay) {
            window.makeKeyAndOrderFront(nil)
            DispatchQueue.main.asyncAfter(deadline: .now() + 0.6) {
                let data = render(window)
                if let d = data { try? d.write(to: URL(fileURLWithPath: path)) }
                if let t = then { t() } else { exit(data != nil ? 0 : 3) }
            }
        }
    }
}
