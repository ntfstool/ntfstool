/* Copyright © 2019-2026 NTFSTool.
 * SPDX-License-Identifier: MIT */
/*
 * ntfstool_bridge.h - inode-number based C API over libntfstool-driver.
 *
 * This is the boundary between the Swift driver (NTFSToolDriver, used by the
 * FSKit module) and libntfstool-driver. Swift imports it as the NTFSToolBridge module
 * (see module.modulemap).
 *
 * Conventions:
 * - Items are addressed by inode number = NTFS MFT record number. The root
 *   directory is NTB_ROOT (MFT record 5). Records below 16 are NTFS metadata
 *   files ($MFT, $LogFile, ...) and are never returned by lookup/readdir.
 * - Every function returns 0 on success or a positive errno value.
 * - Device I/O goes through caller-supplied callbacks (ntfstool_io). The bridge
 *   aligns every request to io->block_size before calling them, and writes are
 *   passed through immediately (no write-back buffering in the bridge).
 * - Not thread safe: callers must serialize all calls on a volume.
 */
#ifndef NTFSTOOL_BRIDGE_H
#define NTFSTOOL_BRIDGE_H

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/** MFT record number of the NTFS root directory. */
#define NTB_ROOT 5

/** Block device backend. FSKit passes an FSBlockDeviceResource; tests use an image file. */
typedef struct ntfstool_io {
    void *ctx;
    /** Read/write exactly [off, off+len); return 0 or a positive errno. off and len are multiples of block_size. */
    int (*read)(void *ctx, void *buf, uint64_t off, uint64_t len);
    int (*write)(void *ctx, const void *buf, uint64_t off, uint64_t len);
    /** Make previous writes durable (may be NULL). */
    int (*sync)(void *ctx);
    uint64_t size;        /**< device size in bytes (a multiple of block_size) */
    uint32_t block_size;  /**< required I/O alignment (e.g. 512 or 4096); 0 or 1 = none */
} ntfstool_io;

/** Opaque mounted volume. */
typedef struct ntfstool_vol ntfstool_vol;

/** Item types reported by getattr/readdir. */
enum { NTB_FILE = 1, NTB_DIR = 2, NTB_SYMLINK = 3 };
/** Selected NTFS file attribute flags reported in ntfstool_attr.flags. */
enum { NTB_F_HIDDEN = 1, NTB_F_READONLY = 2, NTB_F_SYSTEM = 4, NTB_F_COMPRESSED = 8, NTB_F_REPARSE = 16 };

/** Attributes of one item (stat-like). */
typedef struct ntfstool_attr {
    uint64_t ino;
    uint32_t type;   /**< NTB_FILE / NTB_DIR / NTB_SYMLINK */
    uint32_t nlink;
    uint32_t flags;  /**< NTB_F_* */
    uint64_t size;   /**< logical size; for symlinks the target length */
    uint64_t alloc;  /**< bytes allocated on disk */
    struct timespec atime, mtime, ctime, btime;
} ntfstool_attr;

/** Volume capacity, as needed for statfs. */
typedef struct ntfstool_statfs {
    uint32_t cluster_size;
    uint64_t total_clusters;
    uint64_t free_clusters;
    uint64_t total_files;
    uint64_t free_files;
} ntfstool_statfs;

/** Check the boot sector for NTFS. On success *serial (optional) receives the volume serial number. */
int ntfstool_probe(const ntfstool_io *io, uint64_t *serial);

/**
 * Mount a volume. readonly=1 mounts read-only. Returns NULL and sets *err on
 * failure: EINVAL = not NTFS, EPERM = Windows hibernation / Fast Startup (the
 * volume can still be mounted read-only), EIO etc. for other problems.
 * io is copied; the callbacks' ctx must stay valid until ntfstool_umount().
 */
ntfstool_vol *ntfstool_mount(const ntfstool_io *io, int readonly, int *err);
/** Write everything back and free the volume. */
int ntfstool_umount(ntfstool_vol *v);
/** Flush the device (no-op when read-only). */
int ntfstool_sync(ntfstool_vol *v);
/** 1 if the volume is read-only (requested, or forced by libntfstool-driver). */
int ntfstool_is_readonly(ntfstool_vol *v);

/** windows_names: reject names Windows cannot open (forbidden characters, reserved names such as CON,
 *  trailing dot or space) with EINVAL, same as the `windows_names` mount option. Off by default. */
void ntfstool_set_windows_names(ntfstool_vol *v, int on);
int ntfstool_windows_names(ntfstool_vol *v);

/** Volume label as UTF-8 (caller frees). */
char *ntfstool_label(ntfstool_vol *v);
int ntfstool_set_label(ntfstool_vol *v, const char *label);
int ntfstool_statfs_get(ntfstool_vol *v, ntfstool_statfs *out);
uint64_t ntfstool_serial(ntfstool_vol *v);

/** Find `name` in directory `dir`. */
int ntfstool_lookup(ntfstool_vol *v, uint64_t dir, const char *name, uint64_t *ino);
int ntfstool_getattr(ntfstool_vol *v, uint64_t ino, ntfstool_attr *out);

/** Directory listing callback; return non-zero to stop. Excludes ".", "..", DOS short names and metadata files. */
typedef int (*ntfstool_dir_cb)(void *ctx, const char *name, uint64_t ino, uint32_t type);
int ntfstool_readdir(ntfstool_vol *v, uint64_t dir, ntfstool_dir_cb cb, void *ctx);

/** Read file data; *done < len at end of file. */
int ntfstool_read(ntfstool_vol *v, uint64_t ino, uint64_t off, void *buf, size_t len, size_t *done);
/** Write file data (extends the file as needed). */
int ntfstool_write(ntfstool_vol *v, uint64_t ino, uint64_t off, const void *buf, size_t len, size_t *done);
int ntfstool_truncate(ntfstool_vol *v, uint64_t ino, uint64_t size);
/** On file close: write the inode's metadata (size, times) back and flush the device. */
int ntfstool_flush(ntfstool_vol *v, uint64_t ino);

/** Create a file (NTB_FILE) or directory (NTB_DIR). EEXIST if the name is taken. */
int ntfstool_create(ntfstool_vol *v, uint64_t dir, const char *name, uint32_t type, uint64_t *ino);
/** Create an (Interix-style) symbolic link. */
int ntfstool_symlink(ntfstool_vol *v, uint64_t dir, const char *name, const char *target, uint64_t *ino);
/** Symlink target as UTF-8 (caller frees). EINVAL if not a link. */
int ntfstool_readlink(ntfstool_vol *v, uint64_t ino, char **target);
/** Remove a file or an empty directory (ENOTEMPTY otherwise). */
int ntfstool_unlink(ntfstool_vol *v, uint64_t dir, const char *name);
/** Rename/move. An existing target is replaced if it is a file, or an empty directory. */
int ntfstool_rename(ntfstool_vol *v, uint64_t sdir, const char *sname, uint64_t ddir, const char *dname);
/** Set times; NULL leaves a time unchanged. */
int ntfstool_set_times(ntfstool_vol *v, uint64_t ino, const struct timespec *atime, const struct timespec *mtime,
                       const struct timespec *ctime, const struct timespec *btime);
/** Set or clear the NTFS hidden attribute (maps to UF_HIDDEN). */
int ntfstool_set_hidden(ntfstool_vol *v, uint64_t ino, int hidden);

/**
 * Extended attributes are stored as NTFS named data streams, using the xattr
 * name verbatim as the stream name (the usual mapping on macOS with
 * user_xattr / streams_interface=openxattr), so they are interoperable.
 * Missing attribute -> ENOATTR; setting one on a symlink -> EPERM.
 */
enum { NTB_XATTR_CREATE = 1, NTB_XATTR_REPLACE = 2 };
typedef int (*ntfstool_name_cb)(void *ctx, const char *name);
int ntfstool_listxattr(ntfstool_vol *v, uint64_t ino, ntfstool_name_cb cb, void *ctx);
/** *value is malloc'ed and freed by the caller (may be non-NULL even when *len is 0). */
int ntfstool_getxattr(ntfstool_vol *v, uint64_t ino, const char *name, void **value, size_t *len);
int ntfstool_setxattr(ntfstool_vol *v, uint64_t ino, const char *name, const void *value, size_t len, int flags);
int ntfstool_removexattr(ntfstool_vol *v, uint64_t ino, const char *name);

#ifdef __cplusplus
}
#endif
#endif /* NTFSTOOL_BRIDGE_H */
