// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolAbout.swift
//
// Part of NTFSToolApp. The About window: icon, name, version, a one-line
// description, links to the website and the GitHub repository (opened in the
// browser on click).

import AppKit
import NTFSToolCore
import SwiftUI

/// Content of the About window.
struct NTFSToolAboutView: View {
    @ObservedObject var model: NTFSToolModel
    let version: String

    var year: String { String(Calendar(identifier: .gregorian).component(.year, from: Date())) }

    var body: some View {
        VStack(spacing: 10) {
            Image(nsImage: NSApp.applicationIconImage).resizable().frame(width: 96, height: 96)
            Text("NTFSTool").font(.system(size: 20, weight: .semibold))
            Text(L("version") + " " + version).font(.callout).foregroundColor(.secondary)
            Text(L("about_text")).font(.callout).multilineTextAlignment(.center)
            Button("ntfstool.com") { model.openWebsite() }.buttonStyle(.link).accessibilityIdentifier("about-website")
            HStack(spacing: 10) {
                Button(L("about_visit")) { model.openWebsite() }.keyboardShortcut(.defaultAction)
                Button(L("about_github")) { model.openGitHub() }
            }
            .controlSize(.regular)
            .padding(.top, 4)
                .fixedSize(horizontal: false, vertical: true)
            Text("© 2019-\(year) NTFSTool").font(.caption).foregroundColor(.secondary)
        }
        .padding(.horizontal, 28).padding(.vertical, 22)
        .frame(width: 440)
        .appLayoutDirection()
    }
}

/// A small fixed-size window hosting `NTFSToolAboutView`.
final class NTFSToolAboutWindowController: NSWindowController {
    convenience init(model: NTFSToolModel, version: String) {
        let w = NSWindow(contentViewController: NSHostingController(rootView: NTFSToolAboutView(model: model, version: version)))
        w.styleMask = [.titled, .closable]
        w.title = L("menu_about")
        w.isReleasedWhenClosed = false
        self.init(window: w)
    }

    func show() {
        window?.center()
        showWindow(nil)
        NSApp.activate(ignoringOtherApps: true)
    }
}
