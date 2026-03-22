/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_XATTR_NAMED_H
#define NTFSDRV_NTFS_XATTR_NAMED_H

/*
 * Flags that modify setxattr() semantics. These flags are also used by several
 * libntfstool-driver libntfstool-driver functions (e.g. ntfsdrv_set_ntfs_acl()),
 * which were originally tied to extended attribute support, but can now be used by applications
 * even if the platform does not support extended attributes.
 *
 * Note: applications including this header on platforms that support extended attributes should define
 * HAVE_SETXATTR HAVE_SETXATTR or HAVE_SYS_XATTR_H. Otherwise the flag values defined may be incorrect
 * (correct for Linux, but not necessarily for other platforms).
 */
#if defined(HAVE_SETXATTR) || defined(HAVE_SYS_XATTR_H)
#include <sys/xattr.h>
#else
#include "ntfs_compat_layer.h" /* possibly for the ENODATA definition */
#define XATTR_CREATE	1
#define XATTR_REPLACE	2
#endif

/*
 *		identifier of data mapped to the system namespace
 */

enum SYSTEMXATTRS {
	XATTR_UNMAPPED,
	XATTR_NTFS_ACL,
	XATTR_NTFS_ATTRIB,
	XATTR_NTFS_ATTRIB_BE,
	XATTR_NTFS_EFSINFO,
	XATTR_NTFS_REPARSE_DATA,
	XATTR_NTFS_OBJECT_ID,
	XATTR_NTFS_DOS_NAME,
	XATTR_NTFS_TIMES,
	XATTR_NTFS_TIMES_BE,
	XATTR_NTFS_CRTIME,
	XATTR_NTFS_CRTIME_BE,
	XATTR_NTFS_EA,
	XATTR_POSIX_ACC, 
	XATTR_POSIX_DEF
} ;

struct XATTRMAPPING {
	struct XATTRMAPPING *next;
	enum SYSTEMXATTRS xattr;
	char name[1]; /* variable length */
} ;

#ifdef XATTR_MAPPINGS

struct XATTRMAPPING *ntfsdrv_xattr_build_mapping(ntfsdrv_volume *vol,
			const char *path);
void ntfsdrv_xattr_free_mapping(struct XATTRMAPPING*);

#endif /* XATTR_MAPPINGS */

enum SYSTEMXATTRS ntfsdrv_xattr_system_type(const char *name,
			ntfsdrv_volume *vol);

struct SECURITY_CONTEXT;

int ntfsdrv_xattr_system_getxattr(struct SECURITY_CONTEXT *scx,
			enum SYSTEMXATTRS attr,
			ntfsdrv_inode *ni, ntfsdrv_inode *dir_ni,
			char *value, size_t size);
int ntfsdrv_xattr_system_setxattr(struct SECURITY_CONTEXT *scx,
			enum SYSTEMXATTRS attr,
			ntfsdrv_inode *ni, ntfsdrv_inode *dir_ni,
			const char *value, size_t size, int flags);
int ntfsdrv_xattr_system_removexattr(struct SECURITY_CONTEXT *scx,
			enum SYSTEMXATTRS attr,
			ntfsdrv_inode *ni, ntfsdrv_inode *dir_ni);

#endif /* NTFSDRV_NTFS_XATTR_NAMED_H */
