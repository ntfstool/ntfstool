// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolWindow.swift
//
// Part of NTFSToolApp. The AppKit main window around the SwiftUI views.
//
// It is built like Disk Utility: an `NSSplitViewController` with a real
// sidebar item (which gets the system sidebar material, and the "Liquid Glass"
// look on newer macOS automatically) plus a unified toolbar with labeled
// buttons. AppKit is used for the frame because these system behaviors are
// hard to reproduce with a pure SwiftUI window while still supporting macOS 12
// era APIs. The window's height follows its content so the detail pane never
// needs a scroll bar.

import AppKit
import Combine
import NTFSToolCore
import SwiftUI

/// Owns the main window, its toolbar and the content sizing.
public final class NTFSToolWindowController: NSWindowController, NSToolbarDelegate, NSWindowDelegate {
    let model: NTFSToolModel
    private var bag: Set<AnyCancellable> = []
    /// Toolbar items by identifier, so their enabled state can be updated.
    private(set) var items: [NSToolbarItem.Identifier: NSToolbarItem] = [:]

    static let writableItem = NSToolbarItem.Identifier("ntfstool.writable")
    static let openItem = NSToolbarItem.Identifier("ntfstool.open")
    static let ejectItem = NSToolbarItem.Identifier("ntfstool.eject")
    static let refreshItem = NSToolbarItem.Identifier("ntfstool.refresh")

    public init(model: NTFSToolModel) {
        self.model = model
        let split = NSSplitViewController()
        let sidebar = NSSplitViewItem(sidebarWithViewController: NSHostingController(rootView: NTFSToolSidebar(model: model)))
        sidebar.minimumThickness = 210
        sidebar.maximumThickness = 320
        sidebar.canCollapse = true
        let content = NSSplitViewItem(viewController: NSHostingController(rootView: NTFSToolDetailView(model: model)))
        content.minimumThickness = 620
        split.addSplitViewItem(sidebar)
        split.addSplitViewItem(content)
        split.splitView.autosaveName = "ntfstool.main.split"

        let window = NSWindow(contentRect: NSRect(x: 0, y: 0, width: 1060, height: 560),
                              styleMask: [.titled, .closable, .miniaturizable, .resizable, .fullSizeContentView], backing: .buffered, defer: false)
        window.contentViewController = split
        window.titleVisibility = .visible
        window.toolbarStyle = .unified
        window.minSize = NSSize(width: 860, height: 360)
        window.setFrameAutosaveName("ntfstool.main.window")   // remembers position across launches
        window.isReleasedWhenClosed = false
        super.init(window: window)
        window.delegate = self
        window.title = "NTFSTool"
        let tb = NSToolbar(identifier: "ntfstool.main.toolbar")
        tb.delegate = self
        tb.displayMode = .iconAndLabel   // labels under the icons, like Disk Utility
        tb.allowsUserCustomization = false
        window.toolbar = tb
        if window.frame.origin == .zero { window.center() }

        // Toolbar buttons and the subtitle follow the selection and busy state.
        model.$selectedID.combineLatest(model.$volumes, model.$busy).receive(on: DispatchQueue.main)
            .sink { [weak self] _ in self?.refreshToolbar() }.store(in: &bag)
        // Re-fit the height when the page content may have changed (debounced
        // so a burst of updates causes one resize).
        model.$volumes.combineLatest(model.$selectedID, model.$driver, model.$guideWaiting).receive(on: DispatchQueue.main)
            .debounce(for: .milliseconds(120), scheduler: DispatchQueue.main)
            .sink { [weak self] _ in self?.fitToContent() }.store(in: &bag)
        fitToContent()
    }

    required init?(coder: NSCoder) { nil }

    // MARK: Fitting the window height to the content

    /// Current width of the detail column.
    var detailWidth: CGFloat {
        guard let split = window?.contentViewController as? NSSplitViewController, let content = window?.contentView else { return 700 }
        let sidebar = split.splitViewItems.first.map { $0.isCollapsed ? 0 : max($0.viewController.view.frame.width, 210) } ?? 0
        return max(520, content.frame.width - sidebar)
    }

    /// Height taken by title bar + toolbar inside the full-size content view.
    func toolbarHeight() -> CGFloat {
        guard let w = window, let content = w.contentView else { return 52 }
        return max(0, content.frame.height - w.contentLayoutRect.height)
    }

    /// Height the current page (selected drive or empty state) needs at the
    /// current width, measured with an off-screen hosting view.
    func detailHeight() -> CGFloat {
        let view: AnyView = model.selected.map { AnyView(NTFSToolDetailContent(model: model, v: $0)) } ?? AnyView(NTFSToolEmptyContent(model: model))
        return ceil(NSHostingView(rootView: view.frame(width: detailWidth)).fittingSize.height)
    }

    /// Height the drive list needs (capped for very long lists).
    func sidebarHeight() -> CGFloat {
        ceil(min(NSHostingView(rootView: NTFSToolSidebar(model: model).frame(width: 240)).fittingSize.height, 2000))
    }

    /// The content height that shows everything without scrolling.
    func requiredContentHeight() -> CGFloat {
        max(detailHeight() + toolbarHeight(), sidebarHeight() + toolbarHeight(), 360)
    }

    /// Resizes the window to `requiredContentHeight`, keeping the title bar in
    /// place (only the bottom edge moves) and staying on screen.
    func fitToContent() {
        guard let w = window, let content = w.contentView, model.loaded else { return }
        let screen = (w.screen ?? NSScreen.main)?.visibleFrame ?? NSRect(x: 0, y: 0, width: 1440, height: 900)
        let border = w.frame.height - content.frame.height
        let need = min(requiredContentHeight(), screen.height - border)
        w.contentMinSize = NSSize(width: max(w.contentMinSize.width, 860), height: need)
        var f = w.frame
        let delta = need - content.frame.height
        guard abs(delta) > 0.5 else { return }
        f.size.height += delta
        f.origin.y -= delta
        if f.minY < screen.minY { f.origin.y = screen.minY }
        if f.maxY > screen.maxY { f.origin.y = screen.maxY - f.height }
        w.setFrame(f, display: true, animate: w.isVisible)
    }

    /// The labeled toolbar's real height is only known once the window is on
    /// screen, so fit once more after showing it.
    override public func showWindow(_ sender: Any?) {
        super.showWindow(sender)
        DispatchQueue.main.async { [weak self] in self?.fitToContent() }
    }

    // MARK: Toolbar

    public func toolbarAllowedItemIdentifiers(_ toolbar: NSToolbar) -> [NSToolbarItem.Identifier] { toolbarDefaultItemIdentifiers(toolbar) }

    public func toolbarDefaultItemIdentifiers(_ toolbar: NSToolbar) -> [NSToolbarItem.Identifier] {
        [.sidebarTrackingSeparator, .toggleSidebar, .flexibleSpace, Self.writableItem, Self.openItem, Self.ejectItem, .space, Self.refreshItem]
    }

    /// The system creates the sidebar toggle; marking it navigational places it
    /// at the leading edge of the content area, before the title.
    public func toolbarWillAddItem(_ notification: Notification) {
        guard let item = notification.userInfo?["item"] as? NSToolbarItem, item.itemIdentifier == .toggleSidebar else { return }
        item.isNavigational = true
    }

    func makeItem(_ id: NSToolbarItem.Identifier, symbol: String, label: String, action: Selector) -> NSToolbarItem {
        let item = NSToolbarItem(itemIdentifier: id)
        item.image = NSImage(systemSymbolName: symbol, accessibilityDescription: label)
        item.label = label
        item.paletteLabel = label
        item.toolTip = label
        item.target = self
        item.action = action
        item.isBordered = true
        // Enabled state is set by refreshToolbar; stop AppKit from re-enabling items.
        item.autovalidates = false
        items[id] = item
        return item
    }

    public func toolbar(_ toolbar: NSToolbar, itemForItemIdentifier id: NSToolbarItem.Identifier, willBeInsertedIntoToolbar flag: Bool) -> NSToolbarItem? {
        switch id {
        case Self.writableItem: return makeItem(id, symbol: "lock.open", label: L("make_writable"), action: #selector(makeWritable))
        case Self.openItem: return makeItem(id, symbol: "folder", label: L("show_in_finder"), action: #selector(openInFinder))
        case Self.ejectItem: return makeItem(id, symbol: "eject", label: L("eject"), action: #selector(eject))
        case Self.refreshItem: return makeItem(id, symbol: "arrow.clockwise", label: L("refresh"), action: #selector(refresh))
        default: return nil
        }
    }

    /// Enables the buttons that make sense for the selected drive and shows its name as subtitle.
    func refreshToolbar() {
        let v = model.selected
        let idle = v.map { model.busy[$0.id] == nil } ?? false
        items[Self.writableItem]?.isEnabled = idle && (v?.state == .readOnlySystem || v?.state == .unmounted)
        items[Self.openItem]?.isEnabled = idle && (v?.volume.isMounted ?? false)
        items[Self.ejectItem]?.isEnabled = idle && v != nil
        window?.subtitle = v?.volume.displayName ?? ""
    }

    @objc func makeWritable() { if let v = model.selected { model.makeWritable(v) } }
    @objc func openInFinder() { if let v = model.selected { model.open(v) } }
    @objc func eject() { if let v = model.selected { model.eject(v) } }
    @objc func refresh() { model.refresh() }
}
