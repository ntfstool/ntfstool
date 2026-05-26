// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolFSKitSupport.swift - FSKit-independent helpers for the FSKit module.
//
// The FSKit module (FSKitModule/NTFSToolFS/NTFSToolFS.swift) can only be built
// against the macOS 15.4+ SDK and only runs inside the FSKit extension host, so
// it is hard to unit-test. Every decision it makes that does not need FSKit
// types (ID mapping, option parsing, check policy, permissions) lives here
// instead, where NTFSToolDriverTests can cover it.

import Foundation

/// Pure logic used by the FSKit module.
public enum NTFSToolFSKit {
    /// Default FSKit short name of the module (`mount -F -t ntfstooloss ...`; also
    /// the type shown in the mount table). It must differ from Apple's built-in
    /// read-only NTFS module ("ntfs") and from other NTFS FSKit modules that may be
    /// installed side by side. The value actually used comes from the module's
    /// Info.plist (`FSShortName`, set by scripts/build-fskit.sh); this constant
    /// is only the fallback.
    public static let shortName = "ntfstooloss"

    /// The short name declared in the module's Info.plist
    /// (`EXAppExtensionAttributes.FSShortName`), or `shortName` if absent.
    public static func shortName(info: [String: Any]?) -> String {
        ((info?["EXAppExtensionAttributes"] as? [String: Any])?["FSShortName"] as? String).flatMap { $0.isEmpty ? nil : $0 } ?? shortName
    }

    /// Mode of a file system check request (fsck_fskit, run by DiskArbitration
    /// before auto-mounting): `-q` quick, `-n` verify only, `-y` repair.
    public enum CheckMode: Equatable {
        case quick, verify, repair
        public init(_ args: [String]) throws {
            var mode = CheckMode.quick
            for a in args {
                switch a {
                case "-q": break
                case "-n": if mode != .repair { mode = .verify }
                case "-y": mode = .repair
                default: throw NTFSError(EINVAL)
                }
            }
            self = mode
        }
    }

    /// Result of a check: nil = passed, otherwise an errno.
    ///
    /// DiskArbitration only auto-mounts a volume whose quick check succeeds. We
    /// treat "libntfstool-driver could open the volume" as a passing quick/verify check.
    /// Repair is not implemented (use Windows chkdsk), so it reports ENOTSUP.
    public static func checkError(mode: CheckMode, volumeLoaded: Bool) -> Int32? {
        if mode == .repair { return ENOTSUP }
        return volumeLoaded ? nil : ENXIO
    }

    /// Open a volume, falling back to read-only when a writable open fails with
    /// EPERM (Windows hibernation / Fast Startup left the volume in use) or EROFS
    /// (write-protected media). Writing to a hibernated volume would corrupt the
    /// Windows session, so read-only is the safe way to still give access. Other
    /// errors (e.g. EIO for a damaged volume) are reported as they are.
    public static func openWithReadOnlyFallback<T>(readOnly: Bool, _ open: (Bool) throws -> T) throws -> (value: T, fellBack: Bool) {
        do {
            return (try open(readOnly), false)
        } catch let e as NTFSError where !readOnly && (e.code == EPERM || e.code == EROFS) {
            return (try open(true), true)
        }
    }

    /// Inode number -> FSKit item identifier.
    ///
    /// FSKit requires the root directory to have identifier 2
    /// (`FSItem.Identifier.rootDirectory`). In NTFS the root is MFT record 5,
    /// while MFT record 2 is `$LogFile`, a metadata file that is never exposed.
    /// Swapping 2 and 5 therefore cannot collide with any visible item; every
    /// other record number is used unchanged as the item identifier.
    public static func itemID(forIno ino: UInt64) -> UInt64 { ino == NTFSVolume.rootIno ? 2 : ino }
    /// FSKit item identifier -> inode number (inverse of `itemID(forIno:)`).
    public static func ino(forItemID id: UInt64) -> UInt64 { id == 2 ? NTFSVolume.rootIno : id }

    /// POSIX mode for an item. NTFS has no POSIX permissions (ownership is
    /// ignored on this volume), so everything is readable and writable by all,
    /// like the usual default mapping.
    public static func mode(for type: NTFSItemType) -> UInt32 {
        switch type {
        case .directory: return UInt32(S_IFDIR) | 0o777
        case .symlink: return UInt32(S_IFLNK) | 0o777
        case .file: return UInt32(S_IFREG) | 0o666
        }
    }

    /// A stable volume UUID derived from the NTFS serial number, so the same
    /// disk gets the same identity on every mount. Layout: an 8-byte tag
    /// followed by the 8 serial bytes, with RFC 4122 version/variant bits set.
    public static func volumeUUID(serial: UInt64) -> UUID {
        var b = [UInt8](repeating: 0, count: 16)
        let tag: [UInt8] = Array("NTFSTOOL".utf8)
        for i in 0..<8 { b[i] = tag[i]; b[8 + i] = UInt8(truncatingIfNeeded: serial >> (8 * UInt64(i))) }
        b[6] = (b[6] & 0x0F) | 0x50 // version 5 style
        b[8] = (b[8] & 0x3F) | 0x80 // RFC 4122 variant
        return UUID(uuid: (b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9], b[10], b[11], b[12], b[13], b[14], b[15]))
    }

    /// Mount options understood by the module (from `FSTaskOptions.taskOptions`).
    public struct MountOptions: Equatable {
        public var readOnly = false
        /// `-o windows_names`: reject file names Windows cannot open.
        public var windowsNames = false
        public init() {}
        /// Parse task options such as `["-o", "rdonly,windows_names"]`, `["-r"]` or `["-ordonly"]`.
        public init(_ args: [String]) {
            var i = 0
            while i < args.count {
                let a = args[i]
                if a == "-r" || a == "--rdonly" { readOnly = true }
                if a == "-o", i + 1 < args.count {
                    for o in args[i + 1].split(separator: ",") { apply(o) }
                    i += 1
                } else if a.hasPrefix("-o") {
                    for o in a.dropFirst(2).split(separator: ",") { apply(o) }
                } else if a == "ro" || a == "rdonly" {
                    readOnly = true
                }
                i += 1
            }
        }

        private mutating func apply(_ o: Substring) {
            if o == "ro" || o == "rdonly" { readOnly = true }
            if o == "windows_names" { windowsNames = true }
        }
    }

    /// A directory listing captured when enumeration starts (cookie 0).
    ///
    /// FSKit enumerates directories in several calls, resuming from a cookie.
    /// NTFS index positions are not stable across modifications, so the module
    /// takes a snapshot and uses the entry index as the cookie. The verifier
    /// changes whenever the volume modifies a directory; FSKit then sees a
    /// verifier mismatch and restarts enumeration instead of reading stale data.
    public final class DirectorySnapshot {
        public let entries: [NTFSDirEntry]
        public let verifier: UInt64
        public init(entries: [NTFSDirEntry], verifier: UInt64) {
            self.entries = entries
            self.verifier = verifier
        }

        /// Entries starting at `cookie` (an index into `entries`).
        public func page(from cookie: UInt64) -> ArraySlice<NTFSDirEntry> {
            let start = Int(min(cookie, UInt64(entries.count)))
            return entries[start...]
        }

        /// Cookie that resumes after entry `index`.
        public func cookie(after index: Int) -> UInt64 { UInt64(index + 1) }
    }

    /// Short machine-readable reason for an errno (used in diagnostic logs).
    public static func describe(_ code: Int32) -> String {
        switch code {
        case EPERM: return "hibernated_or_fast_startup"
        case EROFS: return "read_only"
        case EINVAL: return "not_ntfs"
        case EIO: return "io_error"
        case ENOSPC: return "no_space"
        default: return "errno_\(code)"
        }
    }
}
