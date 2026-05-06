// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// Integration tests of the NTFS engine (NTFSVolume -> ntfstool_bridge -> libntfstool-driver)
// on real NTFS images.
//
// Images are created with the driver's mkntfs. The block device is a FileBlockDevice
// with 4096-byte strict alignment (like an FSKit block device), so every test also
// exercises the bridge's alignment layer. After each test `ntfsfix -n` checks the
// image for structural damage.
//
// mkntfs / ntfsfix are looked up in $NTFSTOOL_NTFS_TOOLS, then in the Homebrew
// directories; if they are missing the tests are skipped.
import NTFSToolDriver
import Foundation
import XCTest

final class NTFSVolumeTests: XCTestCase {
    static let tools: URL? = {
        let env = ProcessInfo.processInfo.environment["NTFSTOOL_NTFS_TOOLS"].map { [$0] } ?? []
        return (env + ["/opt/homebrew/sbin", "/opt/homebrew/bin", "/usr/local/sbin", "/usr/local/bin"])
            .map { URL(fileURLWithPath: $0) }
            .first { d in ["mkntfs", "ntfsfix"].allSatisfy { FileManager.default.isExecutableFile(atPath: d.appendingPathComponent($0).path) } }
    }()
    var dir: URL!
    var image: URL!
    var vol: NTFSVolume!
    var dev: FileBlockDevice!
    let R = NTFSVolume.rootIno

    static func sh(_ path: String, _ args: [String]) -> (Int32, String) {
        let p = Process()
        p.executableURL = URL(fileURLWithPath: path)
        p.arguments = args
        let pipe = Pipe()
        p.standardOutput = pipe
        p.standardError = pipe
        do { try p.run() } catch { return (-1, "\(error)") }
        let data = pipe.fileHandleForReading.readDataToEndOfFile()
        p.waitUntilExit()
        return (p.terminationStatus, String(decoding: data, as: UTF8.self))
    }

    /// Create a fresh NTFS image of `mb` MiB.
    static func makeImage(at url: URL, mb: Int = 64, label: String = "Test Volume") throws {
        guard let tools = tools else { throw XCTSkip("needs the driver's mkntfs / ntfsfix: set NTFSTOOL_NTFS_TOOLS=<directory>") }
        FileManager.default.createFile(atPath: url.path, contents: nil)
        let h = try FileHandle(forWritingTo: url)
        try h.truncate(atOffset: UInt64(mb) << 20)
        try h.close()
        let (rc, out) = sh(tools.appendingPathComponent("mkntfs").path, ["-F", "-Q", "-L", label, url.path])
        XCTAssertEqual(rc, 0, out)
    }

    override func setUpWithError() throws {
        dir = FileManager.default.temporaryDirectory.appendingPathComponent("ntfstool-\(UUID().uuidString)")
        try FileManager.default.createDirectory(at: dir, withIntermediateDirectories: true)
        image = dir.appendingPathComponent("ntfs.img")
        try Self.makeImage(at: image)
        dev = try FileBlockDevice(path: image.path)
        vol = try NTFSVolume(device: dev)
    }

    override func tearDownWithError() throws {
        try? vol?.unmount()
        vol = nil
        dev = nil
        // Consistency check after every test: any structural damage shows up here.
        if let tools = Self.tools, let image = image, FileManager.default.fileExists(atPath: image.path) {
            let (rc, out) = Self.sh(tools.appendingPathComponent("ntfsfix").path, ["-n", image.path])
            XCTAssertEqual(rc, 0, "ntfsfix -n failed: \(out)")
        }
        try? FileManager.default.removeItem(at: dir)
    }

    func remount(readOnly: Bool = false) throws {
        try vol.unmount()
        dev = try FileBlockDevice(path: image.path)
        vol = try NTFSVolume(device: dev, readOnly: readOnly)
    }

    func posix(_ code: Int32, _ body: () throws -> Void, file: StaticString = #filePath, line: UInt = #line) {
        XCTAssertThrowsError(try body(), file: file, line: line) { XCTAssertEqual(($0 as? NTFSError)?.code, code, "\($0)", file: file, line: line) }
    }

    /// Deterministic pseudo-random bytes (xorshift).
    func random(_ n: Int, seed: UInt64 = 1) -> Data {
        var x = seed &* 0x9E3779B97F4A7C15 | 1
        var d = Data(count: n)
        d.withUnsafeMutableBytes { p in
            let b = p.bindMemory(to: UInt8.self)
            for i in 0..<n { x ^= x << 13; x ^= x >> 7; x ^= x << 17; b[i] = UInt8(truncatingIfNeeded: x) }
        }
        return d
    }

    func names(_ d: UInt64) throws -> [String] { try vol.list(d).map(\.name).sorted() }

    // MARK: - Probe / mount

    func testProbeAndMountBasics() throws {
        XCTAssertEqual(NTFSVolume.probe(dev), vol.serial)
        XCTAssertNotEqual(vol.serial, 0)
        XCTAssertEqual(try vol.label(), "Test Volume")
        XCTAssertFalse(vol.readOnly)
        let st = try vol.statfs()
        XCTAssertEqual(st.clusterSize, 4096)
        XCTAssertGreaterThan(st.freeBytes, 50 << 20)
        XCTAssertLessThanOrEqual(st.freeBytes, st.totalBytes)
        XCTAssertEqual(try vol.attributes(R).type, .directory)
        XCTAssertEqual(try names(R), [], "metadata files such as $MFT are not listed")
        posix(ENOENT) { _ = try vol.lookup("$MFT", in: R) }
    }

    func testNonNTFSIsRejected() throws {
        let other = dir.appendingPathComponent("blank.img")
        FileManager.default.createFile(atPath: other.path, contents: Data(count: 4 << 20))
        let d = try FileBlockDevice(path: other.path)
        XCTAssertNil(NTFSVolume.probe(d))
        posix(EINVAL) { _ = try NTFSVolume(device: d) }
    }

}
