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

// MARK: - Detail

/// The detail column: the selected drive, the empty state, or a spinner
/// before the first refresh.
struct NTFSToolDetailView: View {
    @ObservedObject var model: NTFSToolModel

    var body: some View {
        Group {
            if let v = model.selected {
                FitOrScroll { NTFSToolDetailContent(model: model, v: v) }
                    .overlay(BusyOverlay(volume: v.volume, busy: model.busy[v.id]))
                    .animation(.easeInOut(duration: 0.18), value: model.busy[v.id])
            } else if model.loaded {
                FitOrScroll { NTFSToolEmptyContent(model: model) }
            } else {
                ProgressView().frame(maxWidth: .infinity, maxHeight: .infinity)
            }
        }
        .frame(minWidth: 520, minHeight: 300)
        .appLayoutDirection()
    }
}

/// Everything shown for one drive. The window height is computed from this
/// view (see `NTFSToolWindowController.fitToContent`).
struct NTFSToolDetailContent: View {
    @ObservedObject var model: NTFSToolModel
    let v: NTFSToolVolume

    var body: some View {
        VStack(alignment: .leading, spacing: 20) {
            if model.showsGuide || model.suggestsGuide { NTFSToolGuideCard(model: model) }
            NTFSToolDetailHeader(model: model, v: v)
            Divider()
            CapacityView(volume: v.volume)
            InfoGrid(volume: v.volume)
            switch v.state {
            case .readOnlySystem where !model.showsGuide:
                Banner(symbol: "lock.fill", tint: .orange, text: L("banner_ro_system"), button: L("make_writable")) { model.makeWritable(v) }
            case .readOnlyHibernated:
                Banner(symbol: "moon.zzz.fill", tint: .orange, text: L("banner_ro_hibernated"), button: L("learn_more")) { model.makeWritable(v) }
            case .unmounted:
                Banner(symbol: "externaldrive.badge.plus", tint: .accentColor, text: L("banner_unmounted"), button: L("mount")) { model.mount(v) }
            default:
                EmptyView()
            }
            if model.showsProLink(for: v) { NTFSToolProLink(model: model) }
            HStack(spacing: 10) {
                if v.volume.isMounted {
                    Button { model.open(v) } label: { Label(L("show_in_finder"), systemImage: "folder") }
                }
                Button { model.eject(v) } label: { Label(L("eject"), systemImage: "eject") }
                Spacer()
            }
            .disabled(model.busy[v.id] != nil)
        }
        .padding(.horizontal, 28)
        .padding(.vertical, 24)
        .frame(maxWidth: 980, alignment: .leading)
        .textSelection(.enabled)   // names, sizes and paths can be selected and copied
    }
}

/// A single quiet link line pointing to NTFSTool Pro. Shown only for problems
/// this free app cannot solve (see `NTFSToolModel.showsProLink`); it just
/// opens the website in the browser.
struct NTFSToolProLink: View {
    @ObservedObject var model: NTFSToolModel
    var body: some View {
        Button(L("pro_link")) { model.openProInfo() }
            .buttonStyle(.link)
            .font(.callout)
            .accessibilityIdentifier("pro-link")
    }
}

/// Icon, name and connection on the left; size and state badge on the right.
struct NTFSToolDetailHeader: View {
    @ObservedObject var model: NTFSToolModel
    let v: NTFSToolVolume

    /// e.g. "USB External Volume · SanDisk Extreme · NTFS".
    var subtitle: String {
        let d = v.volume
        let bus = d.busProtocol == "Disk Image" ? L("disk_image") : d.busProtocol
        let source = [bus, L(d.isInternal ? "internal_volume" : "external_volume")].filter { !$0.isEmpty }.joined(separator: " ")
        return [source, d.mediaName.isEmpty || d.mediaName == "Disk Image" ? "" : d.mediaName, "NTFS"].filter { !$0.isEmpty }.joined(separator: " · ")
    }

    var body: some View {
        let tint = model.busy[v.id] != nil ? Color.blue : NTFSToolText.color(v.state)
        HStack(alignment: .center, spacing: 16) {
            VolumeIcon(volume: v.volume).frame(width: 64, height: 64)
            VStack(alignment: .leading, spacing: 4) {
                Text(v.volume.displayName).font(.system(size: 22, weight: .semibold)).lineLimit(1).truncationMode(.middle)
                Text(subtitle).font(.callout).foregroundColor(.secondary).lineLimit(2)
            }
            Spacer(minLength: 12)
            VStack(alignment: .trailing, spacing: 6) {
                Text(ByteFormat.string(v.volume.totalSize)).font(.system(size: 22, weight: .regular)).foregroundColor(.primary)
                HStack(spacing: 5) {
                    Circle().fill(tint).frame(width: 7, height: 7)
                    Text(NTFSToolText.status(v, busy: model.busy[v.id])).font(.caption.weight(.medium))
                }
                .padding(.horizontal, 9).padding(.vertical, 3)
                .background(Capsule().fill(tint.opacity(0.14)))
                .accessibilityIdentifier("status-badge")
            }
        }
    }
}

/// No drives connected: the guide card (if needed) and a "plug in a drive" hint.
struct NTFSToolEmptyContent: View {
    @ObservedObject var model: NTFSToolModel
    var body: some View {
        VStack(alignment: .leading, spacing: 20) {
            if model.showsGuide || model.suggestsGuide { NTFSToolGuideCard(model: model) }
            NTFSToolEmptyState()
        }
        .padding(.horizontal, 28)
        .padding(.vertical, 24)
        .frame(maxWidth: 980, alignment: .leading)
    }
}

/// The "turn on the file system extension" card.
///
/// Full version (extension known to be off, or guide running): explanation,
/// three steps, the button, and a spinner while waiting. Short version (status
/// unknown on older macOS but a drive is read-only): a one-line hint and the button.
struct NTFSToolGuideCard: View {
    @ObservedObject var model: NTFSToolModel

    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            HStack(spacing: 10) {
                Image(systemName: "puzzlepiece.extension.fill").font(.title2).foregroundColor(.accentColor)
                Text(model.suggestsGuide && !model.showsGuide ? L("guide_unknown") : L("guide_title"))
                    .font(.headline).fixedSize(horizontal: false, vertical: true)
            }
            if model.showsGuide {
                Text(L("guide_body")).font(.callout).foregroundColor(.secondary).fixedSize(horizontal: false, vertical: true)
                VStack(alignment: .leading, spacing: 6) {
                    step(1, L("guide_step1"))
                    // If settings could not open the list directly, explain the full path.
                    step(2, model.guideDirect || !model.guideWaiting ? L("guide_step2") : L("guide_step2_full"))
                    step(3, L("guide_step3"))
                }
            }
            HStack(spacing: 10) {
                Button(L("guide_open")) { model.startGuide() }.controlSize(.large).keyboardShortcut(.defaultAction)
                if model.guideWaiting {
                    ProgressView().controlSize(.small)
                    Text(L("guide_waiting")).font(.callout).foregroundColor(.secondary)
                }
            }
        }
        .padding(16)
        .frame(maxWidth: .infinity, alignment: .leading)
        .background(RoundedRectangle(cornerRadius: 12).fill(Color.accentColor.opacity(0.08)))
        .accessibilityIdentifier("guide-card")
    }

    func step(_ n: Int, _ text: String) -> some View {
        HStack(alignment: .firstTextBaseline, spacing: 8) {
            Text("\(n)").font(.caption.weight(.bold)).foregroundColor(.white)
                .frame(width: 18, height: 18).background(Circle().fill(Color.accentColor))
            Text(text).font(.callout).fixedSize(horizontal: false, vertical: true)
        }
    }
}

/// Big "Plug in an NTFS drive" placeholder.
struct NTFSToolEmptyState: View {
    var body: some View {
        VStack(spacing: 14) {
            Image(systemName: "externaldrive.badge.plus").font(.system(size: 56, weight: .light)).foregroundColor(.secondary)
            Text(L("empty_title")).font(.title3.weight(.semibold))
            Text(L("empty_body")).font(.callout).foregroundColor(.secondary).multilineTextAlignment(.center).frame(maxWidth: 400)
        }
        .padding(.vertical, 40)
        .frame(maxWidth: .infinity)
    }
}

/// Sidebar and detail side by side in pure SwiftUI. Used by snapshot tests;
/// the real window is `NTFSToolWindowController`.
@available(macOS 13.0, *)
public struct NTFSToolRootView: View {
    @ObservedObject var model: NTFSToolModel
    public init(model: NTFSToolModel) { self.model = model }
    public var body: some View {
        HStack(spacing: 0) {
            NTFSToolSidebar(model: model).frame(width: 240)
            Divider()
            NTFSToolDetailView(model: model)
        }
    }
}
