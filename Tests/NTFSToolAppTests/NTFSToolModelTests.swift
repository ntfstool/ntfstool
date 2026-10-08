// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolModelTests.swift
//
// Tests for NTFSToolModel and the pieces around it. Everything runs against
// the in-memory `DemoDiskSystem` / `DemoDriverStatus` with the inline
// executor, so each flow completes synchronously inside the test.

import AppKit
@testable import NTFSToolApp
@testable import NTFSToolCore
import SwiftUI
import XCTest

/// Flows: listing, make writable, reconnect fallback, eject, the extension guide, links.
final class NTFSToolModelTests: XCTestCase {
    var sys: DemoDiskSystem!
    var drv: DemoDriverStatus!
    var presenter: ScriptedPresenter!
    var notifier: TestNotifier!
    var model: NTFSToolModel!

    override func setUp() {
        L10n.shared.use(AppLanguage.byLproj("en")!)
        sys = DemoDiskSystem()
        drv = DemoDriverStatus(.ready)
        presenter = ScriptedPresenter()
        notifier = TestNotifier()
        make()
    }

    func make() {
        model = NTFSToolModel(system: sys, driver: drv, presenter: presenter, exec: .inline, notifier: notifier)
        model.refresh()
    }

    func v(_ id: String) -> NTFSToolVolume { model.volumes.first { $0.id == id }! }

    /// Adds a drive mounted by Apple's read-only driver (connected before the extension was on).
    func addSystemRO(_ id: String = "disk8s1", physical: Bool) {
        var vol = DemoDiskSystem.vol(id, "OLD-NTFS", size: 64_000_000_000, used: 10_000_000_000, writable: false)
        vol.isWritable = false
        sys.disks.append(DemoDiskSystem.Disk(volume: vol, state: .readOnlySystem, physical: physical))
        model.refresh()
    }

    func testListsDrivesSelectsFirstAndReportsDriver() {
        XCTAssertTrue(model.loaded)
        XCTAssertEqual(model.volumes.map(\.id), ["disk4s1", "disk5s2", "disk6s1"])
        XCTAssertEqual(model.selected?.id, "disk4s1")
        XCTAssertEqual(model.driver, .ready)
        XCTAssertFalse(model.showsGuide)
        model.selectedID = "disk6s1"
        sys.disks.removeAll { $0.volume.id == "disk6s1" }
        model.diskChanged()
        XCTAssertEqual(model.selectedID, "disk4s1", "selection falls back to the first drive when the selected one is removed")
        sys.disks = []
        model.appDidBecomeActive()
        XCTAssertNil(model.selected)
    }

    func testMakeWritableRemountsThroughSystemWhenAllowed() {
        addSystemRO(physical: false)
        model.makeWritable(v("disk8s1"))
        XCTAssertEqual(sys.calls, ["unmount disk8s1", "mount disk8s1"])
        XCTAssertEqual(v("disk8s1").state, .writable)
        XCTAssertEqual(v("disk8s1").volume.fileSystemType, NTFSToolDriverNames.openSource)
        XCTAssertEqual(notifier.sent.last?.title, L("notify_writable_title"))
        XCTAssertTrue(presenter.alerts.isEmpty)
        model.makeWritable(v("disk8s1"))
        XCTAssertEqual(sys.calls.count, 2, "an already writable drive is left alone")
    }

    func testPhysicalDriveIsEjectedForReconnecting() {
        addSystemRO(physical: true)
        model.makeWritable(v("disk8s1"))
        XCTAssertTrue(presenter.shown("reconnect_title"))
        XCTAssertTrue(presenter.messages.last!.contains("OLD-NTFS"))
        XCTAssertEqual(sys.calls, ["unmount disk8s1", "mount disk8s1", "eject disk8"])
        XCTAssertFalse(model.volumes.contains { $0.id == "disk8s1" })
        XCTAssertEqual(notifier.sent.last?.title, L("notify_ejected_title"))
    }

    func testReconnectDeclinedMountsBack() {
        addSystemRO(physical: true)
        presenter.on("reconnect_title", press: 1)
        model.makeWritable(v("disk8s1"))
        XCTAssertEqual(sys.calls, ["unmount disk8s1", "mount disk8s1", "mount disk8s1"], "the drive should not be left unmounted")
        XCTAssertTrue(model.volumes.contains { $0.id == "disk8s1" })
    }

    func testUnmountBusyExplains() {
        final class BusySystem: DemoDiskSystem {
            override func unmount(_ id: String) -> NTFSToolDiskResult { _ = super.unmount(id); return .busy }
        }
        sys = BusySystem()
        make()
        addSystemRO(physical: false)
        model.makeWritable(v("disk8s1"))
        XCTAssertTrue(presenter.shown("busy_title"))
        XCTAssertFalse(sys.calls.contains("mount disk8s1"))
    }

    func testUnmountFailureOffersReconnect() {
        final class FailingSystem: DemoDiskSystem {
            override func unmount(_ id: String) -> NTFSToolDiskResult { .failed("x") }
        }
        sys = FailingSystem()
        make()
        addSystemRO(physical: true)
        presenter.on("reconnect_title", press: 1)
        model.makeWritable(v("disk8s1"))
        XCTAssertTrue(presenter.shown("reconnect_title"))
        XCTAssertEqual(sys.calls, [], "nothing was unmounted, so nothing is mounted back")
    }

    func testUnmountedDriveMountsAndHibernatedOneExplains() {
        let d = sys.disks.firstIndex { $0.volume.id == "disk6s1" }!
        sys.disks[d].physical = false
        model.mount(v("disk6s1"))
        XCTAssertEqual(v("disk6s1").state, .writable)

        sys.hibernated = ["disk6s1"]
        _ = sys.unmount("disk6s1")
        model.refresh()
        model.mount(v("disk6s1"))
        XCTAssertEqual(v("disk6s1").state, .readOnlyHibernated)
        XCTAssertTrue(presenter.shown("hibernated_title"))
        let n = sys.calls.count
        model.makeWritable(v("disk6s1"))
        XCTAssertEqual(sys.calls.count, n, "a hibernated drive is only explained, never remounted again")
        XCTAssertEqual(presenter.alerts.filter { $0.title == L("hibernated_title") }.count, 2)
    }

    func testEjectResults() {
        model.eject(v("disk4s1"))
        XCTAssertEqual(sys.calls, ["eject disk4"])
        XCTAssertFalse(model.volumes.contains { $0.id == "disk4s1" })
        sys.ejectResult = .busy
        model.eject(v("disk5s2"))
        XCTAssertTrue(presenter.shown("busy_title"))
        sys.ejectResult = .failed("x")
        model.eject(v("disk5s2"))
        XCTAssertTrue(presenter.shown("eject_failed_title"))
        sys.ejectResult = .notPrivileged
        model.eject(v("disk5s2"))
        XCTAssertEqual(presenter.alerts.filter { $0.title == L("eject_failed_title") }.count, 2)
    }

    func testOpenRevealsMountedOnly() {
        model.open(v("disk4s1"))
        model.open(v("disk6s1"))
        XCTAssertEqual(presenter.revealed, ["/Volumes/BACKUP-2T"])
    }

    func testGuideOpensSettingsWaitsAndThenFixesReadOnlyDrives() {
        drv.current = .needsEnable
        sys.driverEnabled = false
        addSystemRO(physical: false)
        XCTAssertTrue(model.showsGuide)
        drv.enableAfterChecks = 3
        sys.driverEnabled = true
        model.makeWritable(v("disk8s1"))
        XCTAssertEqual(drv.opened, 1, "extension off: guide the user to turn it on first")
        XCTAssertTrue(presenter.opened.isEmpty, "settings opened directly, no URL needed")
        XCTAssertFalse(model.guideWaiting, "the switch is detected automatically")
        XCTAssertEqual(model.driver, .ready)
        XCTAssertEqual(v("disk8s1").state, .writable, "read-only drives are remounted once the switch is on")
        XCTAssertTrue(notifier.sent.contains { $0.title == L("notify_driver_ready_title") })
    }

    func testGuideFallsBackToSettingsURLAndTimesOut() {
        drv.current = .needsEnable
        drv.direct = false
        model.refresh()
        model.startGuide()
        XCTAssertEqual(presenter.opened, [NTFSToolModel.settingsURL])
        XCTAssertFalse(model.guideDirect)
        XCTAssertFalse(model.guideWaiting, "polling stops after the limit")
        XCTAssertEqual(model.pollCount, NTFSToolModel.guidePollLimit)
        XCTAssertTrue(model.showsGuide, "the card stays while the extension is off")
    }

    func testCancelGuideStopsPolling() {
        drv.current = .needsEnable
        let exec = FlowExecutor(run: { $0() }, publish: { $0() }, after: { [unowned self] _, b in
            self.model.cancelGuide()
            b()
        })
        model = NTFSToolModel(system: sys, driver: drv, presenter: presenter, exec: exec)
        model.startGuide()
        XCTAssertEqual(model.pollCount, 0)
        XCTAssertFalse(model.guideWaiting)
    }

    func testUnknownDriverStatusSuggestsGuideOnlyWhenSomethingIsReadOnly() {
        drv.current = .unknown
        model.refresh()
        XCTAssertFalse(model.suggestsGuide)
        addSystemRO(physical: true)
        XCTAssertTrue(model.suggestsGuide)
        XCTAssertFalse(model.showsGuide)
    }

    func testLinksOpenOnlyOnRequest() {
        XCTAssertTrue(presenter.opened.isEmpty, "nothing is opened without a click")
        model.openWebsite()
        model.openHelp()
        model.openGitHub()
        model.openProInfo()
        XCTAssertEqual(presenter.opened, [NTFSToolModel.websiteURL, NTFSToolModel.helpURL, NTFSToolModel.githubURL, NTFSToolModel.proURL])
        XCTAssertEqual(NTFSToolModel.websiteURL.host, "ntfstool.com")
        XCTAssertEqual(NTFSToolModel.proURL.query, "utm_source=github_opensource")
    }

    func testProLinkOnlyWhereThisAppCannotHelp() {
        let (s, _) = NTFSToolDemo.scene("readonly")
        let states = s.volumes().map { NTFSToolVolume(volume: $0.volume, state: $0.state) }
        for v in states {
            XCTAssertEqual(model.showsProLink(for: v), v.state == .readOnlyHibernated, "\(v.id) \(v.state)")
        }
        var ro = states[0]
        ro.state = .readOnlySystem
        XCTAssertFalse(model.showsProLink(for: ro), "the extension guide handles this case")
    }

    func testUserFacingStringsHaveNoPurchaseWording() {
        let banned = ["buy", "purchase", "upgrade", "trial", "price", "$", "pay", "license", "app store"]
        L10n.shared.use(AppLanguage.byLproj("en")!)
        for key in L10n.shared.allKeys {
            let s = L(key).lowercased()
            for b in banned { XCTAssertFalse(s.contains(b), "\(key) contains \(b): \(s)") }
        }
    }

    func testEveryLanguageHasEveryKey() {
        L10n.shared.use(AppLanguage.byLproj("en")!)
        let keys = Set(L10n.load("en").keys)
        XCTAssertTrue(keys.contains("pro_link"))
        XCTAssertEqual(L10n.availableLocalizations().count, AppLanguage.all.count)
        for lang in AppLanguage.all {
            XCTAssertEqual(Set(L10n.load(lang.lproj).keys), keys, lang.lproj)
        }
    }

    func testLanguageMatching() {
        XCTAssertEqual(AppLanguage.match(preferred: ["es-AR"]).lproj, "es")
        XCTAssertEqual(AppLanguage.match(preferred: ["zh-Hant-TW"]).lproj, "zh-Hant")
        XCTAssertEqual(AppLanguage.match(preferred: ["zh-HK"]).lproj, "zh-Hant")
        XCTAssertEqual(AppLanguage.match(preferred: ["zh_CN"]).lproj, "zh-Hans")
        XCTAssertEqual(AppLanguage.match(preferred: ["xx", "de-CH"]).lproj, "de")
        XCTAssertEqual(AppLanguage.match(preferred: []).lproj, "en")
        XCTAssertTrue(AppLanguage.byLproj("ar")!.isRTL)
        XCTAssertFalse(AppLanguage.byLproj("en")!.isRTL)
    }

    func testClassifier() {
        let w = DemoDiskSystem.vol("disk1s1", "A", size: 10, used: 1, writable: true)
        let ro = DemoDiskSystem.vol("disk1s1", "A", size: 10, used: 1, writable: false)
        let un = DemoDiskSystem.vol("disk1s1", "A", size: 10, used: 1, mounted: false, writable: false)
        XCTAssertEqual(NTFSToolStateClassifier.state(of: w, fskitType: "ntfstooloss"), .writable)
        XCTAssertEqual(NTFSToolStateClassifier.state(of: ro, fskitType: "ntfstooloss"), .readOnlyHibernated)
        XCTAssertEqual(NTFSToolStateClassifier.state(of: ro, fskitType: nil), .readOnlySystem)
        XCTAssertEqual(NTFSToolStateClassifier.state(of: un, fskitType: nil), .unmounted)
        XCTAssertTrue(NTFSToolStateClassifier.visible(w))
        XCTAssertFalse(NTFSToolStateClassifier.visible(DiskVolume(id: "disk3s1", mountPoint: "/", isSystem: true)))
        XCTAssertFalse(NTFSToolStateClassifier.visible(DiskVolume(id: "disk9s1", fileSystemType: "exfat")))
        XCTAssertEqual(NTFSToolDriverNames.fskitNTFSNames, ["ntfstooloss", "ntfstool", "ntfspro"])
        XCTAssertEqual(DiskVolume.wholeDisk(of: "disk12s3"), "disk12")
        XCTAssertEqual(DiskVolume.wholeDisk(of: "weird"), "weird")
    }

    func testDiskArbitrationResultMapping() {
        let b = LiveDiskSystem.Box()
        XCTAssertEqual(LiveDiskSystem.result(b, timedOut: false), .ok)
        XCTAssertEqual(LiveDiskSystem.result(b, timedOut: true), .failed("timeout"))
        b.status = DAReturn(kDAReturnNotPrivileged)
        XCTAssertEqual(LiveDiskSystem.result(b, timedOut: false), .notPrivileged)
        b.status = DAReturn(kDAReturnBusy)
        XCTAssertEqual(LiveDiskSystem.result(b, timedOut: false), .busy)
        b.status = 1
        b.message = "nope"
        XCTAssertEqual(LiveDiskSystem.result(b, timedOut: false), .failed("nope"))
        b.message = nil
        XCTAssertEqual(LiveDiskSystem.result(b, timedOut: false), .failed("0x00000001"))
    }

    func testLiveSystemReportsErrorsForMissingDisks() {
        let live = LiveDiskSystem()
        _ = live.volumes()
        // A disk that does not exist: DiskArbitration refuses, and the failure is passed on.
        XCTAssertNotEqual(live.unmount("disk999"), .ok)
        XCTAssertNotEqual(live.mount("disk999"), .ok)
        XCTAssertNotEqual(live.eject("disk999"), .ok)
    }

    func testDemoScenes() {
        XCTAssertEqual(NTFSToolDemo.scene("guide").1.current, .needsEnable)
        XCTAssertEqual(NTFSToolDemo.scene("guide").0.disks.first?.state, .readOnlySystem)
        XCTAssertEqual(NTFSToolDemo.scene("readonly").0.disks.first?.state, .readOnlyHibernated)
        XCTAssertTrue(NTFSToolDemo.scene("empty").0.disks.isEmpty)
        XCTAssertEqual(NTFSToolDemo.scene(nil).0.disks.count, 3)
        let s = DemoDiskSystem(disks: [])
        s.plugIn(DemoDiskSystem.vol("disk9s1", "X", size: 1, used: 0, writable: false))
        XCTAssertEqual(s.volumes().first?.state, .writable)
        s.driverEnabled = false
        s.plugIn(DemoDiskSystem.vol("disk10s1", "Y", size: 1, used: 0, writable: false))
        XCTAssertEqual(s.volumes().last?.state, .readOnlySystem)
        XCTAssertEqual(s.mount("nope"), .failed("no_disk"))
        XCTAssertEqual(DemoDriverStatus(.needsEnable).status(), .needsEnable)
    }
}

/// Renders every demo scene in English, Simplified Chinese and Arabic (mirrored)
/// to PNGs under .build/snapshots for visual review.
final class NTFSToolSnapshotTests: XCTestCase {
    override func tearDown() { L10n.shared.use(AppLanguage.byLproj("en")!) }

    func render<V: View>(_ view: V, _ name: String, size: NSSize = NSSize(width: 900, height: 600)) throws {
        let host = NSHostingView(rootView: view.frame(width: size.width, height: size.height))
        host.frame = NSRect(origin: .zero, size: size)
        let window = NSWindow(contentRect: host.frame, styleMask: [.titled], backing: .buffered, defer: false)
        window.contentView = host
        host.layoutSubtreeIfNeeded()
        RunLoop.main.run(until: Date().addingTimeInterval(0.1))
        let rep = try XCTUnwrap(host.bitmapImageRepForCachingDisplay(in: host.bounds))
        host.cacheDisplay(in: host.bounds, to: rep)
        try rep.representation(using: .png, properties: [:])?.write(to: SnapshotTests.outDir.appendingPathComponent(name + ".png"))
    }

    func model(_ scene: String?) -> NTFSToolModel {
        let (s, d) = NTFSToolDemo.scene(scene)
        let m = NTFSToolModel(system: s, driver: d, presenter: ScriptedPresenter(), exec: .inline)
        m.refresh()
        return m
    }

    func testAllScenesRender() throws {
        guard #available(macOS 13.0, *) else { return }
        for lang in ["en", "zh-Hans", "ar"] {
            L10n.shared.use(AppLanguage.byLproj(lang)!)
            for scene in ["main", "guide", "readonly", "empty"] {
                let m = model(scene)
                try render(NTFSToolRootView(model: m), "\(scene)-\(lang)")
                try render(NTFSToolDetailView(model: m), "detail-\(scene)-\(lang)", size: NSSize(width: 760, height: 640))
                try render(NTFSToolSidebar(model: m), "sidebar-\(scene)-\(lang)", size: NSSize(width: 240, height: 600))
            }
            let m = model("main")
            m.selectedID = "disk6s1"
            try render(NTFSToolDetailContent(model: m, v: m.selected!), "unmounted-\(lang)", size: NSSize(width: 760, height: 600))
            let g = model("guide")
            g.startGuide()
            try render(NTFSToolGuideCard(model: g), "guide-waiting-\(lang)", size: NSSize(width: 640, height: 260))
            let u = model("main")
            (u.driverStatus as? DemoDriverStatus)?.current = .unknown
            u.refresh()
            try render(NTFSToolGuideCard(model: u), "guide-unknown-\(lang)", size: NSSize(width: 640, height: 160))
        }
        let ab = model("main")
        try render(NTFSToolAboutView(model: ab, version: "1.0"), "about", size: NSSize(width: 440, height: 380))
        XCTAssertEqual(NTFSToolText.state(.unmounted), L("state_not_mounted"))
        XCTAssertEqual(NTFSToolText.color(.writable), .green)
        XCTAssertEqual(BusyText.of(.mounting), L("busy_mounting"))
    }
}

/// Parsing DiskArbitration descriptions (the sandbox-friendly way to list disks).
final class NTFSToolDiskInfoTests: XCTestCase {
    /// A description captured from a real NTFS disk image mounted by Apple's read-only driver.
    static let appleRO: [String: Any] = [
        "DAMediaBSDName": "disk6s1", "DAMediaContent": "Windows_NTFS", "DAMediaLeaf": true, "DAMediaWhole": false, "DAMediaSize": 314_528_256,
        "DAMediaName": "Untitled 1", "DAMediaRemovable": true, "DAMediaWritable": true, "DAVolumeKind": "ntfs", "DAVolumeName": "TESTVOL",
        "DAVolumePath": URL(fileURLWithPath: "/Volumes/TESTVOL/"), "DADeviceModel": "Disk Image", "DADeviceProtocol": "Virtual Interface",
        "DAVolumeType": "MS-DOS (FAT12)",
    ]

    func stat(_ fs: String, ro: Bool) -> (String) -> NTFSToolMountStat? {
        { _ in NTFSToolMountStat(fsType: fs, readOnly: ro, total: 314_000_000, free: 300_000_000) }
    }

    func testAppleReadOnlyMount() throws {
        let v = try XCTUnwrap(NTFSToolDiskInfo.volume(Self.appleRO, stat: stat("ntfs", ro: true)))
        XCTAssertEqual(v.state, .readOnlySystem)
        XCTAssertEqual(v.volume.id, "disk6s1")
        XCTAssertEqual(v.volume.wholeDisk, "disk6")
        XCTAssertEqual(v.volume.name, "TESTVOL")
        XCTAssertEqual(v.volume.mountPoint, "/Volumes/TESTVOL")
        XCTAssertEqual(v.volume.totalSize, 314_528_256)
        XCTAssertEqual(v.volume.freeSpace, 300_000_000)
        XCTAssertEqual(v.volume.busProtocol, "Disk Image", "Virtual Interface is shown as a disk image")
        XCTAssertFalse(v.volume.isWritable)
    }

    func testOurModuleMountsAndHibernated() throws {
        for name in NTFSToolDriverNames.fskitNTFSNames {
            var d = Self.appleRO
            d["DAVolumeKind"] = name
            d["DADeviceProtocol"] = "USB"
            d["DADeviceModel"] = "Extreme 55AE"
            d["DAVolumeUUID"] = CFUUIDCreateFromString(nil, "6A1C7E2F-3B7D-4C1A-9F2E-000000000001" as CFString)!
            let w = try XCTUnwrap(NTFSToolDiskInfo.volume(d, stat: stat(name, ro: false)))
            XCTAssertEqual(w.state, .writable, name)
            XCTAssertTrue(w.volume.isWritable)
            XCTAssertEqual(w.volume.busProtocol, "USB")
            XCTAssertEqual(w.volume.mediaName, "Extreme 55AE")
            XCTAssertEqual(w.volume.volumeUUID, "6A1C7E2F-3B7D-4C1A-9F2E-000000000001")
            XCTAssertEqual(NTFSToolDiskInfo.volume(d, stat: stat(name, ro: true))?.state, .readOnlyHibernated, name)
        }
    }

    func testUnmountedAndFiltering() {
        var u = Self.appleRO
        u["DAVolumeKind"] = nil
        u["DAVolumePath"] = nil
        XCTAssertEqual(NTFSToolDiskInfo.volume(u, stat: { _ in nil })?.state, .unmounted, "unmounted Windows_NTFS partition")
        var gpt = u
        gpt["DAMediaContent"] = "EBD0A0A2-B9E5-4433-87C0-68B6B72699C7"
        XCTAssertNil(NTFSToolDiskInfo.volume(gpt, stat: { _ in nil }), "an unmounted GPT basic data partition may be exFAT; not listed")
        var exfat = Self.appleRO
        exfat["DAVolumeKind"] = "exfat"
        XCTAssertNil(NTFSToolDiskInfo.volume(exfat, stat: stat("exfat", ro: false)))
        var whole = Self.appleRO
        whole["DAMediaLeaf"] = false
        XCTAssertNil(NTFSToolDiskInfo.volume(whole, stat: stat("ntfs", ro: true)), "partition maps are not listed")
        var root = Self.appleRO
        root["DAVolumePath"] = URL(fileURLWithPath: "/")
        XCTAssertNil(NTFSToolDiskInfo.volume(root, stat: stat("ntfs", ro: true)))
        XCTAssertNil(NTFSToolDiskInfo.volume([:], stat: { _ in nil }))
        var noName = Self.appleRO
        noName["DAVolumeName"] = nil
        XCTAssertEqual(NTFSToolDiskInfo.volume(noName, stat: stat("ntfs", ro: true))?.volume.name, "Untitled 1")
    }

    func testLiveInventoryEnumeratesRealDisksWithoutCrashing() {
        let inv = DAInventory()
        _ = inv.volumes()   // works whether or not this Mac has NTFS drives
        var changed = false
        inv.onChange = { changed = true }
        inv.fire()
        RunLoop.main.run(until: Date().addingTimeInterval(0.8))
        XCTAssertTrue(changed, "callbacks within 0.5 s are coalesced into one change")
        XCTAssertNotNil(NTFSToolDiskInfo.liveStat("/"))
        XCTAssertNil(NTFSToolDiskInfo.liveStat("/nonexistent-\(UUID().uuidString)"))
    }
}
