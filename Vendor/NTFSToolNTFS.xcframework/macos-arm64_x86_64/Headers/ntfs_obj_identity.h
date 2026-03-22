/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */

#ifndef NTFSDRV_NTFS_OBJ_IDENTITY_H
#define NTFSDRV_NTFS_OBJ_IDENTITY_H

int ntfsdrv_get_ntfs_object_id(ntfsdrv_inode *ni, char *value, size_t size);

int ntfsdrv_set_ntfs_object_id(ntfsdrv_inode *ni, const char *value,
			size_t size, int flags);
int ntfsdrv_remove_ntfs_object_id(ntfsdrv_inode *ni);

int ntfsdrv_delete_object_id_index(ntfsdrv_inode *ni);

#endif /* NTFSDRV_NTFS_OBJ_IDENTITY_H */
