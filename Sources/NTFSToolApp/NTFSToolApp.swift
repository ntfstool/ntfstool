// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolApp.swift
//
// Part of NTFSToolApp: the AppKit shell. Starts the application, decides
// between real disks and demo mode, builds the menu bar and the main window,
// and connects system events (disk changes, app activation, notification
// clicks) to the model.
//
// Architecture overview:
//
//   NTFSToolCore   pure logic, no AppKit (volume model, languages, helpers)
//   NTFSToolApp    this layer: AppKit shell + SwiftUI views + NTFSToolModel
//   FSKit module   the actual NTFS driver, loaded by macOS (not by this app)
//
// System services are injected into the model through protocols
// (NTFSToolDiskSystem, NTFSToolDriverStatusProviding, Presenting, Notifying),
// so tests and demo mode run the same UI without real disks.
//
// Environment variables (all optional):
//
//   NTFSTOOL_DEMO=1            use in-memory demo disks instead of real ones
//   NTFSTOOL_DEMO_SCENE=name   demo scene: guide | readonly | empty (default: three drives)
//   NTFSTOOL_DEMO_SELECT=id    select a demo volume, e.g. disk6s1
//   NTFSTOOL_DEMO_LANG=lproj   force a UI language, e.g. ja, zh-Hans, ar
//   NTFSTOOL_SNAPSHOT=path     render the main window to a PNG and quit
//   NTFSTOOL_SNAPSHOT_DELAY=s  seconds to wait before the snapshot (default 1.5)

import AppKit
import NTFSToolCore
import SwiftUI

/// Entry point called from `main.swift`.
public enum NTFSToolAppMain {
    public static func run() {
        let app = NSApplication.shared
        let delegate = NTFSToolAppDelegate()
        app.delegate = delegate
        app.setActivationPolicy(.regular)
        // NSApplication holds its delegate weakly; keep it alive for the whole run.
        withExtendedLifetime(delegate) { app.run() }
    }
}

/// Demo scenes: in-memory disks and extension status for screenshots,
/// UI development and tests.
public enum NTFSToolDemo {
    /// Builds the disk system and extension status for a scene.
    /// - `nil` / unknown name: three drives (two writable, one unmounted), extension on.
    /// - `"guide"`: extension off, one drive mounted read-only by Apple's driver.
    /// - `"readonly"`: adds a drive that Windows left hibernated (read-only).
    /// - `"empty"`: no drives.
    public static func scene(_ name: String?) -> (DemoDiskSystem, DemoDriverStatus) {
        let sys = DemoDiskSystem()
        let drv = DemoDriverStatus(.ready)
        switch name {
        case "guide":
            drv.current = .needsEnable
            sys.driverEnabled = false
            sys.disks[0].state = .readOnlySystem
            sys.disks[0].volume.isWritable = false
            sys.disks[0].volume.fileSystemType = "ntfs"
            sys.disks.removeLast()
        case "readonly":
            sys.disks.insert(DemoDiskSystem.Disk(volume: DemoDiskSystem.vol("disk7s1", "WIN-GAMES", size: 1_000_204_886_016, used: 640_000_000_000,
                                                                           writable: false), state: .readOnlyHibernated), at: 0)
            sys.disks[0].volume.fileSystemType = NTFSToolDriverNames.openSource
        case "empty":
            sys.disks = []
        default:
            break
        }
        return (sys, drv)
    }
}

/// The application delegate: owns the model, the window and the menus.
final class NTFSToolAppDelegate: NSObject, NSApplicationDelegate, NSWindowDelegate {
    var model: NTFSToolModel!
    var window: NSWindow!
    let presenter = AlertPresenter()
    let notifier = UserNotifier()
    var observers: [NSObjectProtocol] = []
    var liveSystem: LiveDiskSystem?
    var mainController: NTFSToolWindowController?
    var aboutController: NTFSToolAboutWindowController?

    /// Reads a string from the app's Info.plist (e.g. the version).
    static func info(_ k: String) -> String { Bundle.main.object(forInfoDictionaryKey: k) as? String ?? "" }

    func applicationDidFinishLaunching(_ notification: Notification) {
        let env = ProcessInfo.processInfo.environment
        L10n.shared.use(env["NTFSTOOL_DEMO_LANG"].flatMap(AppLanguage.byLproj) ?? AppLanguage.match(preferred: Locale.preferredLanguages))
        let demo = env["NTFSTOOL_DEMO"] == "1"
        if demo {
            let (sys, drv) = NTFSToolDemo.scene(env["NTFSTOOL_DEMO_SCENE"])
            model = NTFSToolModel(system: sys, driver: drv, presenter: presenter, exec: .inline)
        } else {
            notifier.requestAuthorization()
            let live = LiveDiskSystem()
            liveSystem = live
            model = NTFSToolModel(system: live, driver: LiveDriverStatus(), presenter: presenter, exec: .live, notifier: notifier)
        }
        // Wire notifications: new drives post a card (not in demo mode), and
        // clicks on cards or plain notifications come back to the model / window.
        model.onInsertion = { [weak self] card in if !demo { self?.notifier.post(card) } }
        model.showWindow = { [weak self] in self?.window.makeKeyAndOrderFront(nil); NSApp.activate(ignoringOtherApps: true) }
        notifier.onCardAction = { [weak self] id, action in self?.model.performInsertion(volumeID: id, action: action) }
        notifier.onOpen = { [weak self] in self?.model.showWindow() }
        buildMenu()
        buildWindow()
        model.refresh()
        if demo, let sel = env["NTFSTOOL_DEMO_SELECT"] { model.selectedID = sel }
        if !demo { observeSystem() }
        if let snap = env["NTFSTOOL_SNAPSHOT"] {
            Snapshot.schedule(window: window, path: snap, delay: Double(env["NTFSTOOL_SNAPSHOT_DELAY"] ?? "") ?? 1.5, then: nil)
        }
    }

    /// Creates and shows the main window.
    func buildWindow() {
        let c = NTFSToolWindowController(model: model)
        mainController = c
        window = c.window
        window.delegate = self
        c.showWindow(nil)
        window.makeKeyAndOrderFront(nil)
        NSApp.activate(ignoringOtherApps: true)
    }

    /// Refreshes the model whenever disks change or the user returns to the app.
    func observeSystem() {
        liveSystem?.inventory.onChange = { [weak self] in self?.model.diskChanged() }
        let nc = NSWorkspace.shared.notificationCenter
        for n in [NSWorkspace.didMountNotification, NSWorkspace.didUnmountNotification, NSWorkspace.didRenameVolumeNotification] {
            observers.append(nc.addObserver(forName: n, object: nil, queue: .main) { [weak self] _ in
                // DiskArbitration's description lags slightly behind the mount event.
                DispatchQueue.main.asyncAfter(deadline: .now() + 0.8) { self?.model.diskChanged() }
            })
        }
        // Coming back from System Settings is the typical moment the extension was just enabled.
        observers.append(NotificationCenter.default.addObserver(forName: NSApplication.didBecomeActiveNotification, object: nil, queue: .main) { [weak self] _ in
            self?.model.appDidBecomeActive()
        })
    }

    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool { true }

    func applicationShouldHandleReopen(_ sender: NSApplication, hasVisibleWindows flag: Bool) -> Bool {
        if !flag { window.makeKeyAndOrderFront(nil) }
        return true
    }

    // MARK: Menus

    /// Builds the menu bar in code (there is no nib): App, Edit, Window, Help.
    func buildMenu() {
        let main = NSMenu()
        let appItem = NSMenuItem()
        main.addItem(appItem)
        let appMenu = NSMenu()
        appMenu.addItem(withTitle: L("menu_about"), action: #selector(about), keyEquivalent: "").target = self
        appMenu.addItem(.separator())
        appMenu.addItem(withTitle: L("menu_hide"), action: #selector(NSApplication.hide(_:)), keyEquivalent: "h")
        appMenu.addItem(.separator())
        appMenu.addItem(withTitle: L("menu_quit"), action: #selector(NSApplication.terminate(_:)), keyEquivalent: "q")
        appItem.submenu = appMenu

        let editItem = NSMenuItem()
        main.addItem(editItem)
        let edit = NSMenu(title: L("menu_edit"))
        edit.addItem(withTitle: L("menu_copy"), action: #selector(NSText.copy(_:)), keyEquivalent: "c")
        edit.addItem(withTitle: L("menu_select_all"), action: #selector(NSText.selectAll(_:)), keyEquivalent: "a")
        editItem.submenu = edit

        let winItem = NSMenuItem()
        main.addItem(winItem)
        let win = NSMenu(title: L("menu_window"))
        win.addItem(withTitle: L("menu_minimize"), action: #selector(NSWindow.performMiniaturize(_:)), keyEquivalent: "m")
        win.addItem(withTitle: L("menu_close"), action: #selector(NSWindow.performClose(_:)), keyEquivalent: "w")
        winItem.submenu = win
        NSApp.windowsMenu = win

        let helpItem = NSMenuItem()
        main.addItem(helpItem)
        let help = NSMenu(title: L("menu_help_title"))
        help.addItem(withTitle: L("menu_help"), action: #selector(openHelp), keyEquivalent: "?").target = self
        help.addItem(withTitle: L("menu_website"), action: #selector(openWebsite), keyEquivalent: "").target = self
        help.addItem(withTitle: L("about_github"), action: #selector(openGitHub), keyEquivalent: "").target = self
        helpItem.submenu = help
        NSApp.helpMenu = help
        NSApp.mainMenu = main
    }

    @objc func about() {
        if aboutController == nil { aboutController = NTFSToolAboutWindowController(model: model, version: NTFSToolAppDelegate.info("CFBundleShortVersionString")) }
        aboutController?.show()
    }

    @objc func openHelp() { model.openHelp() }
    @objc func openWebsite() { model.openWebsite() }
    @objc func openGitHub() { model.openGitHub() }
}
