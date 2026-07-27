// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolComponents.swift
//
// Part of NTFSToolApp. Small reusable SwiftUI views used by the main window:
// drive icons, the progress overlay, the capacity bar, the disk info table and
// the hint banner. They only read `DiskVolume` values and never touch the model.

import AppKit
import NTFSToolCore
import SwiftUI

/// Text for an operation in progress ("Mounting...", "Ejecting...").
enum BusyText {
    static func of(_ busy: BusyKind) -> String {
        switch busy {
        case .mounting: return L("busy_mounting")
        case .unmounting: return L("busy_ejecting")
        }
    }
}

/// A plain SF Symbol drive icon (used in the sidebar, where the system tints it).
struct DriveIcon: View {
    let volume: DiskVolume
    var size: CGFloat
    var body: some View {
        Image(systemName: volume.isSystem ? "internaldrive.fill" : "externaldrive.fill")
            .font(.system(size: size * 0.9))
            .frame(width: size + 4, height: size)
    }
}

/// Shown over the detail pane while an operation runs: dims the content and
/// shows a card with a spinner, what is happening and "don't unplug". It also
/// swallows clicks so buttons cannot be pressed twice.
struct BusyOverlay: View {
    let volume: DiskVolume
    let busy: BusyKind?

    var body: some View {
        if let b = busy {
            ZStack {
                Color(nsColor: .windowBackgroundColor).opacity(0.55)
                VStack(spacing: 12) {
                    ProgressView().controlSize(.large)
                    Text(BusyText.of(b)).font(.headline)
                    Text(volume.displayName).font(.subheadline).foregroundColor(.secondary).lineLimit(1).truncationMode(.middle)
                    Text(L("busy_hint")).font(.caption).foregroundColor(.secondary).multilineTextAlignment(.center)
                }
                .padding(.horizontal, 28).padding(.vertical, 22)
                .frame(minWidth: 240, maxWidth: 340)
                .background(RoundedRectangle(cornerRadius: 14, style: .continuous).fill(.regularMaterial))
                .overlay(RoundedRectangle(cornerRadius: 14, style: .continuous).stroke(Color.primary.opacity(0.08)))
                .shadow(color: .black.opacity(0.18), radius: 18, y: 6)
            }
            .contentShape(Rectangle())
            .transition(.opacity)
            .accessibilityElement(children: .combine)
            .accessibilityIdentifier("busy-overlay")
        }
    }
}

/// A scroll view without scroll bars. The window is sized to fit its content
/// (see `NTFSToolWindowController.fitToContent`), so scrolling only happens
/// when the screen is too small.
struct FitOrScroll<Content: View>: View {
    @ViewBuilder let content: () -> Content

    var body: some View {
        if #available(macOS 13, *) {
            ScrollView { content() }.scrollIndicators(.never)
        } else {
            ScrollView(showsIndicators: false) { content() }
        }
    }
}

/// The large icon in the detail header: Finder's real volume icon when the
/// volume is mounted, otherwise an SF Symbol.
struct VolumeIcon: View {
    let volume: DiskVolume

    static func icon(_ v: DiskVolume) -> NSImage? {
        guard v.isMounted, !v.mountPoint.isEmpty, FileManager.default.fileExists(atPath: v.mountPoint) else { return nil }
        return NSWorkspace.shared.icon(forFile: v.mountPoint)
    }

    var body: some View {
        if let img = Self.icon(volume) {
            Image(nsImage: img).resizable().interpolation(.high).aspectRatio(contentMode: .fit)
        } else {
            Image(systemName: volume.isSystem ? "internaldrive.fill" : "externaldrive.fill")
                .font(.system(size: 46, weight: .regular))
                .foregroundColor(volume.isSystem ? .secondary : .accentColor)
        }
    }
}

/// Used / available bar with a legend below it.
struct CapacityView: View {
    let volume: DiskVolume

    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            GeometryReader { geo in
                ZStack(alignment: .leading) {
                    RoundedRectangle(cornerRadius: 5).fill(Color.primary.opacity(0.08))
                    if volume.isMounted {
                        RoundedRectangle(cornerRadius: 5).fill(Color.accentColor)
                            .frame(width: max(4, geo.size.width * CGFloat(volume.usedFraction)))
                    }
                }
            }
            .frame(height: 22)
            .overlay(RoundedRectangle(cornerRadius: 5).stroke(Color.primary.opacity(0.08), lineWidth: 0.5))
            HStack(alignment: .top, spacing: 0) {
                if volume.isMounted {
                    Legend(color: .accentColor, title: L("used"), value: ByteFormat.string(volume.usedSize)).frame(maxWidth: .infinity, alignment: .leading)
                    Legend(color: Color.primary.opacity(0.06), title: L("available"), value: ByteFormat.string(volume.freeSpace)).frame(maxWidth: .infinity, alignment: .leading)
                } else {
                    // Free space is unknown until the volume is mounted.
                    Legend(color: Color.primary.opacity(0.15), title: L("state_not_mounted"), value: ByteFormat.string(volume.totalSize))
                }
            }
        }
    }
}

/// One legend entry: color swatch, title and value.
struct Legend: View {
    let color: Color
    let title: String
    let value: String
    var body: some View {
        HStack(alignment: .firstTextBaseline, spacing: 7) {
            RoundedRectangle(cornerRadius: 3).fill(color).frame(width: 11, height: 11)
                .overlay(RoundedRectangle(cornerRadius: 3).stroke(Color.primary.opacity(0.12), lineWidth: 0.5))
                .alignmentGuide(.firstTextBaseline) { $0[.bottom] - 1 }
            VStack(alignment: .leading, spacing: 3) {
                Text(title).font(.callout.weight(.semibold))
                Text(value).font(.callout.monospacedDigit()).foregroundColor(.secondary)
            }
        }
    }
}

/// The two-column disk info table (mount point, type, capacity, ...).
struct InfoGrid: View {
    let volume: DiskVolume

    /// Each row holds two label/value pairs.
    var rows: [(String, String, String, String)] {
        let yes = L("yes"), no = L("no")
        let mounted = volume.isMounted
        return [
            (L("mount_point"), mounted ? volume.mountPoint : L("state_not_mounted"), L("type"), "NTFS"),
            (L("capacity"), ByteFormat.string(volume.totalSize), L("info_writable"), mounted ? (volume.isWritable ? yes : no) : "—"),
            (L("available"), mounted ? ByteFormat.string(volume.freeSpace) : "—", L("connection"), volume.busProtocol.isEmpty ? "—" : volume.busProtocol),
            (L("used"), mounted ? ByteFormat.string(volume.usedSize) : "—", L("device"), volume.id),
        ]
    }

    var body: some View {
        VStack(spacing: 0) {
            ForEach(Array(rows.enumerated()), id: \.offset) { i, r in
                HStack(spacing: 0) {
                    Cell(label: r.0, value: r.1)
                    Divider()
                    Cell(label: r.2, value: r.3)
                }
                .frame(height: 30)
                if i < rows.count - 1 { Divider() }
            }
        }
        .background(RoundedRectangle(cornerRadius: 10).fill(Color(nsColor: .controlBackgroundColor)))
        .overlay(RoundedRectangle(cornerRadius: 10).stroke(Color(nsColor: .separatorColor), lineWidth: 0.5))
    }

    struct Cell: View {
        let label: String
        let value: String
        var body: some View {
            HStack {
                Text(label + ":").font(.callout).foregroundColor(.primary).lineLimit(1)
                Spacer(minLength: 8)
                Text(value).font(.callout).lineLimit(1).truncationMode(.middle).textSelection(.enabled)
            }
            .padding(.horizontal, 12)
            .frame(maxWidth: .infinity)
        }
    }
}

/// A tinted hint row with an icon, text and an optional button.
struct Banner: View {
    let symbol: String
    let tint: Color
    let text: String
    let button: String?
    let action: () -> Void
    var body: some View {
        HStack(spacing: 10) {
            Image(systemName: symbol).foregroundColor(tint)
            Text(text).font(.callout).fixedSize(horizontal: false, vertical: true)
            Spacer(minLength: 8)
            if let b = button { Button(b, action: action).controlSize(.regular) }
        }
        .padding(12)
        .background(RoundedRectangle(cornerRadius: 10).fill(tint.opacity(0.08)))
    }
}

extension View {
    /// Mirrors the layout for right-to-left languages (Arabic, Persian), based
    /// on the app's own language choice rather than the system's.
    func appLayoutDirection() -> some View {
        environment(\.layoutDirection, L10n.shared.language.isRTL ? .rightToLeft : .leftToRight)
    }
}
