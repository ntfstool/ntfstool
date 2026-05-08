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

    // MARK: - Read / write

    func testCreateWriteReadBackAndPersist() throws {
        let f = try vol.create("hello.txt", in: R, type: .file)
        XCTAssertEqual(try vol.write(f, offset: 0, data: Data("hello ntfs".utf8)), 10)
        XCTAssertEqual(try vol.read(f, length: 100), Data("hello ntfs".utf8))
        XCTAssertEqual(try vol.attributes(f).size, 10)
        try vol.flush(f)
        try remount()
        let g = try vol.lookup("hello.txt", in: R)
        XCTAssertEqual(g, f)
        XCTAssertEqual(try vol.read(g, length: 100), Data("hello ntfs".utf8))
        posix(ENOENT) { _ = try vol.lookup("HELLO.TXT", in: R) } // POSIX (case-sensitive) lookups, like other NTFS drivers
    }

    func testUnalignedAndLargeWritesThroughAlignmentLayer() throws {
        let f = try vol.create("big.bin", in: R, type: .file)
        let big = random(9 << 20 + 777)
        try vol.write(f, offset: 0, data: big)
        // Unaligned overwrite crossing a block boundary.
        let patch = random(5000, seed: 7)
        try vol.write(f, offset: 4093, data: patch)
        var expect = big
        expect.replaceSubrange(4093..<(4093 + 5000), with: patch)
        // Unaligned append.
        try vol.write(f, offset: UInt64(expect.count), data: Data("tail".utf8))
        expect.append(Data("tail".utf8))
        try remount()
        let g = try vol.lookup("big.bin", in: R)
        XCTAssertEqual(try vol.attributes(g).size, UInt64(expect.count))
        XCTAssertEqual(try vol.read(g, length: expect.count + 100), expect)
        XCTAssertEqual(try vol.read(g, offset: 4090, length: 10), expect[4090..<4100])
        XCTAssertEqual(dev.misaligned, 0, "every device request must be block aligned")
    }

    /// Writes are write-through: once a call returns, another mount of the same
    /// device sees the data without an explicit sync (as after a sudden unplug).
    func testWritesAreVisibleOnDeviceImmediately() throws {
        let d = try vol.create("dir", in: R, type: .directory)
        let f = try vol.create("now.txt", in: d, type: .file)
        try vol.write(f, offset: 0, data: Data(repeating: 1, count: 10_000))
        try vol.write(f, offset: 4090, data: Data("XYZ".utf8)) // small unaligned overwrite
        let expect = Data([1]) + Data("XYZ".utf8) + Data([1])
        XCTAssertEqual(try vol.read(f, offset: 4089, length: 5), expect)
        let other = try NTFSVolume(device: try FileBlockDevice(path: image.path, readOnly: true), readOnly: true)
        XCTAssertEqual(try other.read(try other.resolve("dir/now.txt"), offset: 4089, length: 5), expect)
        try other.unmount()
    }

    func testSparseWriteAndTruncate() throws {
        let f = try vol.create("sparse", in: R, type: .file)
        try vol.write(f, offset: 3 << 20, data: Data("x".utf8))
        XCTAssertEqual(try vol.attributes(f).size, (3 << 20) + 1)
        XCTAssertEqual(try vol.read(f, offset: 1000, length: 4), Data(count: 4), "holes read as zeros")
        try vol.truncate(f, size: 10)
        XCTAssertEqual(try vol.attributes(f).size, 10)
        try vol.truncate(f, size: 1 << 20)
        XCTAssertEqual(try vol.read(f, offset: (1 << 20) - 2, length: 10).count, 2)
        XCTAssertEqual(try vol.read(f, offset: 2 << 20, length: 10).count, 0, "nothing past end of file")
    }

    func testFreeSpaceTracksWrites() throws {
        let before = try vol.statfs().freeBytes
        let f = try vol.create("fill", in: R, type: .file)
        try vol.write(f, offset: 0, data: random(8 << 20))
        let after = try vol.statfs().freeBytes
        XCTAssertLessThanOrEqual(after, before - (8 << 20))
        try vol.remove("fill", in: R)
        XCTAssertGreaterThanOrEqual(try vol.statfs().freeBytes, before - (64 << 10))
    }

    // MARK: - Directories / names

    func testDirectoriesAndUnicodeNames() throws {
        let d = try vol.create("Données Photos", in: R, type: .directory)
        let sub = try vol.create("2026", in: d, type: .directory)
        _ = try vol.create("Résumé Ω.docx", in: sub, type: .file)
        let nfd = "Cafe\u{301}.txt" // decomposed form, as the Finder often sends it
        _ = try vol.create(nfd, in: d, type: .file)
        XCTAssertEqual(try names(R), ["Données Photos"])
        XCTAssertEqual(try vol.list(d).count, 2)
        XCTAssertNoThrow(try vol.lookup("Café.txt", in: d), "an NFC name finds a file created with NFD")
        XCTAssertEqual(try vol.resolve("Données Photos/2026/Résumé Ω.docx"), try vol.lookup("Résumé Ω.docx", in: sub))
        XCTAssertEqual(try vol.attributes(d).type, .directory)
        posix(EEXIST) { _ = try vol.create("2026", in: d, type: .file) }
        posix(ENOTDIR) { _ = try vol.create("x", in: try vol.resolve("Données Photos/2026/Résumé Ω.docx"), type: .file) }
        posix(EINVAL) { _ = try vol.create("a/b", in: R, type: .file) }
        posix(ENAMETOOLONG) { _ = try vol.create(String(repeating: "é", count: 256), in: R, type: .file) }
    }

    func testRemoveFilesAndDirectories() throws {
        let d = try vol.create("dir", in: R, type: .directory)
        _ = try vol.create("f", in: d, type: .file)
        posix(ENOTEMPTY) { try vol.remove("dir", in: R) }
        try vol.remove("f", in: d)
        try vol.remove("dir", in: R)
        XCTAssertEqual(try names(R), [])
        posix(ENOENT) { try vol.remove("nope", in: R) }
    }

    /// windows_names: the usual rules (ntfsdrv_forbidden_names): forbidden
    /// characters, reserved device names, trailing dot or space.
    func testWindowsNamesRule() throws {
        XCTAssertFalse(vol.windowsNames, "off by default")
        _ = try vol.create("off-a:b.txt", in: R, type: .file)
        vol.windowsNames = true
        for bad in ["a:b.txt", "q?.txt", "star*.txt", "pipe|.txt", "lt<.txt", "quote\".txt", "CON", "aux.txt", "dot.", "space "] {
            posix(EINVAL) { _ = try vol.create(bad, in: R, type: .file) }
        }
        posix(EINVAL) { _ = try vol.create("dir:x", in: R, type: .directory) }
        posix(EINVAL) { _ = try vol.symlink("ln?", in: R, target: "x") }
        let ok = try vol.create("Ünïcode ok.txt", in: R, type: .file)
        XCTAssertGreaterThan(ok, 0)
        posix(EINVAL) { try vol.rename("Ünïcode ok.txt", in: R, to: "re:name.txt", in: R) }
        XCTAssertTrue(try names(R).contains("Ünïcode ok.txt"), "a rejected rename leaves the file untouched")
        try vol.rename("Ünïcode ok.txt", in: R, to: "renamed.txt", in: R)
        vol.windowsNames = false
        _ = try vol.create("again:ok.txt", in: R, type: .file)
    }

    func testRenameCases() throws {
        let a = try vol.create("a.txt", in: R, type: .file)
        try vol.write(a, offset: 0, data: Data("A".utf8))
        let b = try vol.create("b.txt", in: R, type: .file)
        try vol.write(b, offset: 0, data: Data("B".utf8))
        let d = try vol.create("docs", in: R, type: .directory)

        // Same directory.
        try vol.rename("a.txt", in: R, to: "a2.txt", in: R)
        XCTAssertEqual(try vol.lookup("a2.txt", in: R), a)
        posix(ENOENT) { _ = try vol.lookup("a.txt", in: R) }
        // Case-only change.
        try vol.rename("a2.txt", in: R, to: "A2.TXT", in: R)
        XCTAssertTrue(try names(R).contains("A2.TXT"))
        XCTAssertFalse(try names(R).contains("a2.txt"))
        XCTAssertEqual(try vol.read(a, length: 5), Data("A".utf8), "content survives a case-only rename")
        // Replace an existing file.
        try vol.rename("A2.TXT", in: R, to: "b.txt", in: R)
        XCTAssertEqual(try vol.read(try vol.lookup("b.txt", in: R), length: 5), Data("A".utf8))
        // Move to another directory.
        try vol.rename("b.txt", in: R, to: "moved.txt", in: d)
        XCTAssertEqual(try names(d), ["moved.txt"])
        // Move a non-empty directory.
        let e = try vol.create("archive", in: R, type: .directory)
        try vol.rename("docs", in: R, to: "docs-2026", in: e)
        XCTAssertEqual(try vol.resolve("archive/docs-2026/moved.txt"), a)
        // A directory cannot move into its own subtree.
        posix(EINVAL) { try vol.rename("archive", in: R, to: "loop", in: try vol.resolve("archive/docs-2026")) }
        // A directory cannot replace a file.
        _ = try vol.create("file", in: R, type: .file)
        posix(ENOTDIR) { try vol.rename("archive", in: R, to: "file", in: R) }
        XCTAssertEqual(try names(R), ["archive", "file"])
        try remount()
        XCTAssertEqual(try vol.read(try vol.resolve("archive/docs-2026/moved.txt"), length: 5), Data("A".utf8))
    }

    func testTimesHiddenAndSymlink() throws {
        let f = try vol.create("t.txt", in: R, type: .file)
        let when = timespec(tv_sec: 1_700_000_000, tv_nsec: 500_000_000)
        try vol.setTimes(f, access: when, modify: when, birth: when)
        try vol.setHidden(f, true)
        try remount()
        let a = try vol.attributes(f)
        XCTAssertEqual(a.modifyTime.tv_sec, when.tv_sec)
        XCTAssertEqual(a.birthTime.tv_sec, when.tv_sec)
        XCTAssertTrue(a.hidden)
        try vol.setHidden(f, false)
        XCTAssertFalse(try vol.attributes(f).hidden)

        let l = try vol.symlink("link", in: R, target: "t.txt")
        XCTAssertEqual(try vol.attributes(l).type, .symlink)
        XCTAssertEqual(try vol.readlink(l), "t.txt")
        posix(EINVAL) { _ = try vol.readlink(f) }
    }

}
