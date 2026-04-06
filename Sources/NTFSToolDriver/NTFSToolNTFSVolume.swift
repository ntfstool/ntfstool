// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolNTFSVolume.swift - Swift wrapper around the ntfstool_bridge C API.
//
// Layering:
//
//   FSKit module (FSKitModule/NTFSToolFS)   translates FSKit callbacks into NTFSVolume calls
//   NTFSVolume (this file)                  Swift API, serialization, errno -> NTFSError
//   ntfstool_bridge.c                       inode-number based API over libntfstool-driver
//   libntfstool-driver                              NTFS on-disk format implementation
//   BlockDevice                             FSKit block resource, or an image file in tests
//
// This target deliberately does not import FSKit, so the whole NTFS engine can be
// built and unit-tested on any macOS version against real NTFS images.

import Foundation
import NTFSToolBridge

/// A POSIX error returned by the bridge (`code` is an errno value such as `ENOENT`).
public struct NTFSError: Error, Equatable, CustomStringConvertible {
    public let code: Int32
    public init(_ code: Int32) { self.code = code }
    public var description: String { "\(code) \(String(cString: strerror(code)))" }
}

/// A raw block device the NTFS volume lives on.
///
/// The bridge guarantees that every `read`/`write` it issues is aligned to
/// `blockSize` (offset and length), because FSKit block devices reject unaligned I/O.
public protocol BlockDevice: AnyObject {
    /// Usable size in bytes (a multiple of `blockSize`).
    var size: UInt64 { get }
    /// Required I/O alignment in bytes.
    var blockSize: UInt32 { get }
    /// Read exactly `length` bytes at `offset`. Returns 0 or an errno.
    func read(into buffer: UnsafeMutableRawPointer, offset: UInt64, length: UInt64) -> Int32
    /// Write exactly `length` bytes at `offset`. Returns 0 or an errno.
    func write(from buffer: UnsafeRawPointer, offset: UInt64, length: UInt64) -> Int32
    /// Make previous writes durable. Returns 0 or an errno.
    func sync() -> Int32
}

/// A `BlockDevice` backed by an image file or device node (used by tests and tools).
///
/// With `strictAlignment` (the default) unaligned requests fail with `EINVAL`,
/// mimicking an FSKit block device so tests prove the bridge's alignment layer works.
public final class FileBlockDevice: BlockDevice {
    public let size: UInt64
    public let blockSize: UInt32
    /// Reject unaligned I/O like a real block device.
    public var strictAlignment = true
    /// Number of unaligned requests seen (should stay 0).
    public private(set) var misaligned = 0
    let fd: Int32

    public init(path: String, blockSize: UInt32 = 4096, readOnly: Bool = false) throws {
        fd = open(path, readOnly ? O_RDONLY : O_RDWR)
        guard fd >= 0 else { throw NTFSError(errno) }
        var st = stat()
        fstat(fd, &st)
        // Ignore a trailing partial block: the device size must be a multiple of the block size.
        size = UInt64(st.st_size) / UInt64(blockSize) * UInt64(blockSize)
        self.blockSize = blockSize
    }

    deinit { close(fd) }

    func aligned(_ off: UInt64, _ len: UInt64) -> Bool {
        let b = UInt64(blockSize)
        if off % b == 0 && len % b == 0 { return true }
        misaligned += 1
        return !strictAlignment
    }

    public func read(into buffer: UnsafeMutableRawPointer, offset: UInt64, length: UInt64) -> Int32 {
        guard aligned(offset, length) else { return EINVAL }
        var done: UInt64 = 0
        while done < length {
            let n = pread(fd, buffer + Int(done), Int(length - done), off_t(offset + done))
            if n < 0 { return errno }
            if n == 0 { return EIO } // unexpected end of file
            done += UInt64(n)
        }
        return 0
    }

    public func write(from buffer: UnsafeRawPointer, offset: UInt64, length: UInt64) -> Int32 {
        guard aligned(offset, length) else { return EINVAL }
        var done: UInt64 = 0
        while done < length {
            let n = pwrite(fd, buffer + Int(done), Int(length - done), off_t(offset + done))
            if n < 0 { return errno }
            done += UInt64(n)
        }
        return 0
    }

    public func sync() -> Int32 { fsync(fd) == 0 ? 0 : errno }
}

/// Kind of a file system item (values match `NTB_FILE` / `NTB_DIR` / `NTB_SYMLINK`).
public enum NTFSItemType: UInt32 {
    case file = 1, directory = 2, symlink = 3
}

/// Attributes of one item, converted from `ntfstool_attr`.
public struct NTFSAttributes: Equatable {
    /// MFT record number.
    public var ino: UInt64
    public var type: NTFSItemType
    public var linkCount: UInt32
    /// Logical size; for a symlink, the target length.
    public var size: UInt64
    /// Bytes allocated on disk.
    public var allocSize: UInt64
    public var hidden: Bool
    public var readOnly: Bool
    public var system: Bool
    public var compressed: Bool
    public var accessTime: timespec
    public var modifyTime: timespec
    public var changeTime: timespec
    public var birthTime: timespec

    // timespec is not Equatable; compare the fields tests care about.
    public static func == (a: Self, b: Self) -> Bool {
        a.ino == b.ino && a.type == b.type && a.size == b.size && a.hidden == b.hidden && a.linkCount == b.linkCount
            && a.modifyTime.tv_sec == b.modifyTime.tv_sec && a.modifyTime.tv_nsec == b.modifyTime.tv_nsec
    }
}

/// One directory entry.
public struct NTFSDirEntry: Equatable {
    public var name: String
    public var ino: UInt64
    public var type: NTFSItemType
    public init(name: String, ino: UInt64, type: NTFSItemType) {
        self.name = name
        self.ino = ino
        self.type = type
    }
}

/// Volume capacity (statfs).
public struct NTFSStatFS: Equatable {
    public var clusterSize: UInt32
    public var totalClusters: UInt64
    public var freeClusters: UInt64
    public var totalFiles: UInt64
    public var freeFiles: UInt64
    public var totalBytes: UInt64 { UInt64(clusterSize) * totalClusters }
    public var freeBytes: UInt64 { UInt64(clusterSize) * freeClusters }
}
