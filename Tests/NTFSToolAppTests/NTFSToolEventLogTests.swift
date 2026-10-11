// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolEventLogTests.swift
//
// Tests for the local event log used by the FSKit driver
// (Sources/NTFSToolCore/NTFSToolEventLog.swift): JSON lines, rotation,
// reading the tail, and the errno tally.

@testable import NTFSToolCore
import XCTest

final class EventLogTests: XCTestCase {
    var dir: URL!
    override func setUp() {
        dir = FileManager.default.temporaryDirectory.appendingPathComponent("evlog-\(UUID().uuidString.prefix(6))")
    }
    override func tearDown() { try? FileManager.default.removeItem(at: dir) }

    func testWritesJSONLinesWithTimeAndFields() throws {
        let log = EventLog(url: dir.appendingPathComponent("a.log"), now: { Date(timeIntervalSince1970: 1_760_000_000) })
        log.event("load", ["ok": false, "errno": 5, "ro_req": false])
        log.event("probe", ["result": "usable"])
        let lines = log.tail(maxBytes: 10_000).split(separator: "\n")
        XCTAssertEqual(lines.count, 2)
        let first = try XCTUnwrap(JSONSerialization.jsonObject(with: Data(lines[0].utf8)) as? [String: Any])
        XCTAssertEqual(first["ev"] as? String, "load")
        XCTAssertEqual(first["errno"] as? Int, 5)
        XCTAssertEqual(first["t"] as? String, "2025-10-09T08:53:20Z")
        XCTAssertNotNil(first["pid"])
    }

    func testRotatesAndTailKeepsNewest() {
        let log = EventLog(url: dir.appendingPathComponent("b.log"), maxBytes: 2_000)
        for i in 0..<200 { log.event("e", ["i": i]) }
        let size = (try? FileManager.default.attributesOfItem(atPath: log.url!.path)[.size] as? Int) ?? 0
        XCTAssertLessThanOrEqual(size, 2_200)
        XCTAssertTrue(FileManager.default.fileExists(atPath: log.url!.path + ".1"), "older lines move to .1")
        XCTAssertTrue(log.tail(maxBytes: 300).contains("\"i\":199"))
        XCTAssertTrue(log.tail(maxBytes: 3_000).contains("\"i\":1"), "tail continues into .1")
    }

    func testNilURLIsNoOpAndUnencodableValuesAreStringified() {
        let off = EventLog(url: nil)
        off.event("x", ["a": 1])
        XCTAssertEqual(off.tail(maxBytes: 100), "")
        XCTAssertNil(EventLog.groupFile("x.log", group: ""))
        let log = EventLog(url: dir.appendingPathComponent("d.log"))
        log.event("x", ["date": Date(timeIntervalSince1970: 0), "nested": ["k": 1]])
        XCTAssertTrue(log.tail(maxBytes: 1000).contains("\"nested\":{\"k\":1}"))
        XCTAssertTrue(log.tail(maxBytes: 1000).contains("1970"))
    }

    func testErrorTallyCountsAndKeepsFirstDetails() {
        let t = ErrorTally(maxDetails: 3)
        for _ in 0..<5 { t.record(2, op: "lookup") }   // ENOENT: routine, only counted
        for i in 0..<5 { t.record(5, op: "write\(i)") }
        let s = t.summary()
        XCTAssertEqual((s["counts"] as? [String: Int])?["2"], 5)
        XCTAssertEqual((s["counts"] as? [String: Int])?["5"], 5)
        XCTAssertEqual((s["first"] as? [String])?.count, 3)
        XCTAssertEqual((s["first"] as? [String])?.first, "write0:5", "details skip routine results like ENOENT")
        t.reset()
        XCTAssertTrue(((t.summary()["counts"] as? [String: Int]) ?? [:]).isEmpty)
    }
}
