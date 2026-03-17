// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolDisks.swift
//
// Part of NTFSToolCore: pure logic with no AppKit dependency.
//
// `DiskVolume` is the plain value type the whole app uses to describe one disk
// partition (a "volume"). The app layer fills it from DiskArbitration
// (see `NTFSToolDiskInfo` in NTFSToolApp) and the views only ever read it, so
// tests and demo mode can create volumes by hand without any real disk attached.

import Foundation

/// A snapshot of one disk volume as seen by macOS.
///
/// All fields are plain values; nothing here talks to the system. A volume is
/// "mounted" when it has a mount point, and "writable" only when it is mounted
/// read-write.
public struct DiskVolume: Equatable, Identifiable {
    /// BSD name of the partition, e.g. `disk4s1`. Used as the stable identifier.
    public var id: String
    /// Device node, e.g. `/dev/disk4s1`.
    public var deviceNode: String
    /// BSD name of the whole physical disk that contains this partition, e.g. `disk4`.
    /// Ejecting always happens on the whole disk.
    public var wholeDisk: String
    /// Volume label as shown in Finder (may be empty).
    public var name: String
    /// Human-readable file system name, e.g. "NTFS".
    public var fileSystemName: String
    /// File system type as reported by `statfs` / DiskArbitration, e.g. `ntfs`
    /// (Apple's read-only driver) or the short name of our FSKit module.
    public var fileSystemType: String
    /// Partition content type, e.g. `Windows_NTFS`.
    public var content: String
    /// Where the volume is mounted, or an empty string when not mounted.
    public var mountPoint: String
    /// Size of the volume in bytes.
    public var totalSize: Int64
    /// Free bytes available to the user (only meaningful while mounted).
    public var freeSpace: Int64
    /// True only when the volume is mounted read-write.
    public var isWritable: Bool
    /// True for the volume mounted at `/` (never shown in the list).
    public var isSystem: Bool
    /// True when the volume is formatted as NTFS.
    public var isNTFS: Bool
    /// True for disks built into the Mac.
    public var isInternal: Bool
    /// Connection type, e.g. "USB", "Thunderbolt" or "Disk Image".
    public var busProtocol: String
    /// Model name of the device, e.g. "SanDisk Extreme".
    public var mediaName: String
    /// File system UUID, if the volume has one.
    public var volumeUUID: String
    /// True for removable media / external devices.
    public var removable: Bool

    public init(id: String, deviceNode: String = "", wholeDisk: String = "", name: String = "", fileSystemName: String = "",
                fileSystemType: String = "", content: String = "", mountPoint: String = "", totalSize: Int64 = 0,
                freeSpace: Int64 = 0, isWritable: Bool = false, isSystem: Bool = false, isNTFS: Bool = false,
                isInternal: Bool = false, busProtocol: String = "", mediaName: String = "", volumeUUID: String = "",
                removable: Bool = false) {
        self.id = id
        self.deviceNode = deviceNode.isEmpty ? "/dev/" + id : deviceNode
        self.wholeDisk = wholeDisk.isEmpty ? DiskVolume.wholeDisk(of: id) : wholeDisk
        self.name = name
        self.fileSystemName = fileSystemName
        self.fileSystemType = fileSystemType
        self.content = content
        self.mountPoint = mountPoint
        self.totalSize = totalSize
        self.freeSpace = freeSpace
        self.isWritable = isWritable
        self.isSystem = isSystem
        self.isNTFS = isNTFS
        self.isInternal = isInternal
        self.busProtocol = busProtocol
        self.mediaName = mediaName
        self.volumeUUID = volumeUUID
        self.removable = removable
    }

    /// True when the volume currently has a mount point.
    public var isMounted: Bool { !mountPoint.isEmpty }
    /// Bytes in use (never negative).
    public var usedSize: Int64 { max(0, totalSize - freeSpace) }
    /// Used space as a fraction between 0 and 1, for the capacity bar.
    public var usedFraction: Double { totalSize > 0 ? min(1, Double(usedSize) / Double(totalSize)) : 0 }
    /// The label to show in the UI: the volume name, or the BSD name when unnamed.
    public var displayName: String { name.isEmpty ? id : name }

    /// Derives the whole-disk BSD name from a partition name: `disk4s1` -> `disk4`.
    /// Names that do not look like `diskN...` are returned unchanged.
    public static func wholeDisk(of id: String) -> String {
        guard let r = id.range(of: #"^disk\d+"#, options: .regularExpression) else { return id }
        return String(id[r])
    }
}
