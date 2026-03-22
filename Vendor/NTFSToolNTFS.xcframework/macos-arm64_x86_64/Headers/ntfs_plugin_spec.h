/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */

/*
 *	This file defines the ntfstool-driver plugin interface,
 *	used to add handling support for specific types of reparse points.
 */

#ifndef NTFSDRV_NTFS_PLUGIN_SPEC_H
#define NTFSDRV_NTFS_PLUGIN_SPEC_H

#include "ntfs_fs_layout.h"
#include "ntfs_vnode_core.h"
#include "ntfs_dir_catalog.h"

struct fuse_file_info;
struct stat;

	/*
	 *		Currently defined plugin operations.
	 * These functions should return a non-negative value on success
	 * and a negative errno value on failure.
	 * They must not close or free their arguments.
	 * The file system must be left in a consistent state after each individual call.
	 * If an operation is not defined, an EOPNOTSUPP error is returned to the caller.
	 */
typedef struct plugin_operations {
	/*
	 *		Set the st_size, st_blocks and st_mode attributes
	 * in struct stat. The returned st_mode must at least
	 * define the file type. Depending on the permission options used at mount time,
	 * the returned permissions are subject to the umask, or are changed according to the
	 * ACL set on the file.
	 */
	int (*getattr)(ntfsdrv_inode *ni, const REPARSE_POINT *reparse,
			struct stat *stbuf);

	/*
	 *		Open a file for reading or writing
	 * The fi->flags field indicates how it is opened.
	 * The fi->fh field may be used to store information
	 * for later reads and writes. If used, this field must be non-null.
	 * Returns zero on success, a negative errno value on failure.
	 */
	int (*open)(ntfsdrv_inode *ni, const REPARSE_POINT *reparse,
			struct fuse_file_info *fi);

	/*
	 *		Release an open file or directory
	 * Called only if fi->fh was set to a non-null value at open time.
	 * May be used to free context specific to the open file or directory.
	 * Returns zero on success, a negative errno value on failure.
	 */
	int (*release)(ntfsdrv_inode *ni, const REPARSE_POINT *reparse,
			struct fuse_file_info *fi);

	/*
	 *		Read from an open file
	 * The return value is the number of bytes read,
	 * or a negative errno value on failure.
	 * If the return value is positive, the access timestamp is updated after the call.
	 */
	int (*read)(ntfsdrv_inode *ni, const REPARSE_POINT *reparse,
			char *buf, size_t size,
			off_t offset, struct fuse_file_info *fi);

	/*
	 *		Write to an open file
	 * The file system must be left in a consistent state after each write call,
	 * and if the writing application is killed, the file itself must at least be deletable.
	 * The return value is the number of bytes written,
	 * or a negative errno value on failure.
	 * If the return value is positive, the modification timestamp is updated after the call.
	 */
	int (*write)(ntfsdrv_inode *ni, const REPARSE_POINT *reparse,
			const char *buf, size_t size,
			off_t offset, struct fuse_file_info *fi);

	/*
	 *		Get a symbolic link
	 * The symbolic link must be returned in an allocated buffer,
	 * encoded as a zero-terminated multibyte string compatible with the locale mount option.
	 * Returns zero on success, a negative errno value on failure.
	 */
	int (*readlink)(ntfsdrv_inode *ni, const REPARSE_POINT *reparse,
			char **pbuf);

	/*
	 *		Truncate a file (shorten or append zeros)
	 * Returns zero on success, a negative errno value on failure.
	 * If the return value is zero, the modification timestamp is updated after the call.
	 */
	int (*truncate)(ntfsdrv_inode *ni, const REPARSE_POINT *reparse,
			off_t size);
	/*
	 *		Open a directory
	 * The fi->flags field indicates how it is opened.
	 * The fi->fh field may be used to store information
	 * for later readdir() calls. If used,
	 * this field must be non-null and be released in release().
	 * Returns zero on success, a negative errno value on failure.
	 */
	int (*opendir)(ntfsdrv_inode *ni, const REPARSE_POINT *reparse,
			struct fuse_file_info *fi);

	/*
	 *		Get the entries in a directory
	 *
	 * Directory entries are returned using the filldir() function and the fillctx argument.
	 * The "." and ".." entries are expected to be returned.
	 * Returns zero on success, a negative errno value on failure.
	 */
	int (*readdir)(ntfsdrv_inode *ni, const REPARSE_POINT *reparse,
			s64 *pos, void *fillctx, ntfsdrv_filldir_t filldir,
			struct fuse_file_info *fi);
	/*
	 *		Create a new file of any type
	 *
	 * The return value is a pointer to the created inode,
	 * or NULL on failure, with errno indicating the reason.
	 */
	ntfsdrv_inode *(*create)(ntfsdrv_inode *dir_ni, const REPARSE_POINT *reparse,
			le32 securid, ntfschar *name, int name_len,
			mode_t type);
	/*
	 *		Link a new name for a file or directory
	 * Linking a directory is used to rename directories
	 * Returns zero on success, a negative errno value on failure.
	 * If the return value is zero, the modification timestamp is updated after the call.
	 */
	int (*link)(ntfsdrv_inode *dir_ni, const REPARSE_POINT *reparse,
			ntfsdrv_inode *ni, ntfschar *name, int name_len);
	/*
	 *		Unlink a name from a directory
	 * The pathname argument may be NULL
	 * Returns zero on success, a negative errno value on failure.
	 */
	int (*unlink)(ntfsdrv_inode *dir_ni, const REPARSE_POINT *reparse,
			const char *pathname,
			ntfsdrv_inode *ni, ntfschar *name, int name_len);
} plugin_operations_t;

/*
 *		Plugin initialization routine
 *	Returns the entry table on success, otherwise NULL
 *	and sets errno (for example EINVAL when the plugin does not support the tag).
 */
typedef const struct plugin_operations *(*plugin_init_t)(le32 tag);
const struct plugin_operations *init(le32 tag);

#endif /* NTFSDRV_NTFS_PLUGIN_SPEC_H */
