// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolSystem.swift
//
// Part of NTFSToolApp. The boundary between the app and the operating system
// for everything disk-related.
//
// How NTFSTool works: the app is sandboxed and never mounts anything itself.
// Once the user has enabled the NTFSTool file system extension (an FSKit
// module) in System Settings, macOS's disk arbitration daemon mounts NTFS
// drives read-write through that module as soon as they are plugged in. The
// app only lists the volumes, explains their state, asks DiskArbitration to
// unmount / remount / eject, and guides the user to enable the extension.
//
// The model talks to the system only through the two protocols defined here
// (`NTFSToolDiskSystem` and `NTFSToolDriverStatusProviding`). Each has a live
// implementation (DiskArbitration / FSKit) and an in-memory demo
// implementation, which powers both the unit tests and demo mode
// (`NTFSTOOL_DEMO=1`), so the UI can be developed without real disks or an
// Apple Developer account.

import DiskArbitration
import Foundation
import NTFSToolCore

/// The state of one NTFS volume, as shown in the list.
public enum NTFSToolVolumeState: Equatable {
    /// Mounted read-write by our FSKit module.
    case writable
    /// Mounted by our module, but read-only: Windows left the volume in a
    /// hibernated / Fast Startup state (or the media is write-protected), and
    /// writing to it could lose data.
    case readOnlyHibernated
    /// Mounted by Apple's built-in NTFS driver, which is read-only. Happens when
    /// the extension is off, or the drive was mounted before it was turned on.
    case readOnlySystem
    /// Present but not mounted.
    case unmounted
}

/// An NTFS volume plus its classified state.
public struct NTFSToolVolume: Identifiable, Equatable {
    public var volume: DiskVolume
    public var state: NTFSToolVolumeState
    public var id: String { volume.id }
    public init(volume: DiskVolume, state: NTFSToolVolumeState) {
        self.volume = volume
        self.state = state
    }
}

/// Outcome of a DiskArbitration request.
public enum NTFSToolDiskResult: Equatable {
    case ok
    /// DiskArbitration refused (`kDAReturnNotPrivileged`). A sandboxed app may
    /// not mount a physical drive itself; only the system does that when the
    /// drive is connected. The fix is to eject and reconnect the drive.
    case notPrivileged
    /// The volume is in use (open files) and cannot be unmounted.
    case busy
    /// Any other failure, with DiskArbitration's message or status code.
    case failed(String)
}

/// All disk operations the model needs. Implemented by `LiveDiskSystem`
/// (DiskArbitration) and `DemoDiskSystem` (in memory).
public protocol NTFSToolDiskSystem: AnyObject {
    /// The NTFS volumes currently known, already classified.
    func volumes() -> [NTFSToolVolume]
    /// Unmounts one volume (partition).
    func unmount(_ id: String) -> NTFSToolDiskResult
    /// Asks the system to mount one volume, letting it pick the file system.
    func mount(_ id: String) -> NTFSToolDiskResult
    /// Unmounts every volume of a whole disk and ejects it.
    func eject(_ wholeDisk: String) -> NTFSToolDiskResult
}

/// Whether our file system extension is enabled in System Settings.
public enum NTFSToolDriverStatus: Equatable {
    /// Enabled: NTFS drives will be mounted read-write.
    case ready
    /// Installed but switched off; the user has to enable it.
    case needsEnable
    /// Cannot be determined. FSKit only lets apps query extension state on
    /// macOS 26 and later; on older systems the UI falls back to hints.
    case unknown
}

/// Reads the extension status and opens the place where the user enables it.
public protocol NTFSToolDriverStatusProviding: AnyObject {
    func status() -> NTFSToolDriverStatus
    /// Opens the File System Extensions settings. Returns true when the system
    /// could open the extension list directly; false means the caller should
    /// open the general Login Items & Extensions pane instead.
    func openSettings() -> Bool
}

/// Pure functions that turn raw volume facts into a `NTFSToolVolumeState`.
public enum NTFSToolStateClassifier {
    /// Classifies a volume.
    /// - Parameter fskitType: the mount's file system type if it is one of our
    ///   module's short names (see `NTFSToolDriverNames`), otherwise nil.
    ///
    /// Rules: not mounted -> `.unmounted`; mounted by our module -> writable or,
    /// if the module mounted it read-only, `.readOnlyHibernated` (the module
    /// only does that when Windows did not shut down cleanly); mounted by
    /// anything else (Apple's `ntfs`) -> `.readOnlySystem`.
    public static func state(of v: DiskVolume, fskitType: String?) -> NTFSToolVolumeState {
        guard v.isMounted else { return .unmounted }
        if fskitType != nil { return v.isWritable ? .writable : .readOnlyHibernated }
        return .readOnlySystem
    }

    /// Only non-system NTFS volumes are listed.
    public static func visible(_ v: DiskVolume) -> Bool { v.isNTFS && !v.isSystem }
}
