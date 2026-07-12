// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolModel.swift
//
// Part of NTFSToolApp. The view model: every user-visible flow lives here.
//
// The views (SwiftUI) and the AppKit shell only read the model's published
// state and call its methods. The model in turn touches the outside world only
// through injected protocols:
//
//   NTFSToolDiskSystem             - list / mount / unmount / eject disks
//   NTFSToolDriverStatusProviding  - is the file system extension enabled?
//   Presenting                     - alerts, opening URLs, revealing in Finder
//   Notifying                      - system notifications
//   FlowExecutor                   - which thread flows run on
//
// That is what lets the unit tests and demo mode drive the full UI logic with
// in-memory disks and scripted alerts.

import AppKit
import Combine
import Foundation
import NTFSToolCore

/// State and actions behind the main window.
public final class NTFSToolModel: ObservableObject {
    /// Project website.
    public static let websiteURL = URL(string: "https://ntfstool.com")!
    /// Online help.
    public static let helpURL = URL(string: "https://ntfstool.com/help.html")!
    /// Source code.
    public static let githubURL = URL(string: "https://github.com/ntfstool/ntfstool")!
    /// Information page about NTFSTool Pro, the separate edition that can also
    /// handle cases this free app cannot (see `showsProLink`).
    public static let proURL = URL(string: "https://ntfstool.com/?utm_source=github_opensource")!
    /// System Settings > General > Login Items & Extensions; used when the
    /// system cannot open the File System Extensions list directly.
    public static let settingsURL = URL(string: "x-apple.systempreferences:com.apple.LoginItems-Settings.extension")!
    /// While the guide waits for the user, the extension status is checked every
    /// `guidePollInterval` seconds, at most `guidePollLimit` times (10 minutes).
    public static let guidePollInterval: TimeInterval = 2
    public static let guidePollLimit = 300

    /// NTFS volumes, sorted by name.
    @Published public private(set) var volumes: [NTFSToolVolume] = []
    /// The volume selected in the sidebar (nil = first one).
    @Published public var selectedID: String?
    /// Extension status from the last refresh.
    @Published public private(set) var driver: NTFSToolDriverStatus = .unknown
    /// Volumes with an operation in progress (shows a progress overlay).
    @Published public private(set) var busy: [String: BusyKind] = [:]
    /// False until the first refresh finished (shows a spinner instead of "no drives").
    @Published public private(set) var loaded = false
    /// True after "Open System Settings" was clicked, while waiting for the switch.
    @Published public private(set) var guideWaiting = false
    /// True if System Settings opened directly at the extension list (changes the step text).
    @Published public private(set) var guideDirect = false

    let system: NTFSToolDiskSystem
    let driverStatus: NTFSToolDriverStatusProviding
    let presenter: Presenting
    let exec: FlowExecutor
    let notifier: Notifying
    /// Number of status checks made by the current guide session.
    var pollCount = 0

    /// Called (on the main thread) when a new drive appears after launch.
    public var onInsertion: (InsertionCard) -> Void = { _ in }
    /// Brings the main window to the front (set by the app delegate).
    public var showWindow: () -> Void = {}
    /// Volume ids seen in the previous refresh; nil before the first one, so
    /// drives that were already connected at launch do not trigger notifications.
    var insertionSeen: Set<String>?

    public init(system: NTFSToolDiskSystem, driver: NTFSToolDriverStatusProviding, presenter: Presenting, exec: FlowExecutor = .live,
                notifier: Notifying = NoNotifier()) {
        self.system = system
        driverStatus = driver
        self.presenter = presenter
        self.exec = exec
        self.notifier = notifier
    }

    /// The selected volume, falling back to the first one.
    public var selected: NTFSToolVolume? { volumes.first { $0.id == selectedID } ?? volumes.first }
    /// Show the "turn on the extension" card: it is known to be off, or the guide is running.
    public var showsGuide: Bool { driver == .needsEnable || guideWaiting }
    /// When the status cannot be queried (older macOS), a volume mounted by
    /// Apple's read-only driver is the best hint that the extension is off,
    /// so a short version of the card is shown.
    public var suggestsGuide: Bool { driver == .unknown && volumes.contains { $0.state == .readOnlySystem } }

    /// Whether to show the quiet "Learn about NTFSTool Pro" link for a volume.
    /// Only where this app genuinely cannot help: the drive is read-only because
    /// Windows was not fully shut down, which this app does not repair.
    public func showsProLink(for v: NTFSToolVolume) -> Bool { v.state == .readOnlyHibernated }

    func ui(_ block: @escaping () -> Void) { exec.publish(block) }

    // MARK: Refresh

    /// Reloads volumes and the extension status (asynchronously in the app).
    public func refresh() { exec.run { self.refreshNow() } }

    /// Reloads synchronously on the current (flow) thread and publishes the result.
    @discardableResult
    func refreshNow() -> [NTFSToolVolume] {
        let vols = system.volumes()
        let drv = driverStatus.status()
        noteInsertions(vols)
        ui {
            self.volumes = vols
            self.driver = drv
            self.loaded = true
            // Keep the selection valid when the selected drive goes away.
            if let id = self.selectedID, !vols.contains(where: { $0.id == id }) { self.selectedID = vols.first?.id }
            if self.selectedID == nil { self.selectedID = vols.first?.id }
        }
        return vols
    }

    /// Emits an insertion card for every volume that was not there last time.
    func noteInsertions(_ vols: [NTFSToolVolume]) {
        if let seen = insertionSeen {
            for v in vols where !seen.contains(v.id) {
                let kind: InsertionCard.Kind
                switch v.state {
                case .writable: kind = .mounted(readOnly: false)
                case .readOnlyHibernated: kind = .mounted(readOnly: true)
                case .readOnlySystem, .unmounted: kind = .needsAction
                }
                let card = InsertionCard(volumeID: v.id, name: v.volume.displayName, kind: kind)
                ui { self.onInsertion(card) }
            }
        }
        insertionSeen = Set(vols.map(\.id))
    }

    func setBusy(_ id: String, _ kind: BusyKind?) { ui { self.busy[id] = kind } }

    func alert(_ title: String, _ message: String, style: AlertStyle = .warning) {
        _ = presenter.alert(title: L(title), message: L(message), buttons: [L("done")], style: style)
    }

}
