/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_DIR_CATALOG_H
#define NTFSDRV_NTFS_DIR_CATALOG_H

#include "ntfs_core_types.h"

#define PATH_SEP '/'

/*
 * These are not defined under DJGPP, so define versions that do not
 * conflict with other S_IF definitions under DJGPP.
 */
#ifdef DJGPP
#ifndef S_IFLNK
#define S_IFLNK  0120000
#endif
#ifndef S_ISLNK
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#endif
#ifndef S_IFSOCK
#define S_IFSOCK 0140000
#endif
#ifndef S_ISSOCK
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#endif
#endif

/*
 * Global constants for the little-endian Unicode strings $I30, $SII, $SDH, $O, $Q, $R.
 *
 */
extern ntfschar NTFS_INDEX_I30[5];
extern ntfschar NTFS_INDEX_SII[5];
extern ntfschar NTFS_INDEX_SDH[5];
extern ntfschar NTFS_INDEX_O[3];
extern ntfschar NTFS_INDEX_Q[3];

extern u64 ntfsdrv_inode_lookup_by_name(ntfsdrv_inode *dir_ni,
		const ntfschar *uname, const int uname_len);
extern u64 ntfsdrv_inode_lookup_by_mbsname(ntfsdrv_inode *dir_ni, const char *name);

extern ntfsdrv_inode *ntfsdrv_pathname_to_inode(ntfsdrv_volume *vol, ntfsdrv_inode *parent,
		const char *pathname);
extern ntfsdrv_inode *ntfsdrv_create(ntfsdrv_inode *dir_ni, le32 securid,
		const ntfschar *name, u8 name_len, mode_t type);
extern ntfsdrv_inode *ntfsdrv_create_device(ntfsdrv_inode *dir_ni, le32 securid,
		const ntfschar *name, u8 name_len, mode_t type, dev_t dev);
extern ntfsdrv_inode *ntfsdrv_create_symlink(ntfsdrv_inode *dir_ni, le32 securid,
		const ntfschar *name, u8 name_len, const ntfschar *target,
		int target_len);
extern int ntfsdrv_check_empty_dir(ntfsdrv_inode *ni);
extern int ntfsdrv_delete(ntfsdrv_volume *vol, const char *path,
		ntfsdrv_inode *ni, ntfsdrv_inode *dir_ni, const ntfschar *name,
		u8 name_len);

extern int ntfsdrv_link(ntfsdrv_inode *ni, ntfsdrv_inode *dir_ni, const ntfschar *name,
		u8 name_len);

/*
 * File types (adapted from <linux/fs.h>)
 */
#define NTFS_DT_UNKNOWN		0
#define NTFS_DT_FIFO		1
#define NTFS_DT_CHR		2
#define NTFS_DT_DIR		4
#define NTFS_DT_BLK		6
#define NTFS_DT_REG		8
#define NTFS_DT_LNK		10
#define NTFS_DT_SOCK		12
#define NTFS_DT_WHT		14
#define NTFS_DT_REPARSE		32

/*
 * This is the "ntfsdrv_filldir" function type, used by ntfsdrv_readdir(),
 * to let the caller specify the desired directory entry layout.
 * This allows the caller to read a directory into an application,
 * or to use different directory entry layouts depending on the binary type.
 */
typedef int (*ntfsdrv_filldir_t)(void *dirent, const ntfschar *name,
		const int name_len, const int name_type, const s64 pos,
		const MFT_REF mref, const unsigned dt_type);

extern int ntfsdrv_readdir(ntfsdrv_inode *dir_ni, s64 *pos,
		void *dirent, ntfsdrv_filldir_t filldir);

ntfsdrv_inode *ntfsdrv_dir_parent_inode(ntfsdrv_inode *ni);
u32 ntfsdrv_interix_types(ntfsdrv_inode *ni);

int ntfsdrv_get_ntfs_dos_name(ntfsdrv_inode *ni, ntfsdrv_inode *dir_ni,
			char *value, size_t size);
int ntfsdrv_set_ntfs_dos_name(ntfsdrv_inode *ni, ntfsdrv_inode *dir_ni,
			const char *value, size_t size,	int flags);
int ntfsdrv_remove_ntfs_dos_name(ntfsdrv_inode *ni, ntfsdrv_inode *dir_ni);
int ntfsdrv_dir_link_cnt(ntfsdrv_inode *ni);

#if CACHE_INODE_SIZE

struct CACHED_GENERIC;

extern int ntfsdrv_dir_inode_hash(const struct CACHED_GENERIC *cached);
extern int ntfsdrv_dir_lookup_hash(const struct CACHED_GENERIC *cached);

#endif

#endif /* defined NTFSDRV_NTFS_DIR_CATALOG_H */
