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

// MARK: - Live implementation (DiskArbitration)

/// Real disk operations through the DiskArbitration framework.
///
/// DiskArbitration is asynchronous: every request takes a callback that runs on
/// the session's dispatch queue. The methods here wait for that callback with a
/// semaphore (and a 30-second timeout), so they must be called from a
/// background thread, which `FlowExecutor.live` guarantees.
public final class LiveDiskSystem: NTFSToolDiskSystem {
    /// Keeps the current disk list up to date and reports changes.
    public let inventory: DAInventory
    let session: DASession?
    let queue = DispatchQueue(label: "ntfstool.da")

    public init(inventory: DAInventory = DAInventory()) {
        self.inventory = inventory
        session = DASessionCreate(kCFAllocatorDefault)
        if let s = session { DASessionSetDispatchQueue(s, queue) }
    }

    public func volumes() -> [NTFSToolVolume] { inventory.volumes() }

    /// Carries the result from the C callback back to the waiting caller.
    final class Box {
        var status: DAReturn = 0
        var message: String?
        let sem = DispatchSemaphore(value: 0)
    }

    /// Shared completion callback for mount, unmount and eject. The context
    /// pointer is a retained `Box`; taking the retained value balances the
    /// `passRetained` done when the request was made.
    static let callback: DADiskMountCallback = { _, dissenter, ctx in
        let box = Unmanaged<Box>.fromOpaque(ctx!).takeRetainedValue()
        if let d = dissenter {   // a "dissenter" means the request was refused
            box.status = DADissenterGetStatus(d)
            box.message = DADissenterGetStatusString(d) as String?
        }
        box.sem.signal()
    }

    func disk(_ bsd: String) -> DADisk? {
        guard let s = session else { return nil }
        return DADiskCreateFromBSDName(kCFAllocatorDefault, s, bsd)
    }

    /// Maps a finished (or timed-out) request to a `NTFSToolDiskResult`.
    static func result(_ box: Box, timedOut: Bool) -> NTFSToolDiskResult {
        if timedOut { return .failed("timeout") }
        switch box.status {
        case 0: return .ok
        case DAReturn(kDAReturnNotPrivileged): return .notPrivileged
        case DAReturn(kDAReturnBusy): return .busy
        default: return .failed(box.message ?? String(format: "0x%08x", box.status))
        }
    }

    func wait(_ box: Box) -> NTFSToolDiskResult {
        Self.result(box, timedOut: box.sem.wait(timeout: .now() + 30) == .timedOut)
    }

    public func unmount(_ id: String) -> NTFSToolDiskResult {
        guard let d = disk(id) else { return .failed("no_disk") }
        let box = Box()
        DADiskUnmount(d, DADiskUnmountOptions(kDADiskUnmountOptionDefault), Self.callback, Unmanaged.passRetained(box).toOpaque())
        return wait(box)
    }

    public func mount(_ id: String) -> NTFSToolDiskResult {
        guard let d = disk(id) else { return .failed("no_disk") }
        let box = Box()
        // No file system is specified: DiskArbitration probes the volume and,
        // with the extension enabled, picks our FSKit module for NTFS.
        DADiskMount(d, nil, DADiskMountOptions(kDADiskMountOptionDefault), Self.callback, Unmanaged.passRetained(box).toOpaque())
        return wait(box)
    }

    public func eject(_ wholeDisk: String) -> NTFSToolDiskResult {
        guard let d = disk(wholeDisk) else { return .failed("no_disk") }
        // Step 1: unmount every volume on the disk (kDADiskUnmountOptionWhole).
        let box = Box()
        DADiskUnmount(d, DADiskUnmountOptions(kDADiskUnmountOptionWhole), Self.callback, Unmanaged.passRetained(box).toOpaque())
        let r = wait(box)
        guard r == .ok else { return r }
        // Step 2: eject the media so it is safe to unplug.
        let box2 = Box()
        DADiskEject(d, DADiskEjectOptions(kDADiskEjectOptionDefault), Self.callback, Unmanaged.passRetained(box2).toOpaque())
        return wait(box2)
    }
}
