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

/// Context object handed to the C callbacks (passed as an unretained `ctx` pointer).
final class DeviceBox {
    let device: BlockDevice
    init(_ d: BlockDevice) { device = d }
}

/// Build the C callback table for a device. The C side copies the struct; `box`
/// must outlive the mount (NTFSVolume keeps it in a stored property).
private func makeIO(_ box: DeviceBox) -> ntfstool_io {
    var io = ntfstool_io()
    io.ctx = Unmanaged.passUnretained(box).toOpaque()
    io.size = box.device.size
    io.block_size = box.device.blockSize
    io.read = { ctx, buf, off, len in
        Unmanaged<DeviceBox>.fromOpaque(ctx!).takeUnretainedValue().device.read(into: buf!, offset: off, length: len)
    }
    io.write = { ctx, buf, off, len in
        Unmanaged<DeviceBox>.fromOpaque(ctx!).takeUnretainedValue().device.write(from: buf!, offset: off, length: len)
    }
    io.sync = { ctx in Unmanaged<DeviceBox>.fromOpaque(ctx!).takeUnretainedValue().device.sync() }
    return io
}

/// A mounted NTFS volume, addressed by inode number (MFT record number).
///
/// Thread safety: libntfstool-driver is not thread safe and FSKit calls the module from
/// many threads, so every operation takes one lock for its whole duration.
/// That also preserves the bridge's "only one open ntfs_inode per MFT record"
/// rule: an operation's inodes are always closed before the next one starts.
public final class NTFSVolume {
    /// MFT record number of the NTFS root directory (5).
    public static let rootIno: UInt64 = UInt64(NTB_ROOT)

    let box: DeviceBox
    var vol: OpaquePointer?
    let lock = NSLock()
    /// True if mounted read-only (requested, or forced by libntfstool-driver).
    public let readOnly: Bool
    /// NTFS volume serial number (stable per formatted volume).
    public let serial: UInt64

    /// Returns the volume serial number if `device` contains NTFS, else nil.
    public static func probe(_ device: BlockDevice) -> UInt64? {
        let box = DeviceBox(device)
        var io = makeIO(box)
        var serial: UInt64 = 0
        return withExtendedLifetime(box) { ntfstool_probe(&io, &serial) == 0 ? serial : nil }
    }

    /// Mount. Throws `NTFSError(EINVAL)` for non-NTFS, `EPERM` for a hibernated /
    /// Fast Startup volume opened writable.
    public init(device: BlockDevice, readOnly: Bool = false) throws {
        box = DeviceBox(device)
        var io = makeIO(box)
        var err: Int32 = 0
        guard let v = ntfstool_mount(&io, readOnly ? 1 : 0, &err) else { throw NTFSError(err == 0 ? EIO : err) }
        vol = v
        self.readOnly = ntfstool_is_readonly(v) != 0
        serial = ntfstool_serial(v)
    }

    deinit { try? unmount() }

    /// Reject file names Windows cannot open (same rule as the `windows_names` option).
    public var windowsNames: Bool {
        get { lock.lock(); defer { lock.unlock() }; return vol.map { ntfstool_windows_names($0) != 0 } ?? false }
        set { lock.lock(); defer { lock.unlock() }; if let v = vol { ntfstool_set_windows_names(v, newValue ? 1 : 0) } }
    }

    /// Run `body` with the volume lock held; throws ENXIO once unmounted.
    func locked<T>(_ body: (OpaquePointer) throws -> T) throws -> T {
        lock.lock()
        defer { lock.unlock() }
        guard let v = vol else { throw NTFSError(ENXIO) }
        return try body(v)
    }

    /// Convert a bridge return value (0 or errno) into a Swift error.
    func check(_ r: Int32) throws { if r != 0 { throw NTFSError(r) } }

    /// Write everything back and release the volume. Safe to call more than once.
    public func unmount() throws {
        lock.lock()
        defer { lock.unlock() }
        guard let v = vol else { return }
        vol = nil
        let r = ntfstool_umount(v)
        if r != 0 { throw NTFSError(r) }
    }

    public var isMounted: Bool { lock.lock(); defer { lock.unlock() }; return vol != nil }

    public func label() throws -> String {
        try locked { v in
            guard let c = ntfstool_label(v) else { return "" }
            defer { free(c) }
            return String(cString: c)
        }
    }

    public func setLabel(_ name: String) throws { try locked { v in try check(ntfstool_set_label(v, name)) } }

    public func statfs() throws -> NTFSStatFS {
        try locked { v in
            var s = ntfstool_statfs()
            try check(ntfstool_statfs_get(v, &s))
            return NTFSStatFS(clusterSize: s.cluster_size, totalClusters: s.total_clusters, freeClusters: s.free_clusters,
                              totalFiles: s.total_files, freeFiles: s.free_files)
        }
    }

    /// Find `name` in directory `dir` and return its inode number.
    public func lookup(_ name: String, in dir: UInt64) throws -> UInt64 {
        try locked { v in
            var ino: UInt64 = 0
            try check(ntfstool_lookup(v, dir, name, &ino))
            return ino
        }
    }

    public func attributes(_ ino: UInt64) throws -> NTFSAttributes {
        try locked { v in
            var a = ntfstool_attr()
            try check(ntfstool_getattr(v, ino, &a))
            return NTFSAttributes(ino: a.ino, type: NTFSItemType(rawValue: a.type) ?? .file, linkCount: a.nlink, size: a.size, allocSize: a.alloc,
                                  hidden: a.flags & UInt32(NTB_F_HIDDEN) != 0, readOnly: a.flags & UInt32(NTB_F_READONLY) != 0,
                                  system: a.flags & UInt32(NTB_F_SYSTEM) != 0, compressed: a.flags & UInt32(NTB_F_COMPRESSED) != 0,
                                  accessTime: a.atime, modifyTime: a.mtime, changeTime: a.ctime, birthTime: a.btime)
        }
    }

    /// Collects entries from the C readdir callback.
    final class ListBox { var items: [NTFSDirEntry] = [] }

    /// List a directory (without ".", "..", DOS short names and metadata files).
    public func list(_ dir: UInt64) throws -> [NTFSDirEntry] {
        try locked { v in
            let box = ListBox()
            let r = withExtendedLifetime(box) {
                ntfstool_readdir(v, dir, { ctx, name, ino, type in
                    let b = Unmanaged<ListBox>.fromOpaque(ctx!).takeUnretainedValue()
                    b.items.append(NTFSDirEntry(name: String(cString: name!), ino: ino, type: NTFSItemType(rawValue: type) ?? .file))
                    return 0
                }, Unmanaged.passUnretained(box).toOpaque())
            }
            try check(r)
            return box.items
        }
    }

    /// Read into `buffer`; returns the number of bytes read (fewer at end of file).
    public func read(_ ino: UInt64, offset: UInt64, into buffer: UnsafeMutableRawPointer, length: Int) throws -> Int {
        try locked { v in
            var done = 0
            try check(ntfstool_read(v, ino, offset, buffer, length, &done))
            return done
        }
    }

    /// Convenience read returning `Data`.
    public func read(_ ino: UInt64, offset: UInt64 = 0, length: Int) throws -> Data {
        var d = Data(count: length)
        let n = try d.withUnsafeMutableBytes { try read(ino, offset: offset, into: $0.baseAddress!, length: length) }
        return d.prefix(n)
    }

    /// Write from `buffer`; the file grows as needed. Returns bytes written.
    @discardableResult
    public func write(_ ino: UInt64, offset: UInt64, from buffer: UnsafeRawPointer, length: Int) throws -> Int {
        try locked { v in
            var done = 0
            try check(ntfstool_write(v, ino, offset, buffer, length, &done))
            return done
        }
    }

    @discardableResult
    public func write(_ ino: UInt64, offset: UInt64, data: Data) throws -> Int {
        if data.isEmpty { return 0 }
        return try data.withUnsafeBytes { try write(ino, offset: offset, from: $0.baseAddress!, length: data.count) }
    }

    public func truncate(_ ino: UInt64, size: UInt64) throws { try locked { v in try check(ntfstool_truncate(v, ino, size)) } }
    /// Write the item's metadata back and flush the device (called on last close).
    public func flush(_ ino: UInt64) throws { try locked { v in try check(ntfstool_flush(v, ino)) } }
    /// Flush the device.
    public func sync() throws { try locked { v in try check(ntfstool_sync(v)) } }

    /// Create a file or directory; returns the new inode number.
    public func create(_ name: String, in dir: UInt64, type: NTFSItemType) throws -> UInt64 {
        try locked { v in
            var ino: UInt64 = 0
            try check(ntfstool_create(v, dir, name, type.rawValue, &ino))
            return ino
        }
    }

    public func symlink(_ name: String, in dir: UInt64, target: String) throws -> UInt64 {
        try locked { v in
            var ino: UInt64 = 0
            try check(ntfstool_symlink(v, dir, name, target, &ino))
            return ino
        }
    }

    public func readlink(_ ino: UInt64) throws -> String {
        try locked { v in
            var t: UnsafeMutablePointer<CChar>?
            try check(ntfstool_readlink(v, ino, &t))
            defer { free(t) }
            return String(cString: t!)
        }
    }

    /// Remove a file or an empty directory.
    public func remove(_ name: String, in dir: UInt64) throws { try locked { v in try check(ntfstool_unlink(v, dir, name)) } }

    /// rename(2): replaces an existing file / empty directory at the destination.
    public func rename(_ name: String, in dir: UInt64, to newName: String, in newDir: UInt64) throws {
        try locked { v in try check(ntfstool_rename(v, dir, name, newDir, newName)) }
    }

    /// Set any subset of the four NTFS times (nil = unchanged).
    public func setTimes(_ ino: UInt64, access: timespec? = nil, modify: timespec? = nil, change: timespec? = nil, birth: timespec? = nil) throws {
        try locked { v in
            // Turn each optional timespec into an optional pointer for the C call.
            func ptr(_ t: timespec?, _ body: (UnsafePointer<timespec>?) -> Int32) -> Int32 {
                guard var t = t else { return body(nil) }
                return withUnsafePointer(to: &t) { body($0) }
            }
            let r = ptr(access) { a in ptr(modify) { m in ptr(change) { c in ptr(birth) { b in ntfstool_set_times(v, ino, a, m, c, b) } } } }
            try check(r)
        }
    }

    public func setHidden(_ ino: UInt64, _ hidden: Bool) throws { try locked { v in try check(ntfstool_set_hidden(v, ino, hidden ? 1 : 0)) } }

}
