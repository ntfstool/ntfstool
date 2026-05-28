// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// Unit tests for the FSKit-independent helpers used by the FSKit module.
import NTFSToolDriver
import Foundation
import XCTest

final class FSKitSupportTests: XCTestCase {
    func testRootIDMappingIsAnInvolution() {
        XCTAssertEqual(NTFSToolFSKit.itemID(forIno: NTFSVolume.rootIno), 2)
        XCTAssertEqual(NTFSToolFSKit.ino(forItemID: 2), NTFSVolume.rootIno)
        for ino: UInt64 in [16, 64, 123_456] {
            XCTAssertEqual(NTFSToolFSKit.ino(forItemID: NTFSToolFSKit.itemID(forIno: ino)), ino)
        }
    }

    func testDefaultShortName() {
        XCTAssertEqual(NTFSToolFSKit.shortName, "ntfstooloss")
        XCTAssertNotEqual(NTFSToolFSKit.shortName, "ntfs", "must not clash with Apple's built-in NTFS module")
    }

    func testModes() {
        XCTAssertEqual(NTFSToolFSKit.mode(for: .directory) & UInt32(S_IFMT), UInt32(S_IFDIR))
        XCTAssertEqual(NTFSToolFSKit.mode(for: .file) & 0o777, 0o666)
        XCTAssertEqual(NTFSToolFSKit.mode(for: .symlink) & UInt32(S_IFMT), UInt32(S_IFLNK))
    }

    func testVolumeUUIDIsStablePerSerial() {
        let a = NTFSToolFSKit.volumeUUID(serial: 0x1234_5678_9ABC_DEF0)
        XCTAssertEqual(a, NTFSToolFSKit.volumeUUID(serial: 0x1234_5678_9ABC_DEF0))
        XCTAssertNotEqual(a, NTFSToolFSKit.volumeUUID(serial: 1))
    }

    func testMountOptionParsing() {
        XCTAssertFalse(NTFSToolFSKit.MountOptions([]).readOnly)
        XCTAssertTrue(NTFSToolFSKit.MountOptions(["-r"]).readOnly)
        XCTAssertTrue(NTFSToolFSKit.MountOptions(["-o", "nobrowse,ro"]).readOnly)
        XCTAssertTrue(NTFSToolFSKit.MountOptions(["-ordonly"]).readOnly)
        XCTAssertTrue(NTFSToolFSKit.MountOptions(["rdonly"]).readOnly)
        XCTAssertFalse(NTFSToolFSKit.MountOptions(["-o", "nobrowse,rw"]).readOnly)
        XCTAssertFalse(NTFSToolFSKit.MountOptions(["-o"]).readOnly)
    }

    func testDirectorySnapshotPaging() {
        let e = (0..<5).map { NTFSDirEntry(name: "f\($0)", ino: UInt64(100 + $0), type: .file) }
        let s = NTFSToolFSKit.DirectorySnapshot(entries: e, verifier: 7)
        XCTAssertEqual(s.page(from: 0).count, 5)
        XCTAssertEqual(s.page(from: 3).map(\.name), ["f3", "f4"])
        XCTAssertTrue(s.page(from: 99).isEmpty)
        XCTAssertEqual(s.cookie(after: 3), 4)
    }

    func testErrorDescriptions() {
        XCTAssertEqual(NTFSToolFSKit.describe(EPERM), "hibernated_or_fast_startup")
        XCTAssertEqual(NTFSToolFSKit.describe(EINVAL), "not_ntfs")
        XCTAssertEqual(NTFSToolFSKit.describe(EIO), "io_error")
        XCTAssertEqual(NTFSToolFSKit.describe(EROFS), "read_only")
        XCTAssertEqual(NTFSToolFSKit.describe(ENOSPC), "no_space")
        XCTAssertEqual(NTFSToolFSKit.describe(ENOENT), "errno_2")
    }
}

/// Auto-mount support: DiskArbitration needs a passing quick check before it mounts,
/// and hibernated / Fast Startup volumes must fall back to read-only.
final class AutoMountSupportTests: XCTestCase {
    func testCheckModeParsing() throws {
        XCTAssertEqual(try NTFSToolFSKit.CheckMode(["-q"]), .quick)
        XCTAssertEqual(try NTFSToolFSKit.CheckMode([]), .quick, "no arguments means a quick check")
        XCTAssertEqual(try NTFSToolFSKit.CheckMode(["-n"]), .verify)
        XCTAssertEqual(try NTFSToolFSKit.CheckMode(["-y"]), .repair)
        XCTAssertEqual(try NTFSToolFSKit.CheckMode(["-q", "-y"]), .repair, "a repair flag wins")
        XCTAssertThrowsError(try NTFSToolFSKit.CheckMode(["-x"])) { XCTAssertEqual(($0 as? NTFSError)?.code, EINVAL) }
    }

    func testCheckResult() {
        XCTAssertNil(NTFSToolFSKit.checkError(mode: .quick, volumeLoaded: true), "a loadable volume passes the quick check")
        XCTAssertNil(NTFSToolFSKit.checkError(mode: .verify, volumeLoaded: true))
        XCTAssertEqual(NTFSToolFSKit.checkError(mode: .quick, volumeLoaded: false), ENXIO)
        XCTAssertEqual(NTFSToolFSKit.checkError(mode: .repair, volumeLoaded: true), ENOTSUP, "repair is left to Windows chkdsk")
    }

    func testHibernatedOrReadOnlyMediaFallsBackToReadOnly() throws {
        var calls: [Bool] = []
        let r = try NTFSToolFSKit.openWithReadOnlyFallback(readOnly: false) { ro -> String in
            calls.append(ro)
            if !ro { throw NTFSError(EPERM) }
            return "ro-volume"
        }
        XCTAssertEqual(r.value, "ro-volume")
        XCTAssertTrue(r.fellBack)
        XCTAssertEqual(calls, [false, true])

        calls = []
        let ok = try NTFSToolFSKit.openWithReadOnlyFallback(readOnly: false) { ro -> String in calls.append(ro); return "rw" }
        XCTAssertFalse(ok.fellBack)
        XCTAssertEqual(calls, [false])

        XCTAssertThrowsError(try NTFSToolFSKit.openWithReadOnlyFallback(readOnly: false) { _ -> String in throw NTFSError(EIO) },
                             "a damaged volume (EIO) is reported, not mounted read-only") { XCTAssertEqual(($0 as? NTFSError)?.code, EIO) }
        calls = []
        _ = try? NTFSToolFSKit.openWithReadOnlyFallback(readOnly: true) { ro -> String in calls.append(ro); throw NTFSError(EPERM) }
        XCTAssertEqual(calls, [true], "no retry when read-only was requested anyway")
    }

    func testWritableOpenOnRealImageDoesNotFallBack() throws {
        let dir = FileManager.default.temporaryDirectory.appendingPathComponent("am-\(UUID().uuidString.prefix(6))")
        try FileManager.default.createDirectory(at: dir, withIntermediateDirectories: true)
        defer { try? FileManager.default.removeItem(at: dir) }
        let img = dir.appendingPathComponent("a.img")
        try NTFSVolumeTests.makeImage(at: img, mb: 32)
        let dev = try FileBlockDevice(path: img.path)
        let r = try NTFSToolFSKit.openWithReadOnlyFallback(readOnly: false) { ro in try NTFSVolume(device: dev, readOnly: ro) }
        XCTAssertFalse(r.fellBack)
        XCTAssertFalse(r.value.readOnly)
        try r.value.unmount()
    }

    func testShortNameComesFromModuleInfoPlist() {
        XCTAssertEqual(NTFSToolFSKit.shortName(info: ["EXAppExtensionAttributes": ["FSShortName": "custom"]]), "custom")
        XCTAssertEqual(NTFSToolFSKit.shortName(info: ["EXAppExtensionAttributes": ["FSShortName": ""]]), NTFSToolFSKit.shortName)
        XCTAssertEqual(NTFSToolFSKit.shortName(info: [:]), NTFSToolFSKit.shortName, "falls back to the default")
        XCTAssertEqual(NTFSToolFSKit.shortName(info: nil), "ntfstooloss")
    }

    func testMountOptionsWindowsNames() {
        XCTAssertFalse(NTFSToolFSKit.MountOptions(["-o", "rdonly"]).windowsNames)
        XCTAssertTrue(NTFSToolFSKit.MountOptions(["-o", "windows_names"]).windowsNames)
        let both = NTFSToolFSKit.MountOptions(["-o", "rdonly,windows_names"])
        XCTAssertTrue(both.readOnly && both.windowsNames)
        XCTAssertTrue(NTFSToolFSKit.MountOptions(["-owindows_names"]).windowsNames)
    }
}
