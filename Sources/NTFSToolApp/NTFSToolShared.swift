// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolShared.swift
//
// Part of NTFSToolApp. Small types shared by the model, the views and the
// system-service wrappers: how flows are scheduled (`FlowExecutor`), which
// long-running operation a volume is busy with (`BusyKind`), and the
// notification abstractions (`Notifying`, `InsertionCard`, `InsertionAction`).

import Foundation
import NTFSToolCore

/// Decides where model flows run and where UI state is published.
///
/// In the app, flows run on a background serial queue (DiskArbitration calls
/// block while waiting for their callback) and state changes hop to the main
/// thread. In tests everything runs inline and synchronously, so a test can
/// call `model.makeWritable(v)` and immediately assert on the result.
public struct FlowExecutor {
    /// Runs a whole flow (may block).
    public var run: (@escaping () -> Void) -> Void
    /// Applies a state change that SwiftUI observes (must end up on the main thread).
    public var publish: (@escaping () -> Void) -> Void
    /// Runs a block after a delay (used for polling the extension status).
    public var after: (TimeInterval, @escaping () -> Void) -> Void

    /// The real executor: one background serial queue + main-thread publishing.
    public static let live: FlowExecutor = {
        let q = DispatchQueue(label: "ntfstool.flows", qos: .userInitiated)
        return FlowExecutor(
            run: { q.async(execute: $0) },
            publish: { block in Thread.isMainThread ? block() : DispatchQueue.main.async(execute: block) },
            after: { delay, block in q.asyncAfter(deadline: .now() + delay, execute: block) })
    }()

    /// Synchronous executor for tests: everything runs immediately, delays are ignored.
    public static let inline = FlowExecutor(run: { $0() }, publish: { $0() }, after: { _, b in b() })
}

/// The operation a volume is currently busy with; drives the progress overlay.
public enum BusyKind: String {
    /// Remounting the volume (to make it writable or to mount it).
    case mounting
    /// Ejecting the disk.
    case unmounting
}

/// Posts a simple system notification. `UserNotifier` implements it with
/// UNUserNotificationCenter; tests record the calls instead.
public protocol Notifying: AnyObject {
    func notify(id: String, title: String, body: String)
}

/// A notifier that does nothing (default for tests and demo mode).
public final class NoNotifier: Notifying {
    public init() {}
    public func notify(id: String, title: String, body: String) {}
}

/// The notification shown when a new NTFS drive is plugged in: its text and
/// the buttons it offers. `UserNotifier.post` turns it into a real notification.
public struct InsertionCard: Equatable, CustomStringConvertible {
    public enum Kind: Equatable {
        /// Mounted by our driver; `readOnly` is true when Windows was not fully shut down.
        case mounted(readOnly: Bool)
        /// Not writable yet (Apple's read-only driver, or not mounted): offer "Make Writable".
        case needsAction
    }
    public var volumeID: String
    public var name: String
    public var kind: Kind
    public var description: String { "\(volumeID):\(kind)" }
}

/// What the user chose on an insertion notification.
public enum InsertionAction: String, CaseIterable, Equatable {
    /// Clicked the notification itself: bring up the main window and select the disk.
    case show
    case open, eject, mount

    /// Button title (`show` has no button).
    public var title: String {
        switch self {
        case .show: return ""
        case .open: return L("open")
        case .eject: return L("eject")
        case .mount: return L("make_writable")
        }
    }
}

public extension InsertionCard {
    var title: String {
        switch kind {
        case .mounted(let ro): return L(ro ? "card_mounted_ro_title" : "card_mounted_title", ["name": name])
        case .needsAction: return L("card_needs_title", ["name": name])
        }
    }

    var body: String {
        switch kind {
        case .mounted(let ro): return ro ? L("banner_ro_hibernated") : L("card_mounted_body")
        case .needsAction: return L("card_needs_body")
        }
    }

    /// Notification buttons, most important first.
    var actions: [InsertionAction] {
        switch kind {
        case .mounted: return [.open, .eject]
        case .needsAction: return [.mount]
        }
    }

    /// Notification Center needs one registered category per distinct set of buttons.
    var categoryID: String { "ntfstool.card." + actions.map(\.rawValue).joined(separator: "-") }
}
