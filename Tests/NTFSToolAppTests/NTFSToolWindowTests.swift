// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolWindowTests.swift
//
// Tests for the AppKit main window: labeled toolbar with a navigational
// sidebar button, subtitle and button states following the selection, and
// the window height fitting the current page exactly.

import AppKit
@testable import NTFSToolApp
@testable import NTFSToolCore
import XCTest

final class NTFSToolWindowTests: XCTestCase {
    var model: NTFSToolModel!
    var c: NTFSToolWindowController!

    override func setUp() {
        _ = NSApplication.shared
        L10n.shared.use(AppLanguage.byLproj("en")!)
        let (sys, drv) = NTFSToolDemo.scene(nil)
        model = NTFSToolModel(system: sys, driver: drv, presenter: ScriptedPresenter(), exec: .inline)
        model.refresh()
        c = NTFSToolWindowController(model: model)
        c.showWindow(nil)
        RunLoop.main.run(until: Date().addingTimeInterval(0.4))
    }

    override func tearDown() { c.window?.close() }

    func testToolbarIsLabeledWithNavigationalSidebarButton() throws {
        let tb = try XCTUnwrap(c.window?.toolbar)
        XCTAssertEqual(tb.displayMode, .iconAndLabel)
        let ids = tb.items.map(\.itemIdentifier)
        XCTAssertEqual(ids.first, .sidebarTrackingSeparator)
        XCTAssertTrue(ids.contains(.toggleSidebar))
        XCTAssertTrue(tb.items.first { $0.itemIdentifier == .toggleSidebar }?.isNavigational ?? false, "sidebar toggle sits at the leading edge of the content")
        for id in [NTFSToolWindowController.writableItem, NTFSToolWindowController.openItem, NTFSToolWindowController.ejectItem, NTFSToolWindowController.refreshItem] {
            XCTAssertFalse(c.items[id]?.label.isEmpty ?? true, "\(id.rawValue) has a label")
        }
    }

    func testSubtitleAndItemStatesFollowSelection() {
        XCTAssertEqual(c.window?.subtitle, "BACKUP-2T")
        XCTAssertFalse(c.items[NTFSToolWindowController.writableItem]!.isEnabled, "already writable")
        XCTAssertTrue(c.items[NTFSToolWindowController.openItem]!.isEnabled)
        model.selectedID = "disk6s1"
        RunLoop.main.run(until: Date().addingTimeInterval(0.3))
        XCTAssertEqual(c.window?.subtitle, "Camera SSD")
        XCTAssertTrue(c.items[NTFSToolWindowController.writableItem]!.isEnabled, "an unmounted drive can be mounted")
        XCTAssertFalse(c.items[NTFSToolWindowController.openItem]!.isEnabled)
        c.refresh()
        c.openInFinder()
        c.makeWritable()
        c.eject()
    }

    func testWindowHeightFitsCurrentPageExactly() throws {
        let w = try XCTUnwrap(c.window)
        c.fitToContent()
        RunLoop.main.run(until: Date().addingTimeInterval(0.4))
        XCTAssertEqual(w.contentView!.frame.height, c.requiredContentHeight(), accuracy: 2, "no scrolling, no empty space at the bottom")
        let first = w.frame.height
        // The guide card makes the page taller; the window grows with it.
        (model.driverStatus as? DemoDriverStatus)?.current = .needsEnable
        model.refresh()
        RunLoop.main.run(until: Date().addingTimeInterval(0.6))
        XCTAssertGreaterThan(w.frame.height, first + 40)
        XCTAssertEqual(w.contentView!.frame.height, c.requiredContentHeight(), accuracy: 2)
    }
}
