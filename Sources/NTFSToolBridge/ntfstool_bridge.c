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

/* ===================================================================== */
/* Helpers                                                               */
/* ===================================================================== */

/* Map "libntfstool-driver failed" to an errno, falling back when it did not set one. */
static int err_or(int fallback) { return errno ? errno : fallback; }

/* Read the boot sector, check the NTFS signature and return the volume serial number. */
static uint64_t read_serial(const ntfstool_io *io, int *is_ntfs)
{
    uint8_t bs[512];
    *is_ntfs = 0;
    if (io->size < 512 || io_read(io, bs, 0, 512)) return 0;
    if (memcmp(bs + 3, "NTFS    ", 8) != 0) return 0;      /* OEM ID */
    if (bs[510] != 0x55 || bs[511] != 0xAA) return 0;      /* boot sector signature */
    *is_ntfs = 1;
    uint64_t s = 0;
    for (int i = 7; i >= 0; i--) s = (s << 8) | bs[0x48 + i]; /* little-endian serial at 0x48 */
    return s;
}

/* Open MFT record `ino`; on failure errno is always set (ENOENT if libntfstool-driver left it 0). */
static ntfsdrv_inode *open_ino(ntfstool_vol *v, uint64_t ino)
{
    errno = 0;
    ntfsdrv_inode *ni = ntfsdrv_inode_open(v->vol, (MFT_REF)ino);
    if (!ni && !errno) errno = ENOENT;
    return ni;
}

/* Validate a file name and convert it to NTFS UTF-16. Returns the length or -1 (errno set). */
static int uname_of(const char *name, ntfschar **u)
{
    *u = NULL;
    if (!name || !*name || !strcmp(name, ".") || !strcmp(name, "..") || strchr(name, '/')) { errno = EINVAL; return -1; }
    int len = ntfsdrv_mbstoucs(name, u);
    if (len < 0) return -1;
    if (len > NTFS_MAX_NAME_LEN) { free(*u); *u = NULL; errno = ENAMETOOLONG; return -1; }
    return len;
}

/* Look up `name` in an already open directory and return its MFT record number. */
static int lookup_in(ntfsdrv_inode *dir_ni, const char *name, uint64_t *ino)
{
    /* Deliberately not ntfsdrv_inode_lookup_by_mbsname(): it consults a name cache
     * that also caches negative results and is only invalidated by path-based
     * deletes, so an inode-based caller could see stale answers. Searching the
     * directory's B+tree index directly is always current. */
    ntfschar *u = NULL;
    errno = 0;
    int len = ntfsdrv_mbstoucs(name, &u);
    if (len <= 0) return err_or(EINVAL);
    u64 mref = ntfsdrv_inode_lookup_by_name(dir_ni, u, len);
    free(u);
    if (mref == (u64)-1) return err_or(ENOENT);
    *ino = MREF(mref); /* strip the sequence number, keep the record number */
    return 0;
}

static int is_dir(ntfsdrv_inode *ni) { return (ni->mrec->flags & MFT_RECORD_IS_DIRECTORY) != 0; }

static const char intx_magic[8] = { 'I', 'n', 't', 'x', 'L', 'N', 'K', 1 };

/* Interix-style symlink (the common kind): SYSTEM attribute and data starting with "IntxLNK\1". */
static int is_interix_link(ntfsdrv_inode *ni)
{
    if (!(ni->flags & FILE_ATTR_SYSTEM) || is_dir(ni)) return 0;
    ntfsdrv_attr *na = ntfsdrv_attr_open(ni, AT_DATA, AT_UNNAMED, 0);
    if (!na) return 0;
    int ok = 0;
    char magic[8];
    if (na->data_size > 8 && na->data_size < 4096 && ntfsdrv_attr_pread(na, 0, 8, magic) == 8 && !memcmp(magic, intx_magic, 8)) ok = 1;
    ntfsdrv_attr_close(na);
    return ok;
}

/* Fill ntfstool_attr from an open inode. */
static int fill_attr(ntfsdrv_inode *ni, ntfstool_attr *a)
{
    memset(a, 0, sizeof(*a));
    a->ino = ni->mft_no;
    a->atime = ntfs2timespec(ni->last_access_time);
    a->mtime = ntfs2timespec(ni->last_data_change_time);
    a->ctime = ntfs2timespec(ni->last_mft_change_time);
    a->btime = ntfs2timespec(ni->creation_time);
    if (ni->flags & FILE_ATTR_HIDDEN) a->flags |= NTB_F_HIDDEN;
    if (ni->flags & FILE_ATTR_READONLY) a->flags |= NTB_F_READONLY;
    if (ni->flags & FILE_ATTR_SYSTEM) a->flags |= NTB_F_SYSTEM;
    if (ni->flags & FILE_ATTR_COMPRESSED) a->flags |= NTB_F_COMPRESSED;
    if (ni->flags & FILE_ATTR_REPARSE_POINT) a->flags |= NTB_F_REPARSE;
    if (is_dir(ni)) {
        a->type = NTB_DIR;
        a->nlink = 2;
        /* A directory's "size" is the size of its $I30 index allocation. */
        ntfsdrv_attr *na = ntfsdrv_attr_open(ni, AT_INDEX_ALLOCATION, NTFS_INDEX_I30, 4);
        if (na) { a->size = na->data_size; a->alloc = na->allocated_size; ntfsdrv_attr_close(na); }
        /* Directory reparse points (junctions, directory symlinks) are presented as symlinks. */
        if ((ni->flags & FILE_ATTR_REPARSE_POINT)) {
            char *t = ntfsdrv_make_symlink(ni, "");
            if (t) { a->type = NTB_SYMLINK; a->size = strlen(t); free(t); }
        }
        return 0;
    }
    a->type = NTB_FILE;
    a->nlink = le16_to_cpu(ni->mrec->link_count);
    if (a->nlink == 0) a->nlink = 1;
    /* Windows symlinks are reparse points; ntfsdrv_make_symlink() translates the target. */
    if (ni->flags & FILE_ATTR_REPARSE_POINT) {
        char *t = ntfsdrv_make_symlink(ni, "");
        if (t) { a->type = NTB_SYMLINK; a->size = strlen(t); free(t); return 0; }
    }
    ntfsdrv_attr *na = ntfsdrv_attr_open(ni, AT_DATA, AT_UNNAMED, 0);
    if (na) {
        a->size = na->data_size;
        /* For compressed/sparse files the space really used is compressed_size. */
        a->alloc = (na->data_flags & (ATTR_COMPRESSION_MASK | ATTR_IS_SPARSE)) ? na->compressed_size : na->allocated_size;
        ntfsdrv_attr_close(na);
    }
    if (is_interix_link(ni)) {
        a->type = NTB_SYMLINK;
        /* Data = 8-byte magic + UTF-16 target; report the target length in characters. */
        a->size = a->size > 8 ? (a->size - 8) / 2 : 0;
    }
    return 0;
}

/* ===================================================================== */
/* Probe / mount / volume-wide operations                                */
/* ===================================================================== */

int ntfstool_probe(const ntfstool_io *io, uint64_t *serial)
{
    int ok;
    uint64_t s = read_serial(io, &ok);
    if (!ok) return EINVAL;
    if (serial) *serial = s;
    return 0;
}

ntfstool_vol *ntfstool_mount(const ntfstool_io *io, int readonly, int *err)
{
    int is_ntfs;
    *err = 0;
    ntfstool_vol *v = calloc(1, sizeof(*v));
    devpriv *p = calloc(1, sizeof(*p));
    if (!v || !p) { free(v); free(p); *err = ENOMEM; return NULL; }
    v->io = *io;
    v->readonly = readonly;
    v->serial = read_serial(io, &is_ntfs);
    if (!is_ntfs) { free(v); free(p); *err = EINVAL; return NULL; }
    p->io = *io;
    p->readonly = readonly;
    ntfsdrv_log_set_handler(ntfsdrv_log_handler_null); /* keep libntfstool-driver quiet; errors come back as errno */
    struct ntfsdrv_device *dev = ntfsdrv_device_alloc("ntfstool", 0, &ntfstool_dev_ops, p);
    if (!dev) { *err = err_or(ENOMEM); free(v); free(p); return NULL; }
    errno = 0;
    /* NTFS_MNT_RECOVER lets libntfstool-driver replay/reset an unclean $LogFile as expected.
     * A hibernated or Fast Startup volume fails here with EPERM; the caller may retry read-only. */
    v->vol = ntfsdrv_device_mount(dev, readonly ? NTFS_MNT_RDONLY : NTFS_MNT_RECOVER);
    if (!v->vol) {
        *err = err_or(EIO);
        ntfsdrv_device_free(dev);
        free(v); free(p);
        return NULL;
    }
    /* Create the LRU caches as a regular mount does. This is needed for
     * correctness, not just speed: without them, name lookups in case-sensitive
     * mode reach ntfs_inode_lookup_by_mbsname() with a NULL name buffer and fail
     * with EINVAL (a regular mount never hits this because it always creates the caches). */
    ntfsdrv_create_lru_caches(v->vol);
    /* Hide system metadata files ($MFT, ...) and DOS short names from listings. */
    ntfsdrv_set_shown_files(v->vol, FALSE, TRUE, FALSE);
    /* Load the free cluster / free MFT record counts used by statfs. */
    if (ntfsdrv_volume_get_free_space(v->vol)) {
        *err = err_or(EIO);
        ntfsdrv_umount(v->vol, TRUE);
        free(v); free(p);
        return NULL;
    }
    return v;
}

int ntfstool_umount(ntfstool_vol *v)
{
    if (!v) return 0;
    devpriv *p = P(v->vol->dev);
    /* ntfsdrv_umount() writes back the volume state, closes the system files and calls d_close(). */
    int r = ntfsdrv_umount(v->vol, FALSE) ? err_or(EIO) : 0;
    free(p);
    free(v);
    return r;
}

int ntfstool_sync(ntfstool_vol *v)
{
    if (v->readonly) return 0;
    struct ntfsdrv_device *dev = v->vol->dev;
    return dev->d_ops->sync(dev) ? err_or(EIO) : 0;
}

int ntfstool_is_readonly(ntfstool_vol *v) { return v->readonly || NVolReadOnly(v->vol); }
uint64_t ntfstool_serial(ntfstool_vol *v) { return v->serial; }

char *ntfstool_label(ntfstool_vol *v) { return strdup(v->vol->vol_name ? v->vol->vol_name : ""); }

int ntfstool_set_label(ntfstool_vol *v, const char *label)
{
    if (ntfstool_is_readonly(v)) return EROFS;
    ntfschar *u = NULL;
    int len = ntfsdrv_mbstoucs(label, &u);
    if (len < 0) return err_or(EINVAL);
    if (len > 128) { free(u); return ENAMETOOLONG; } /* $VOLUME_NAME holds at most 128 UTF-16 units */
    int r = ntfsdrv_volume_rename(v->vol, u, len) ? err_or(EIO) : 0;
    free(u);
    if (!r) { free(v->vol->vol_name); v->vol->vol_name = strdup(label); }
    return r;
}

int ntfstool_statfs_get(ntfstool_vol *v, ntfstool_statfs *o)
{
    ntfsdrv_volume *vol = v->vol;
    o->cluster_size = vol->cluster_size;
    o->total_clusters = (uint64_t)vol->nr_clusters;
    o->free_clusters = vol->free_clusters > 0 ? (uint64_t)vol->free_clusters : 0;
    /* Free "files" = free MFT records plus the records that would fit into the
     * free clusters if $MFT grew (same estimate as the usual statfs). */
    s64 freefiles = vol->free_clusters > 0 ? vol->free_clusters : 0;
    int delta = vol->cluster_size_bits - vol->mft_record_size_bits;
    freefiles = delta >= 0 ? freefiles << delta : freefiles >> -delta;
    if (vol->free_mft_records > 0) freefiles += vol->free_mft_records;
    o->free_files = (uint64_t)freefiles;
    o->total_files = (uint64_t)(vol->mftbmp_na ? vol->mftbmp_na->allocated_size << 3 : 0) + o->free_files;
    return 0;
}

/* ===================================================================== */
/* Lookup / attributes / directory listing                               */
/* ===================================================================== */

int ntfstool_lookup(ntfstool_vol *v, uint64_t dir, const char *name, uint64_t *ino)
{
    ntfsdrv_inode *dir_ni = open_ino(v, dir);
    if (!dir_ni) return err_or(ENOENT);
    int r = is_dir(dir_ni) ? lookup_in(dir_ni, name, ino) : ENOTDIR;
    ntfsdrv_inode_close(dir_ni);
    /* Records 0..15 are NTFS metadata files ($MFT, $LogFile, ...); never expose them. */
    if (!r && *ino < FILE_first_user && *ino != FILE_root) r = ENOENT;
    return r;
}

int ntfstool_getattr(ntfstool_vol *v, uint64_t ino, ntfstool_attr *out)
{
    ntfsdrv_inode *ni = open_ino(v, ino);
    if (!ni) return err_or(ENOENT);
    int r = fill_attr(ni, out);
    ntfsdrv_inode_close(ni);
    return r;
}

/* State passed through ntfsdrv_readdir() to filler(). */
typedef struct {
    ntfstool_dir_cb cb;
    void *ctx;
    int stop;
} rdctx;

/* ntfsdrv_readdir() callback: filter and convert one index entry. */
static int filler(void *dirent, const ntfschar *name, const int name_len, const int name_type,
                  const s64 pos, const MFT_REF mref, const unsigned dt_type)
{
    rdctx *c = dirent;
    (void)pos;
    if (c->stop) return 1;
    if (name_type == FILE_NAME_DOS) return 0;            /* 8.3 alias of a long name: skip duplicate */
    if (MREF(mref) < FILE_first_user) return 0;          /* metadata files, and "." / ".." in the root */
    char *s = NULL;
    if (ntfsdrv_ucstombs(name, name_len, &s, 0) < 0) return 0;
    if (!strcmp(s, ".") || !strcmp(s, "..")) { free(s); return 0; }
    uint32_t t = dt_type == NTFS_DT_DIR ? NTB_DIR : (dt_type == NTFS_DT_LNK || dt_type == NTFS_DT_REPARSE) ? NTB_SYMLINK : NTB_FILE;
    if (c->cb(c->ctx, s, MREF(mref), t)) c->stop = 1;
    free(s);
    return c->stop;
}

int ntfstool_readdir(ntfstool_vol *v, uint64_t dir, ntfstool_dir_cb cb, void *ctx)
{
    ntfsdrv_inode *ni = open_ino(v, dir);
    if (!ni) return err_or(ENOENT);
    if (!is_dir(ni)) { ntfsdrv_inode_close(ni); return ENOTDIR; }
    rdctx c = { cb, ctx, 0 };
    s64 pos = 0;
    errno = 0;
    int r = ntfsdrv_readdir(ni, &pos, &c, filler);
    int e = (r && !c.stop) ? err_or(EIO) : 0; /* a stop requested by the callback is not an error */
    ntfsdrv_inode_close(ni);
    return e;
}

/* ===================================================================== */
/* File data                                                             */
/* ===================================================================== */

int ntfstool_read(ntfstool_vol *v, uint64_t ino, uint64_t off, void *buf, size_t len, size_t *done)
{
    *done = 0;
    ntfsdrv_inode *ni = open_ino(v, ino);
    if (!ni) return err_or(ENOENT);
    if (is_dir(ni)) { ntfsdrv_inode_close(ni); return EISDIR; }
    ntfsdrv_attr *na = ntfsdrv_attr_open(ni, AT_DATA, AT_UNNAMED, 0); /* the unnamed $DATA stream = file contents */
    if (!na) { int e = err_or(EIO); ntfsdrv_inode_close(ni); return e; }
    int r = 0;
    if ((s64)off < na->data_size) {
        if ((s64)(off + len) > na->data_size) len = (size_t)(na->data_size - (s64)off);
        size_t total = 0;
        /* ntfsdrv_attr_pread may return short counts (e.g. at compression block boundaries). */
        while (total < len) {
            s64 n = ntfsdrv_attr_pread(na, (s64)(off + total), (s64)(len - total), (char *)buf + total);
            if (n <= 0) { if (n < 0) r = err_or(EIO); break; }
            total += (size_t)n;
        }
        *done = total;
    }
    ntfsdrv_attr_close(na);
    ntfsdrv_inode_close(ni);
    return r;
}

int ntfstool_write(ntfstool_vol *v, uint64_t ino, uint64_t off, const void *buf, size_t len, size_t *done)
{
    *done = 0;
    if (ntfstool_is_readonly(v)) return EROFS;
    ntfsdrv_inode *ni = open_ino(v, ino);
    if (!ni) return err_or(ENOENT);
    if (is_dir(ni)) { ntfsdrv_inode_close(ni); return EISDIR; }
    ntfsdrv_attr *na = ntfsdrv_attr_open(ni, AT_DATA, AT_UNNAMED, 0);
    if (!na) { int e = err_or(EIO); ntfsdrv_inode_close(ni); return e; }
    int r = 0;
    size_t total = 0;
    while (total < len) {
        errno = 0;
        s64 n = ntfsdrv_attr_pwrite(na, (s64)(off + total), (s64)(len - total), (const char *)buf + total);
        if (n <= 0) { r = err_or(EIO); break; }
        total += (size_t)n;
    }
    *done = total;
    /* Compressed files: ntfsdrv_attr_pclose() writes out the last partial compression block. */
    if (total && (na->data_flags & ATTR_COMPRESSION_MASK)) ntfsdrv_attr_pclose(na);
    ntfsdrv_attr_close(na);
    if (total) ntfsdrv_inode_update_times(ni, NTFS_UPDATE_MCTIME);
    /* Closing the inode writes the MFT record (size, times) back to the device. */
    if (ntfsdrv_inode_close(ni) && !r) r = err_or(EIO);
    return r;
}

int ntfstool_truncate(ntfstool_vol *v, uint64_t ino, uint64_t size)
{
    if (ntfstool_is_readonly(v)) return EROFS;
    ntfsdrv_inode *ni = open_ino(v, ino);
    if (!ni) return err_or(ENOENT);
    if (is_dir(ni)) { ntfsdrv_inode_close(ni); return EISDIR; }
    ntfsdrv_attr *na = ntfsdrv_attr_open(ni, AT_DATA, AT_UNNAMED, 0);
    if (!na) { int e = err_or(EIO); ntfsdrv_inode_close(ni); return e; }
    int r = 0;
    if (na->data_size != (s64)size) {
        errno = 0;
        if (ntfsdrv_attr_truncate(na, (s64)size)) r = err_or(EIO);
        else ntfsdrv_inode_update_times(ni, NTFS_UPDATE_MCTIME);
    }
    ntfsdrv_attr_close(na);
    if (ntfsdrv_inode_close(ni) && !r) r = err_or(EIO);
    return r;
}

int ntfstool_flush(ntfstool_vol *v, uint64_t ino)
{
    if (ntfstool_is_readonly(v)) return 0;
    ntfsdrv_inode *ni = open_ino(v, ino);
    if (!ni) return err_or(ENOENT);
    /* Also refreshes the FILE_NAME copies in the parent directory indexes. */
    int r = ntfsdrv_inode_sync(ni) ? err_or(EIO) : 0;
    ntfsdrv_inode_close(ni);
    if (!r) r = ntfstool_sync(v);
    return r;
}
