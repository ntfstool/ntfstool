// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolFS.swift - the FSKit file system module (app extension) for NTFS.
//
// How the pieces fit together:
//
//   macOS (mount, Finder, DiskArbitration)
//        |  FSKit framework (user-space file systems, macOS 15.4+)
//   NTFSToolFileSystem   (this file) - FSUnaryFileSystem: probe / load / unload a disk, fsck
//   NTFSToolVolume       (this file) - FSVolume: lookup, attributes, read/write, rename, xattrs...
//        |  plain Swift calls, item = inode number
//   NTFSVolume           (Sources/NTFSToolDriver) - locking, Swift types, NTFSError
//        |  C calls
//   ntfstool_bridge.c    (Sources/NTFSToolBridge) - inode-number API over libntfstool-driver
//        |
//   libntfstool-driver           - the NTFS implementation
//        |  BlockDevice callbacks
//   ResourceDevice       (this file) - reads/writes the FSBlockDeviceResource FSKit gives us
//
// This file only translates between FSKit types and NTFSVolume calls. All NTFS
// logic lives in NTFSToolDriver / ntfstool_bridge, which are unit-tested against
// real NTFS images (Tests/NTFSToolDriverTests); the FSKit-independent decisions
// (ID mapping, option parsing, check policy) are in NTFSToolFSKitSupport.swift.
//
// Entry point: like Apple's own FSKit modules, the binary is linked with
// `-e _NSExtensionMain` (see scripts/build-fskit.sh) and the principal class is
// named by EXExtensionPrincipalClass in Info.plist. A Swift `@main` extension
// entry point would return from main after FSKit initialization and the process
// would exit, so mounting would fail.
//
// Error handling: NTFSVolume throws NTFSError(errno); posixError() turns it into
// the NSError FSKit expects (fs_errorForPOSIXError), so the kernel reports the
// same errno to the calling process.

import ExtensionFoundation
import FSKit
import Foundation
import os

/// Unified logging (view with `log stream --predicate 'subsystem == "<module bundle id>"'`).
let log = Logger(subsystem: Bundle.main.bundleIdentifier ?? "ntfstool.fskit", category: "fs")

/// Optional structured event log (JSON lines) in an App Group container shared
/// with the host app, so the app's diagnostics view can show module events.
/// Enabled only when Info.plist has `NTFSToolLogGroup`; otherwise a no-op.
let events = EventLog(url: (Bundle.main.object(forInfoDictionaryKey: "NTFSToolLogGroup") as? String).flatMap { EventLog.groupFile("module.log", group: $0) })
/// Errors (by errno and operation) seen during the current mount; summarized in the event log on unload.
let errorTally = ErrorTally()
/// Number of device I/O errors logged individually during this mount (capped to keep the log small).
var deviceErrorEvents = 0

/// Convert a driver error to the error FSKit expects, recording it for diagnostics.
func posixError(_ e: Error, op: String = #function) -> Error {
    let opName = String(op.prefix(while: { $0 != "(" }))
    if let n = e as? NTFSError {
        errorTally.record(n.code, op: opName)
        return fs_errorForPOSIXError(n.code)
    }
    errorTally.record(EIO, op: opName)
    return e
}

/// Record a device read/write failure. The first 20 are logged with details, later ones are only counted.
func deviceError(_ op: String, _ code: Int32, offset: UInt64, length: UInt64, _ error: Error) {
    errorTally.record(code, op: "device_" + op)
    deviceErrorEvents += 1
    if deviceErrorEvents <= 20 {
        events.event("device_error", ["op": op, "errno": Int(code), "off": offset, "len": length, "err": String(describing: error).prefix(200).description])
    }
}

/// Adapts FSKit's `FSBlockDeviceResource` to the driver's `BlockDevice` protocol.
///
/// Reads and writes go directly to the device with `read(into:startingAt:length:)`
/// and `write(from:startingAt:length:)`. These calls require offsets and lengths
/// that are multiples of the device block size; the bridge guarantees that.
final class ResourceDevice: BlockDevice {
    let resource: FSBlockDeviceResource
    let size: UInt64
    let blockSize: UInt32
    /// Largest single request passed to FSKit; bigger transfers are split.
    static let maxChunk: UInt64 = 1 << 20

    init(_ r: FSBlockDeviceResource) {
        resource = r
        // Use the larger of logical and physical block size so writes never need
        // a read-modify-write inside the device (e.g. 512e drives with 4K sectors).
        blockSize = UInt32(max(r.physicalBlockSize, r.blockSize, 512))
        size = r.blockSize * r.blockCount / UInt64(blockSize) * UInt64(blockSize)
    }

    /// errno carried by an FSKit error (POSIX domain), else EIO.
    static func errno(_ error: Error) -> Int32 {
        (error as NSError).domain == NSPOSIXErrorDomain ? Int32((error as NSError).code) : EIO
    }

    func read(into buffer: UnsafeMutableRawPointer, offset: UInt64, length: UInt64) -> Int32 {
        var done: UInt64 = 0
        do {
            while done < length {
                let n = min(length - done, Self.maxChunk)
                let p = UnsafeMutableRawBufferPointer(start: buffer + Int(done), count: Int(n))
                let got = try resource.read(into: p, startingAt: off_t(offset + done), length: Int(n))
                if got <= 0 { return EIO } // short read at a position that must exist
                done += UInt64(got)
            }
            return 0
        } catch {
            log.error("read failed off=\(offset) len=\(length): \(error.localizedDescription)")
            deviceError("read", Self.errno(error), offset: offset, length: length, error)
            return Self.errno(error)
        }
    }

    func write(from buffer: UnsafeRawPointer, offset: UInt64, length: UInt64) -> Int32 {
        var done: UInt64 = 0
        do {
            while done < length {
                let n = min(length - done, Self.maxChunk)
                let p = UnsafeRawBufferPointer(start: buffer + Int(done), count: Int(n))
                let put = try resource.write(from: p, startingAt: off_t(offset + done), length: Int(n))
                if put <= 0 { return EIO }
                done += UInt64(put)
            }
            return 0
        } catch {
            log.error("write failed off=\(offset) len=\(length): \(error.localizedDescription)")
            deviceError("write", Self.errno(error), offset: offset, length: length, error)
            return Self.errno(error)
        }
    }

    /// Writes above are passed to the device immediately and nothing is buffered
    /// in this module, so there is nothing extra to flush here.
    func sync() -> Int32 { 0 }
}

/// The file system object FSKit instantiates (named in Info.plist as EXExtensionPrincipalClass).
///
/// A "unary" file system serves one volume per resource (disk partition).
@objc(NTFSToolFileSystem)
final class NTFSToolFileSystem: FSUnaryFileSystem, FSUnaryFileSystemOperations {
    var volume: NTFSToolVolume?
    /// File system type name reported by statfs / shown in `mount` output: the module's FSShortName.
    static let typeName = NTFSToolFSKit.shortName(info: Bundle.main.infoDictionary)

    override init() {
        super.init()
        events.event("module_start", ["version": Bundle.main.object(forInfoDictionaryKey: "CFBundleShortVersionString") as? String ?? "",
                                      "build": Bundle.main.object(forInfoDictionaryKey: "CFBundleVersion") as? String ?? "",
                                      "os": ProcessInfo.processInfo.operatingSystemVersionString])
    }

    /// Called when the system asks "is this disk yours?". We answer `.usable` for
    /// NTFS and give the volume name and a stable container ID (from the serial).
    func probeResource(resource: FSResource, replyHandler: @escaping (FSProbeResult?, Error?) -> Void) {
        guard let block = resource as? FSBlockDeviceResource else {
            events.event("probe", ["result": "not_block_device"])
            replyHandler(.notRecognized, nil); return
        }
        let dev = ResourceDevice(block)
        guard let serial = NTFSVolume.probe(dev) else {
            events.event("probe", ["result": "not_ntfs", "bsd": block.bsdName, "size": block.blockCount * block.blockSize])
            replyHandler(.notRecognized, nil); return
        }
        // Mount read-only briefly to read the label (shown in Disk Utility / mount dialogs).
        var name = "NTFS"
        if let v = try? NTFSVolume(device: dev, readOnly: true) {
            name = (try? v.label()).flatMap { $0.isEmpty ? nil : $0 } ?? name
            try? v.unmount()
        }
        log.info("probe: NTFS serial=\(serial, format: .hex) name=\(name)")
        events.event("probe", ["result": "usable", "bsd": block.bsdName, "size": block.blockCount * block.blockSize,
                               "block": block.blockSize, "writable": block.isWritable])
        replyHandler(.usable(name: name, containerID: FSContainerIdentifier(uuid: NTFSToolFSKit.volumeUUID(serial: serial))), nil)
    }

    /// Open the volume with libntfstool-driver and hand FSKit an FSVolume for it.
    func loadResource(resource: FSResource, options: FSTaskOptions, replyHandler: @escaping (FSVolume?, Error?) -> Void) {
        guard let block = resource as? FSBlockDeviceResource else {
            events.event("load", ["ok": false, "errno": Int(ENODEV), "why": "not_block_device"])
            replyHandler(nil, fs_errorForPOSIXError(ENODEV)); return
        }
        let opts = NTFSToolFSKit.MountOptions(options.taskOptions)
        let started = Date()
        errorTally.reset()
        deviceErrorEvents = 0
        do {
            let dev = ResourceDevice(block)
            // A volume left hibernated by Windows (or Fast Startup) must not be
            // written; fall back to read-only so the user can still read it.
            let opened = try NTFSToolFSKit.openWithReadOnlyFallback(readOnly: opts.readOnly || !block.isWritable) { ro in
                try NTFSVolume(device: dev, readOnly: ro)
            }
            let ntfs = opened.value
            ntfs.windowsNames = opts.windowsNames // usually empty here; `mount -o` options arrive in activate
            if opened.fellBack { log.info("load: volume is hibernated or write-protected, mounted read-only") }
            let v = NTFSToolVolume(ntfs)
            volume = v
            containerStatus = .ready
            log.info("load: mounted readOnly=\(ntfs.readOnly, privacy: .public) options=\(options.taskOptions.joined(separator: " "), privacy: .public)")
            events.event("load", ["ok": true, "bsd": block.bsdName, "ro_req": opts.readOnly, "media_writable": block.isWritable,
                                  "fell_back": opened.fellBack, "ro": ntfs.readOnly, "ms": Int(Date().timeIntervalSince(started) * 1000),
                                  "options": options.taskOptions.joined(separator: " ")])
            replyHandler(v, nil)
        } catch {
            log.error("load failed: \(String(describing: error))")
            events.event("load", ["ok": false, "bsd": block.bsdName, "ro_req": opts.readOnly, "media_writable": block.isWritable,
                                  "errno": Int((error as? NTFSError)?.code ?? EIO), "err": String(describing: error).prefix(300).description,
                                  "ms": Int(Date().timeIntervalSince(started) * 1000), "options": options.taskOptions.joined(separator: " ")])
            replyHandler(nil, posixError(error))
        }
    }

    /// Release the volume: libntfstool-driver writes back the volume state and the device is flushed.
    func unloadResource(resource: FSResource, options: FSTaskOptions, replyHandler: @escaping (Error?) -> Void) {
        do {
            try volume?.ntfs.unmount()
            events.event("unload", ["ok": true, "errors": errorTally.summary()])
            replyHandler(nil)
        } catch {
            events.event("unload", ["ok": false, "errno": Int((error as? NTFSError)?.code ?? EIO), "errors": errorTally.summary()])
            replyHandler(posixError(error))
        }
        volume = nil
    }
}

/// File system check / format requests (fsck_fskit, newfs_fskit).
///
/// DiskArbitration runs a quick check (`-q`) before auto-mounting and mounts
/// only if it succeeds. The policy is in `NTFSToolFSKit.checkError` (unit tested).
extension NTFSToolFileSystem: FSManageableResourceMaintenanceOperations {
    func startCheck(task: FSTask, options: FSTaskOptions) throws -> Progress {
        let mode: NTFSToolFSKit.CheckMode
        do { mode = try NTFSToolFSKit.CheckMode(options.taskOptions) } catch {
            events.event("check", ["options": options.taskOptions.joined(separator: " "), "errno": Int(EINVAL)])
            throw fs_errorForPOSIXError(EINVAL)
        }
        if let code = NTFSToolFSKit.checkError(mode: mode, volumeLoaded: volume != nil) {
            log.info("check: mode=\(String(describing: mode)) -> errno \(code)")
            events.event("check", ["mode": String(describing: mode), "errno": Int(code), "loaded": volume != nil])
            throw fs_errorForPOSIXError(code)
        }
        events.event("check", ["mode": String(describing: mode), "errno": 0])
        // The check itself is instantaneous; complete the task asynchronously as FSKit expects.
        let progress = Progress(totalUnitCount: 1)
        DispatchQueue.global(qos: .utility).async {
            progress.completedUnitCount = 1
            task.didComplete(error: nil)
        }
        return progress
    }

    /// Formatting is not supported (use mkntfs or Windows).
    func startFormat(task: FSTask, options: FSTaskOptions) throws -> Progress {
        throw fs_errorForPOSIXError(ENOTSUP)
    }
}

/// FSKit item for one NTFS file or directory.
///
/// FSKit keeps FSItem objects alive while the kernel references the vnode, and
/// tells us via `reclaimItem` when it is done. We remember the parent inode for
/// `..` and `parentID`, and, for directories, the current enumeration snapshot.
final class NItem: FSItem {
    let ino: UInt64
    var parent: UInt64
    var snapshot: NTFSToolFSKit.DirectorySnapshot?
    init(ino: UInt64, parent: UInt64) {
        self.ino = ino
        self.parent = parent
    }
}

/// The mounted volume as seen by FSKit.
///
/// Concurrency: FSKit may call these methods concurrently from several threads.
/// NTFSVolume serializes all NTFS access with its own lock; `items` below only
/// protects this class's item registry and directory snapshots.
final class NTFSToolVolume: FSVolume, FSVolume.Operations, FSVolume.OpenCloseOperations, FSVolume.ReadWriteOperations,
    FSVolume.RenameOperations, FSVolume.XattrOperations {
    let ntfs: NTFSVolume
    /// Protects `registry`, `generation` and NItem.snapshot.
    let items = NSLock()
    /// One NItem per inode number, so FSKit always gets the same object for the same file.
    var registry: [UInt64: NItem] = [:]
    /// Bumped on every directory modification; used as the enumeration verifier.
    var generation: UInt64 = 1
    lazy var root = item(NTFSVolume.rootIno, parent: NTFSVolume.rootIno)

    init(_ ntfs: NTFSVolume) {
        self.ntfs = ntfs
        let label = (try? ntfs.label()) ?? "NTFS"
        super.init(volumeID: FSVolume.Identifier(uuid: NTFSToolFSKit.volumeUUID(serial: ntfs.serial)),
                   volumeName: FSFileName(string: label.isEmpty ? "NTFS" : label))
    }

    /// The registered NItem for `ino`, creating it if needed.
    func item(_ ino: UInt64, parent: UInt64) -> NItem {
        items.lock()
        defer { items.unlock() }
        if let i = registry[ino] { i.parent = parent; return i }
        let i = NItem(ino: ino, parent: parent)
        registry[ino] = i
        return i
    }

    /// A directory's contents changed: invalidate its snapshot and change the verifier.
    func changed(_ dir: FSItem) {
        items.lock()
        generation += 1
        (dir as? NItem)?.snapshot = nil
        items.unlock()
    }

    func n(_ i: FSItem) -> NItem { i as! NItem }

    /// FSKit can pass a nil object where the Swift signature promises a non-optional
    /// FSItem (observed for the replaced target after a rename). Touching it would
    /// crash, so every entry point that receives an item checks this first.
    func isNull(_ i: FSItem) -> Bool { unsafeBitCast(i, to: UInt.self) == 0 }

    func name(_ f: FSFileName) throws -> String {
        guard let s = f.string, !s.isEmpty else { throw NTFSError(EINVAL) }
        return s
    }

    // MARK: Volume capabilities and statistics

    var supportedVolumeCapabilities: FSVolume.SupportedCapabilities {
        let c = FSVolume.SupportedCapabilities()
        c.supportsPersistentObjectIDs = true   // item IDs are MFT record numbers
        c.supportsSymbolicLinks = true
        c.supportsHardLinks = false            // createLink is not implemented
        c.supportsSparseFiles = true
        c.supports2TBFiles = true
        c.supportsHiddenFiles = true           // NTFS hidden attribute <-> UF_HIDDEN
        c.supports64BitObjectIDs = true
        c.supportsFastStatFS = true
        c.doesNotSupportSettingFilePermissions = true
        c.caseFormat = .sensitive              // POSIX names, the usual default
        return c
    }

    var volumeStatistics: FSStatFSResult {
        let r = FSStatFSResult(fileSystemTypeName: NTFSToolFileSystem.typeName)
        if let s = try? ntfs.statfs() {
            r.blockSize = Int(s.clusterSize)
            r.ioSize = 1 << 20
            r.totalBlocks = s.totalClusters
            r.freeBlocks = s.freeClusters
            r.availableBlocks = s.freeClusters
            r.usedBlocks = s.totalClusters - s.freeClusters
            r.totalBytes = s.totalBytes
            r.freeBytes = s.freeBytes
            r.availableBytes = s.freeBytes
            r.usedBytes = s.totalBytes - s.freeBytes
            r.totalFiles = s.totalFiles
            r.freeFiles = s.freeFiles
        }
        return r
    }

    var maximumLinkCount: Int { 1024 }
    var maximumNameLength: Int { 255 }         // NTFS: 255 UTF-16 code units
    var restrictsOwnershipChanges: Bool { false }
    var truncatesLongNames: Bool { false }
    var maximumFileSize: UInt64 { 1 << 44 }
    var maximumXattrSize: Int { 64 << 20 }     // matches macOS XATTR_MAXSIZE

    // MARK: Lifecycle

    func mount(options: FSTaskOptions, replyHandler: @escaping (Error?) -> Void) { replyHandler(nil) }

    func unmount(replyHandler: @escaping () -> Void) {
        try? ntfs.sync()
        replyHandler()
    }

    /// sync(2) / fsync / unmount: flush the volume.
    func synchronize(flags: FSSyncFlags, replyHandler: @escaping (Error?) -> Void) {
        do { try ntfs.sync(); replyHandler(nil) } catch { replyHandler(posixError(error)) }
    }

    /// Called when the volume is attached to a mount point; returns the root item.
    func activate(options: FSTaskOptions, replyHandler: @escaping (FSItem?, Error?) -> Void) {
        // `mount -o ...` options are delivered here, not in loadResource.
        let opts = NTFSToolFSKit.MountOptions(options.taskOptions)
        if opts.windowsNames { ntfs.windowsNames = true }
        log.info("activate: windowsNames=\(self.ntfs.windowsNames, privacy: .public) options=\(options.taskOptions.joined(separator: " "), privacy: .public)")
        events.event("activate", ["ro": ntfs.readOnly, "options": options.taskOptions.joined(separator: " ")])
        replyHandler(root, nil)
    }

    func deactivate(options: FSDeactivateOptions = [], replyHandler: @escaping (Error?) -> Void) {
        events.event("deactivate", ["errors": errorTally.summary()])
        do { try ntfs.sync(); replyHandler(nil) } catch { replyHandler(posixError(error)) }
    }

}
