// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// NTFSToolDiskInfo.swift
//
// Part of NTFSToolApp. Reads the list of disks from DiskArbitration.
//
// Why DiskArbitration and not `diskutil`: inside the App Sandbox the app cannot
// usefully run `diskutil` (it does not see the disks), but it can open a
// DiskArbitration session, receive "disk appeared / disappeared / changed"
// callbacks, and read each disk's description dictionary. That dictionary,
// plus a `statfs` of the mount point, is all the app needs to build the list.
//
// `NTFSToolDiskInfo.volume` is a pure function (description dictionary in,
// volume out) so it can be unit-tested with captured dictionaries.

import DiskArbitration
import Foundation
import NTFSToolCore

/// Live facts about a mount point, read with `statfs`.
public struct NTFSToolMountStat: Equatable {
    /// File system type name, e.g. `ntfs` or our module's short name.
    public var fsType: String
    /// True if mounted with MNT_RDONLY.
    public var readOnly: Bool
    /// Total and available bytes.
    public var total: Int64
    public var free: Int64
}

/// Converts DiskArbitration descriptions into `NTFSToolVolume`s.
public enum NTFSToolDiskInfo {
    /// Builds a volume from a `DADiskCopyDescription` dictionary, or returns nil
    /// when the entry should not be listed (whole disks / partition maps, the
    /// root volume, anything that is not NTFS).
    ///
    /// - Parameter stat: looks up live mount facts for a mount point (injected
    ///   so tests can fake it; the app passes `liveStat`).
    public static func volume(_ d: [String: Any], stat: (String) -> NTFSToolMountStat?) -> NTFSToolVolume? {
        // Only "leaf" media (partitions or unpartitioned disks) carry a file system.
        guard let bsd = d[kDADiskDescriptionMediaBSDNameKey as String] as? String, !bsd.isEmpty,
              (d[kDADiskDescriptionMediaLeafKey as String] as? Bool) == true else { return nil }
        let content = d[kDADiskDescriptionMediaContentKey as String] as? String ?? ""
        let kind = d[kDADiskDescriptionVolumeKindKey as String] as? String
        var path = (d[kDADiskDescriptionVolumePathKey as String] as? URL)?.path ?? ""
        if path.count > 1, path.hasSuffix("/") { path.removeLast() }
        guard path != "/" else { return nil }
        // statfs is the most reliable source for the type and read-only flag of a mount.
        let st = path.isEmpty ? nil : stat(path)
        let fsType = st?.fsType ?? kind ?? ""
        // An unmounted volume has no type yet; trust the MBR partition type then.
        // (A GPT "Microsoft Basic Data" partition could be NTFS or exFAT, so it is
        // only listed once mounted.)
        let isNTFS = NTFSToolDriverNames.fskitNTFSNames.contains(fsType) || fsType.lowercased().contains("ntfs")
            || (path.isEmpty && kind == nil && content == "Windows_NTFS")
        guard isNTFS else { return nil }
        var proto = d[kDADiskDescriptionDeviceProtocolKey as String] as? String ?? ""
        if proto == "Virtual Interface" { proto = "Disk Image" }   // how mounted .dmg / .img files report
        // The UUID is a CFUUID object, not a string.
        let uuid = (d[kDADiskDescriptionVolumeUUIDKey as String]).flatMap { v -> String? in
            guard CFGetTypeID(v as CFTypeRef) == CFUUIDGetTypeID() else { return nil }
            return CFUUIDCreateString(nil, (v as! CFUUID)) as String?
        } ?? ""
        let name = (d[kDADiskDescriptionVolumeNameKey as String] as? String).flatMap { $0.isEmpty ? nil : $0 }
            ?? d[kDADiskDescriptionMediaNameKey as String] as? String ?? ""
        let v = DiskVolume(id: bsd, name: name, fileSystemName: "NTFS", fileSystemType: fsType.isEmpty ? "ntfs" : fsType, content: content,
                           mountPoint: path, totalSize: (d[kDADiskDescriptionMediaSizeKey as String] as? NSNumber)?.int64Value ?? st?.total ?? 0,
                           freeSpace: st?.free ?? 0, isWritable: st.map { !$0.readOnly } ?? false, isNTFS: true,
                           isInternal: (d[kDADiskDescriptionDeviceInternalKey as String] as? Bool) ?? false, busProtocol: proto,
                           mediaName: d[kDADiskDescriptionDeviceModelKey as String] as? String ?? "", volumeUUID: uuid,
                           removable: (d[kDADiskDescriptionMediaRemovableKey as String] as? Bool) ?? false)
        let ours = NTFSToolDriverNames.fskitNTFSNames.contains(fsType) ? fsType : nil
        return NTFSToolVolume(volume: v, state: NTFSToolStateClassifier.state(of: v, fskitType: ours))
    }

    /// Real `statfs` of a mount point, limited to 2 seconds: the mount point of
    /// a drive that was pulled out without ejecting can block indefinitely.
    public static func liveStat(_ path: String) -> NTFSToolMountStat? {
        Timed.run(timeout: 2) { () -> NTFSToolMountStat? in
            var s = statfs()
            guard statfs(path, &s) == 0 else { return nil }
            // f_fstypename is a fixed-size C char array; read it as a C string.
            let type = withUnsafeBytes(of: s.f_fstypename) { String(cString: $0.bindMemory(to: CChar.self).baseAddress!) }
            let bs = Int64(s.f_bsize)
            return NTFSToolMountStat(fsType: type, readOnly: (s.f_flags & UInt32(MNT_RDONLY)) != 0, total: Int64(s.f_blocks) * bs, free: Int64(s.f_bavail) * bs)
        } ?? nil
    }
}

/// Keeps a live set of disks from DiskArbitration callbacks and calls
/// `onChange` (on the main thread) whenever a disk appears, disappears or
/// changes, e.g. when it gets mounted.
public final class DAInventory {
    let session: DASession?
    let queue = DispatchQueue(label: "ntfstool.inventory")
    var disks: [String: DADisk] = [:]
    let lock = NSLock()
    /// Called on the main thread after disks changed.
    public var onChange: () -> Void = {}

    public init() {
        session = DASessionCreate(kCFAllocatorDefault)
        guard let s = session else { return }
        // The C callbacks cannot capture Swift context, so `self` is passed as an
        // unretained pointer (the inventory lives as long as the app).
        let ctx = Unmanaged.passUnretained(self).toOpaque()
        DARegisterDiskAppearedCallback(s, nil, { d, ctx in Unmanaged<DAInventory>.fromOpaque(ctx!).takeUnretainedValue().set(d, present: true) }, ctx)
        DARegisterDiskDisappearedCallback(s, nil, { d, ctx in Unmanaged<DAInventory>.fromOpaque(ctx!).takeUnretainedValue().set(d, present: false) }, ctx)
        DARegisterDiskDescriptionChangedCallback(s, nil, nil, { d, _, ctx in Unmanaged<DAInventory>.fromOpaque(ctx!).takeUnretainedValue().set(d, present: true) }, ctx)
        DASessionSetDispatchQueue(s, queue)
        // Right after registration DiskArbitration reports every existing disk as
        // "appeared". Give it a moment so the first `volumes()` call is complete.
        queue.sync {}
        Thread.sleep(forTimeInterval: 0.2)
    }

    func set(_ d: DADisk, present: Bool) {
        guard let c = DADiskGetBSDName(d) else { return }
        let bsd = String(cString: c)
        lock.lock()
        disks[bsd] = present ? d : nil
        lock.unlock()
        fire()
    }

    var pending = false
    /// Coalesces bursts of callbacks (plugging in one drive produces several)
    /// into a single `onChange` half a second later.
    func fire() {
        lock.lock()
        let already = pending
        pending = true
        lock.unlock()
        if already { return }
        DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) { [weak self] in
            guard let self = self else { return }
            self.lock.lock(); self.pending = false; self.lock.unlock()
            self.onChange()
        }
    }

    /// Current NTFS volumes, sorted by name. Each call re-reads the descriptions,
    /// so mount state is always fresh.
    public func volumes() -> [NTFSToolVolume] {
        lock.lock()
        let list = Array(disks.values)
        lock.unlock()
        return list.compactMap { DADiskCopyDescription($0) as? [String: Any] }
            .compactMap { NTFSToolDiskInfo.volume($0, stat: NTFSToolDiskInfo.liveStat) }
            .sorted { $0.volume.displayName.localizedStandardCompare($1.volume.displayName) == .orderedAscending }
    }
}
