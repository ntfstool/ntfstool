// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolEventLog.swift
//
// Part of NTFSToolCore, but intentionally self-contained (Foundation only, no
// other NTFSToolCore types): scripts/build-fskit.sh compiles this single file
// directly into the FSKit driver extension, which is built outside SwiftPM.
//
// The driver uses it to keep a small, purely local log of what it did (probe,
// load, errors, unload) so that problems can be investigated on the Mac itself.
// Nothing is ever sent anywhere; the file just sits on disk and is rotated when
// it grows too large.

import Foundation

/// Append-only structured log: one JSON object per line, with the time (`t`),
/// event name (`ev`), process id (`pid`) and any extra fields.
///
/// When the file exceeds `maxBytes` it is renamed to `<file>.1` (replacing any
/// previous `.1`) and a new file is started, so at most about `2 * maxBytes`
/// are kept. A log created with `url == nil` silently ignores every call,
/// which lets callers log unconditionally even when no location is configured.
public final class EventLog {
    /// Location of the log file, or nil when logging is disabled.
    public let url: URL?
    let maxBytes: Int
    let now: () -> Date
    let lock = NSLock()
    static let iso: ISO8601DateFormatter = {
        let f = ISO8601DateFormatter()
        f.formatOptions = [.withInternetDateTime]
        return f
    }()

    /// Creates a log writing to `url` (its directory is created if needed).
    /// - Parameters:
    ///   - maxBytes: size at which the file is rotated.
    ///   - now: clock used for timestamps (injectable for tests).
    public init(url: URL?, maxBytes: Int = 1_000_000, now: @escaping () -> Date = Date.init) {
        self.url = url
        self.maxBytes = maxBytes
        self.now = now
        if let u = url { try? FileManager.default.createDirectory(at: u.deletingLastPathComponent(), withIntermediateDirectories: true) }
    }

    /// Returns `Library/Logs/<name>` inside a shared App Group container, or nil
    /// when `group` is empty or the container is not available to this process
    /// (sandboxed processes only see App Groups listed in their entitlements).
    public static func groupFile(_ name: String, group: String) -> URL? {
        group.isEmpty ? nil : FileManager.default.containerURL(forSecurityApplicationGroupIdentifier: group)?
            .appendingPathComponent("Library/Logs/\(name)")
    }

    /// Converts an arbitrary value into something `JSONSerialization` accepts:
    /// dates become ISO 8601 strings, collections are converted recursively and
    /// unknown types fall back to their `description`.
    public static func clean(_ v: Any) -> Any {
        switch v {
        case let d as Date: return iso.string(from: d)
        case let a as [Any]: return a.map(clean)
        case let m as [String: Any]: return m.mapValues(clean)
        case is String, is Bool, is Int, is Int32, is Int64, is UInt64, is UInt32, is Double, is Float: return v
        case let n as NSNumber: return n
        default: return String(describing: v)
        }
    }

    /// Appends one event line. Safe to call from any thread.
    public func event(_ name: String, _ fields: [String: Any] = [:]) {
        guard let url = url else { return }
        var row = fields.mapValues(EventLog.clean)
        row["t"] = EventLog.iso.string(from: now())
        row["ev"] = name
        row["pid"] = Int(ProcessInfo.processInfo.processIdentifier)
        guard JSONSerialization.isValidJSONObject(row),
              var data = try? JSONSerialization.data(withJSONObject: row, options: [.sortedKeys]) else { return }
        data.append(0x0A)
        lock.lock(); defer { lock.unlock() }
        let fm = FileManager.default
        // Rotate before the write that would push the file over the limit.
        if let size = (try? fm.attributesOfItem(atPath: url.path)[.size] as? Int), size + data.count > maxBytes {
            let old = URL(fileURLWithPath: url.path + ".1")
            try? fm.removeItem(at: old)
            try? fm.moveItem(at: url, to: old)
        }
        if let h = try? FileHandle(forWritingTo: url) {
            h.seekToEndOfFile()
            h.write(data)
            try? h.close()
        } else {
            try? data.write(to: url)   // first write: the file does not exist yet
        }
    }

    /// Returns roughly the last `maxBytes` of the log (including the rotated
    /// `.1` file when the current one is shorter), trimmed to whole lines.
    public func tail(maxBytes: Int) -> String {
        guard let url = url else { return "" }
        lock.lock(); defer { lock.unlock() }
        let cur = (try? Data(contentsOf: url)) ?? Data()
        var all = cur
        if cur.count < maxBytes, let old = try? Data(contentsOf: URL(fileURLWithPath: url.path + ".1")) { all = old + cur }
        var slice = all.count > maxBytes ? all.suffix(maxBytes) : all
        // Drop the partial first line left over from cutting in the middle.
        if all.count > maxBytes, let nl = slice.firstIndex(of: 0x0A) { slice = slice[(nl + 1)...] }
        return String(decoding: slice, as: UTF8.self)
    }
}

/// Counts file system errors by errno and remembers the first few unusual ones.
///
/// Routine results such as "not found" or "already exists" happen constantly
/// during normal Finder use, so they are only counted; other errors are also
/// recorded as `"<operation>:<errno>"` strings (up to `maxDetails`).
public final class ErrorTally {
    let maxDetails: Int
    var counts: [Int32: Int] = [:]
    var first: [String] = []
    let lock = NSLock()
    /// Errors that are expected in normal operation.
    static let routine: Set<Int32> = [ENOENT, EEXIST, ENOATTR, ENOTEMPTY]

    public init(maxDetails: Int = 20) { self.maxDetails = maxDetails }

    /// Records one error `code` returned by operation `op`. Thread-safe.
    public func record(_ code: Int32, op: String) {
        lock.lock(); defer { lock.unlock() }
        counts[code, default: 0] += 1
        if !ErrorTally.routine.contains(code), first.count < maxDetails { first.append("\(op):\(code)") }
    }

    /// Returns `["counts": [errno: count], "first": [details]]`, ready for `EventLog.event`.
    public func summary() -> [String: Any] {
        lock.lock(); defer { lock.unlock() }
        return ["counts": Dictionary(uniqueKeysWithValues: counts.map { (String($0.key), $0.value) }), "first": first]
    }

    /// Clears all counters (e.g. when a new volume is loaded).
    public func reset() {
        lock.lock(); defer { lock.unlock() }
        counts = [:]
        first = []
    }
}
