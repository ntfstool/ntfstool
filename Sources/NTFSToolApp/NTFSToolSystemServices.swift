// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolSystemServices.swift
//
// Part of NTFSToolApp. System notifications via UNUserNotificationCenter.
// The model only knows the `Notifying` protocol and `InsertionCard`; this
// class turns them into real notifications and routes button clicks back.

import AppKit
import NTFSToolCore
import UserNotifications

/// Posts local notifications (nothing leaves the Mac) and reports clicks.
public final class UserNotifier: NSObject, Notifying, UNUserNotificationCenterDelegate {
    private var requested = false
    /// Called when a plain notification (not an insertion card) is clicked.
    public var onOpen: () -> Void = {}
    /// Called with (volume id, action) when an insertion card or one of its buttons is clicked.
    public var onCardAction: (String, InsertionAction) -> Void = { _, _ in }
    private var categories: [String: UNNotificationCategory] = [:]

    /// Notification Center only works inside a real .app bundle; calling it from
    /// a command-line process (such as `swift test`) raises an exception.
    public static var available: Bool { Bundle.main.bundleURL.pathExtension == "app" }

    public override init() {
        super.init()
        if UserNotifier.available { UNUserNotificationCenter.current().delegate = self }
    }

    /// Asks once for permission to show alerts.
    public func requestAuthorization() {
        guard UserNotifier.available, !requested else { return }
        requested = true
        UNUserNotificationCenter.current().requestAuthorization(options: [.alert, .sound]) { _, _ in }
    }

    public func notify(id: String, title: String, body: String) {
        guard UserNotifier.available else { return }
        requestAuthorization()
        let c = UNMutableNotificationContent()
        c.title = title
        c.body = body
        UNUserNotificationCenter.current().add(UNNotificationRequest(identifier: id + "-\(Date().timeIntervalSince1970)", content: c, trigger: nil))
    }

    /// Shows an insertion card with its buttons. The identifier is per volume,
    /// so a newer card for the same drive replaces the older one.
    public func post(_ card: InsertionCard) {
        guard UserNotifier.available else { return }
        requestAuthorization()
        let center = UNUserNotificationCenter.current()
        // Button titles follow the current language, so the category is
        // re-registered each time (setNotificationCategories replaces the whole set).
        categories[card.categoryID] = UNNotificationCategory(
            identifier: card.categoryID,
            actions: card.actions.map { UNNotificationAction(identifier: $0.rawValue, title: $0.title, options: [.foreground]) },
            intentIdentifiers: [], options: [])
        center.setNotificationCategories(Set(categories.values))
        let c = UNMutableNotificationContent()
        c.title = card.title
        c.body = card.body
        c.categoryIdentifier = card.categoryID
        c.userInfo = ["volume": card.volumeID]
        center.add(UNNotificationRequest(identifier: "card-" + card.volumeID, content: c, trigger: nil))
    }

    /// Show banners even while the app is in the foreground.
    public func userNotificationCenter(_ center: UNUserNotificationCenter, willPresent notification: UNNotification,
                                       withCompletionHandler completionHandler: @escaping (UNNotificationPresentationOptions) -> Void) {
        completionHandler([.banner])
    }

    /// Routes a click: a button identifier maps to an `InsertionAction`;
    /// clicking the notification body itself maps to `.show`.
    public func userNotificationCenter(_ center: UNUserNotificationCenter, didReceive response: UNNotificationResponse,
                                       withCompletionHandler completionHandler: @escaping () -> Void) {
        let info = response.notification.request.content.userInfo
        if let volume = info["volume"] as? String {
            let action = InsertionAction(rawValue: response.actionIdentifier) ?? .show
            DispatchQueue.main.async { self.onCardAction(volume, action) }
        } else {
            DispatchQueue.main.async { self.onOpen() }
        }
        completionHandler()
    }
}
