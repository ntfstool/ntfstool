// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolTimed.swift
//
// Part of NTFSToolCore: pure logic with no AppKit dependency.

import Foundation

/// Runs a block with a deadline.
///
/// Some system calls can block for a very long time; the classic example is
/// `statfs` on the mount point of a drive that was unplugged without ejecting.
/// `Timed.run` executes the block on a background queue and simply stops
/// waiting after `timeout`, so the caller (and the UI) never hangs. The block
/// itself keeps running in the background until the system call returns.
public enum Timed {
    /// Returns the block's result, or nil if it did not finish within `timeout` seconds.
    public static func run<T>(timeout: TimeInterval, qos: DispatchQoS.QoSClass = .utility, _ block: @escaping () -> T) -> T? {
        let sem = DispatchSemaphore(value: 0)
        let box = Box<T>()
        DispatchQueue.global(qos: qos).async {
            box.value = block()
            sem.signal()
        }
        return sem.wait(timeout: .now() + timeout) == .timedOut ? nil : box.value
    }

    /// Thread-safe holder for the result (written on the worker, read by the caller).
    final class Box<T> {
        private let lock = NSLock()
        private var _value: T?
        var value: T? {
            get { lock.lock(); defer { lock.unlock() }; return _value }
            set { lock.lock(); _value = newValue; lock.unlock() }
        }
    }
}
