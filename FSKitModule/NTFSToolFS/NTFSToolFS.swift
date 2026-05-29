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
