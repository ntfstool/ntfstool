/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */

#ifndef NTFSDRV_NTFS_REPARSE_TAG_H
#define NTFSDRV_NTFS_REPARSE_TAG_H

char *ntfsdrv_make_symlink(ntfsdrv_inode *ni, const char *mnt_point);

BOOL ntfsdrv_possible_symlink(ntfsdrv_inode *ni);

int ntfsdrv_get_ntfs_reparse_data(ntfsdrv_inode *ni, char *value, size_t size);

char *ntfsdrv_get_abslink(ntfsdrv_volume *vol, ntfschar *junction, 
			int count, const char *mnt_point, BOOL isdir);

REPARSE_POINT *ntfsdrv_get_reparse_point(ntfsdrv_inode *ni);

int ntfsdrv_reparse_check_wsl(ntfsdrv_inode *ni, const REPARSE_POINT *reparse);

int ntfsdrv_reparse_set_wsl_symlink(ntfsdrv_inode *ni,
			const ntfschar *target, int target_len);

int ntfsdrv_reparse_set_wsl_not_symlink(ntfsdrv_inode *ni, mode_t mode);

int ntfsdrv_set_ntfs_reparse_data(ntfsdrv_inode *ni, const char *value,
			size_t size, int flags);
int ntfsdrv_remove_ntfs_reparse_data(ntfsdrv_inode *ni);

int ntfsdrv_delete_reparse_index(ntfsdrv_inode *ni);

#endif /* NTFSDRV_NTFS_REPARSE_TAG_H */
