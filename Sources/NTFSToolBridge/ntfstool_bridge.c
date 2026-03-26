/* Copyright © 2019-2026 NTFSTool.
 * SPDX-License-Identifier: MIT */
/*
 * ntfstool_bridge.c - an inode-number based C API on top of libntfstool-driver.
 *
 * Architecture (top to bottom):
 *
 *   FSKit module (FSKitModule/NTFSToolFS)       Swift, talks to the macOS FSKit framework
 *        |
 *   NTFSToolDriver (NTFSVolume, BlockDevice)    Swift wrapper: locking, Swift types, errors
 *        |
 *   ntfstool_bridge (this file)                 C: "operate on MFT record N" API, errno results
 *        |
 *   libntfstool-driver (Vendor/NTFSToolNTFS.xcframework) the NTFS implementation (prebuilt)
 *        |
 *   ntfstool_io callbacks                       raw block device (FSKit resource or image file)
 *
 * libntfstool-driver was designed around path names (its FUSE front end resolves a
 * path on every call). FSKit, like a kernel VFS, works with item identifiers.
 * This bridge uses the NTFS MFT record number as the item identifier, so every
 * public function takes an inode number ("ino") instead of a path.
 *
 * Inode discipline (important for correctness):
 *   Every operation opens the ntfs_inode(s) it needs, does its work and closes
 *   them again before returning. libntfstool-driver keeps an in-memory copy of an MFT
 *   record per open ntfs_inode and writes that copy back on close; if the same
 *   record were open twice, closing the stale copy would overwrite changes made
 *   through the other one (for example a directory index update). Opening and
 *   closing per call guarantees there is never more than one live ntfs_inode
 *   for any MFT record. the usual FUSE NTFS drivers follow the same rule.
 *
 * Device layer:
 *   libntfstool-driver performs I/O through a struct ntfsdrv_device_operations table. We
 *   implement that table on top of the ntfstool_io callbacks supplied by the
 *   caller. libntfstool-driver issues reads and writes of arbitrary offset/length (for
 *   example 1 KB MFT records), while FSKit block devices only accept I/O that
 *   is a multiple of the device block size. io_read()/io_write() therefore
 *   widen each request to whole blocks (read-modify-write for partial blocks).
 *   All writes go straight to the device (write-through); nothing is buffered
 *   in this layer, so a successful write is on the device when it returns.
 *
 * Errors:
 *   libntfstool-driver reports failure through errno. Every public function here
 *   returns 0 on success or a positive errno value, which the Swift layer turns
 *   into NTFSError and the FSKit module into an NSError for the kernel.
 *
 * Threading:
 *   libntfstool-driver is not thread safe. Callers must serialize all calls on a
 *   volume (NTFSVolume does this with a single lock).
 */
#include "config.h"
#include "ntfstool_bridge.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/disk.h>

#include "ntfs_core_types.h"
#include "ntfs_fs_layout.h"
#include "ntfs_block_dev.h"
#include "ntfs_vol_context.h"
#include "ntfs_vnode_core.h"
#include "ntfs_attr_core.h"
#include "ntfs_dir_catalog.h"
#include "ntfs_unicode_trans.h"
#include "ntfs_time_utils.h"
#include "ntfs_reparse_tag.h"
#include "ntfs_logger.h"
#include "ntfs_util_misc.h"
#include "ntfs_cache_engine.h"

/* A mounted volume as seen by callers of this API. */
struct ntfstool_vol {
    ntfsdrv_volume *vol;    /* the libntfstool-driver volume */
    ntfstool_io io;      /* device callbacks (copy) */
    int readonly;        /* mounted read-only on request */
    int win_names;       /* reject names Windows cannot open ("windows_names") */
    uint64_t serial;     /* NTFS volume serial number from the boot sector */
};

/* ===================================================================== */
/* Device layer: ntfsdrv_device_operations on top of ntfstool_io callbacks */
/* ===================================================================== */

/* Per-device private data stored in ntfsdrv_device.d_private. */
typedef struct {
    ntfstool_io io;
    s64 pos;      /* current position for the seek/read/write style calls */
    int readonly;
} devpriv;

static devpriv *P(struct ntfsdrv_device *dev) { return (devpriv *)dev->d_private; }

/* Effective alignment unit of the backend (1 = no alignment requirement). */
static uint32_t bsz(const ntfstool_io *io) { return io->block_size > 1 ? io->block_size : 1; }

/*
 * Read [off, off+len) from the device. The range may be unaligned; it is
 * widened to whole blocks [a, e) and the requested bytes are copied out of a
 * temporary buffer. Reads past the end of the device are clipped.
 */
static int io_read(const ntfstool_io *io, void *buf, uint64_t off, uint64_t len)
{
    uint32_t b = bsz(io);
    if (len == 0) return 0;
    if (off + len > io->size) {
        if (off >= io->size) return EIO;
        len = io->size - off;
    }
    if (b == 1 || (off % b == 0 && len % b == 0))
        return io->read(io->ctx, buf, off, len);
    uint64_t a = off - off % b;              /* round start down to a block */
    uint64_t end = off + len;
    uint64_t e = (end + b - 1) / b * b;      /* round end up to a block */
    if (e > io->size) e = io->size;
    uint8_t *tmp = malloc(e - a);
    if (!tmp) return ENOMEM;
    int r = io->read(io->ctx, tmp, a, e - a);
    if (!r) memcpy(buf, tmp + (off - a), len);
    free(tmp);
    return r;
}

/*
 * Write [off, off+len) to the device. Unaligned ranges are handled with
 * read-modify-write: the first and last partial blocks are read, the new
 * bytes are merged in, and whole blocks are written back.
 */
static int io_write(const ntfstool_io *io, const void *buf, uint64_t off, uint64_t len)
{
    uint32_t b = bsz(io);
    if (len == 0) return 0;
    if (off + len > io->size) return ENOSPC;
    if (b == 1 || (off % b == 0 && len % b == 0))
        return io->write(io->ctx, buf, off, len);
    uint64_t a = off - off % b;
    uint64_t end = off + len;
    uint64_t e = (end + b - 1) / b * b; /* io->size is a multiple of b, so e stays in range */
    uint8_t *tmp = malloc(e - a);
    if (!tmp) return ENOMEM;
    int r = 0;
    /* Partial first block: fetch its current contents. */
    if (off != a) r = io->read(io->ctx, tmp, a, b);
    /* Partial last block: fetch it too (may be the same block as the first; reading twice is harmless). */
    if (!r && end != e) r = io->read(io->ctx, tmp + (e - b - a), e - b, b);
    if (!r) {
        memcpy(tmp + (off - a), buf, len);
        r = io->write(io->ctx, tmp, a, e - a);
    }
    free(tmp);
    return r;
}

static int d_open(struct ntfsdrv_device *dev, int flags)
{
    if (NDevOpen(dev)) { errno = EBUSY; return -1; }
    if ((flags & O_RDWR) != O_RDWR || P(dev)->readonly) NDevSetReadOnly(dev);
    NDevSetBlock(dev);
    NDevSetOpen(dev);
    P(dev)->pos = 0;
    return 0;
}

/* Called by ntfsdrv_umount(): make sure everything written reaches stable storage. */
static int d_close(struct ntfsdrv_device *dev)
{
    if (!NDevOpen(dev)) { errno = EBADF; return -1; }
    int r = 0;
    if (NDevDirty(dev) && P(dev)->io.sync) r = P(dev)->io.sync(P(dev)->io.ctx);
    NDevClearOpen(dev);
    if (r) { errno = r; return -1; }
    return 0;
}

static s64 d_seek(struct ntfsdrv_device *dev, s64 offset, int whence)
{
    devpriv *p = P(dev);
    switch (whence) {
    case SEEK_SET: p->pos = offset; break;
    case SEEK_CUR: p->pos += offset; break;
    case SEEK_END: p->pos = (s64)p->io.size + offset; break;
    default: errno = EINVAL; return -1;
    }
    return p->pos;
}

/* libntfstool-driver convention: return the byte count, or -1 with errno set. */
static s64 d_pread(struct ntfsdrv_device *dev, void *buf, s64 count, s64 offset)
{
    devpriv *p = P(dev);
    if (count <= 0) return 0;
    if ((uint64_t)offset >= p->io.size) return 0;
    if ((uint64_t)(offset + count) > p->io.size) count = (s64)p->io.size - offset;
    int r = io_read(&p->io, buf, (uint64_t)offset, (uint64_t)count);
    if (r) { errno = r; return -1; }
    return count;
}

static s64 d_pwrite(struct ntfsdrv_device *dev, const void *buf, s64 count, s64 offset)
{
    devpriv *p = P(dev);
    if (NDevReadOnly(dev)) { errno = EROFS; return -1; }
    if (count <= 0) return 0;
    int r = io_write(&p->io, buf, (uint64_t)offset, (uint64_t)count);
    if (r) { errno = r; return -1; }
    NDevSetDirty(dev); /* remember that a sync is needed */
    return count;
}

static s64 d_read(struct ntfsdrv_device *dev, void *buf, s64 count)
{
    s64 r = d_pread(dev, buf, count, P(dev)->pos);
    if (r > 0) P(dev)->pos += r;
    return r;
}

static s64 d_write(struct ntfsdrv_device *dev, const void *buf, s64 count)
{
    s64 r = d_pwrite(dev, buf, count, P(dev)->pos);
    if (r > 0) P(dev)->pos += r;
    return r;
}

/* Flush the backend (fsync for an image file) if anything was written since the last sync. */
static int d_sync(struct ntfsdrv_device *dev)
{
    devpriv *p = P(dev);
    if (!NDevReadOnly(dev) && NDevDirty(dev)) {
        int r = p->io.sync ? p->io.sync(p->io.ctx) : 0;
        if (r) { errno = r; return -1; }
        NDevClearDirty(dev);
    }
    return 0;
}

static int d_stat(struct ntfsdrv_device *dev, struct stat *buf)
{
    memset(buf, 0, sizeof(*buf));
    buf->st_mode = S_IFBLK;
    buf->st_size = (off_t)P(dev)->io.size;
    buf->st_blksize = (blksize_t)bsz(&P(dev)->io);
    return 0;
}

/* libntfstool-driver asks for the sector size / count of block devices through these ioctls. */
static int d_ioctl(struct ntfsdrv_device *dev, unsigned long request, void *argp)
{
    devpriv *p = P(dev);
    uint32_t b = p->io.block_size >= 512 ? p->io.block_size : 512;
    switch (request) {
    case DKIOCGETBLOCKSIZE: *(uint32_t *)argp = b; return 0;
    case DKIOCGETBLOCKCOUNT: *(uint64_t *)argp = p->io.size / b; return 0;
    default: errno = EOPNOTSUPP; return -1;
    }
}

static struct ntfsdrv_device_operations ntfstool_dev_ops = {
    .open = d_open, .close = d_close, .seek = d_seek, .read = d_read, .write = d_write,
    .pread = d_pread, .pwrite = d_pwrite, .sync = d_sync, .stat = d_stat, .ioctl = d_ioctl,
};
