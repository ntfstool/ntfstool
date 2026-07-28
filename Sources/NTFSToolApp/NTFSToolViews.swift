// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolViews.swift
//
// Part of NTFSToolApp. The SwiftUI content of the main window.
//
// Layout (modelled on Disk Utility): a system sidebar listing the NTFS drives
// and a detail pane for the selected drive with a header (icon, name, size,
// state), a capacity bar, the info table, a hint banner for read-only or
// unmounted drives, and action buttons. The AppKit window around these views
// (split view and toolbar) is in NTFSToolWindow.swift.

import AppKit
import NTFSToolCore
import SwiftUI

/// Texts and colors for volume states.
enum NTFSToolText {
    static func state(_ s: NTFSToolVolumeState) -> String {
        switch s {
        case .writable: return L("state_read_write")
        case .readOnlyHibernated: return L("state_ro_hibernated")
        case .readOnlySystem: return L("state_read_only")
        case .unmounted: return L("state_not_mounted")
        }
    }

    /// The busy text while an operation runs, otherwise the state.
    static func status(_ v: NTFSToolVolume, busy: BusyKind?) -> String {
        guard let b = busy else { return state(v.state) }
        return BusyText.of(b)
    }

    static func color(_ s: NTFSToolVolumeState) -> Color {
        switch s {
        case .writable: return .green
        case .readOnlyHibernated, .readOnlySystem: return .orange
        case .unmounted: return .gray
        }
    }
}

// MARK: - Sidebar

/// The drive list.
struct NTFSToolSidebar: View {
    @ObservedObject var model: NTFSToolModel

    var body: some View {
        // The selection is written back asynchronously to avoid publishing
        // changes while SwiftUI is updating the list.
        List(selection: Binding(get: { model.selected?.id }, set: { id in DispatchQueue.main.async { model.selectedID = id } })) {
            Section(header: Text(L("sidebar_title"))) {
                ForEach(model.volumes) { v in NTFSToolSidebarRow(model: model, v: v).tag(Optional(v.id)) }
                if model.loaded && model.volumes.isEmpty {
                    Text(L("sidebar_none")).font(.callout).foregroundColor(.secondary).padding(.vertical, 2)
                }
            }
        }
        .listStyle(.sidebar)
        .frame(minWidth: 200)
        .appLayoutDirection()
        .accessibilityIdentifier("sidebar")
    }
}

/// One drive in the sidebar: icon, name, state, eject button.
/// Double-click opens it in Finder; right-click shows the actions.
struct NTFSToolSidebarRow: View {
    @ObservedObject var model: NTFSToolModel
    let v: NTFSToolVolume

    var body: some View {
        HStack(spacing: 8) {
            // Not tinted: the sidebar colors it and turns it white when selected.
            DriveIcon(volume: v.volume, size: 18)
            VStack(alignment: .leading, spacing: 1) {
                Text(v.volume.displayName).lineLimit(1).truncationMode(.middle)
                Text(NTFSToolText.status(v, busy: model.busy[v.id])).font(.caption2).foregroundColor(.secondary).lineLimit(1)
            }
            Spacer(minLength: 4)
            if model.busy[v.id] != nil {
                ProgressView().controlSize(.small).scaleEffect(0.7).frame(width: 16, height: 16)
            } else if v.volume.isMounted {
                Button { model.eject(v) } label: {
                    Image(systemName: "eject.fill").font(.system(size: 10, weight: .semibold)).foregroundColor(.secondary)
                }
                .buttonStyle(.borderless)
                .help(L("eject"))
            }
        }
        .padding(.vertical, 2)
        .contentShape(Rectangle())
        // simultaneousGesture keeps single-click selection working.
        .simultaneousGesture(TapGesture(count: 2).onEnded { model.open(v) })
        .contextMenu {
            if v.volume.isMounted { Button(L("show_in_finder")) { model.open(v) } }
            if v.state == .readOnlySystem || v.state == .unmounted { Button(L("make_writable")) { model.makeWritable(v) } }
            Button(L("eject")) { model.eject(v) }
        }
    }
}
