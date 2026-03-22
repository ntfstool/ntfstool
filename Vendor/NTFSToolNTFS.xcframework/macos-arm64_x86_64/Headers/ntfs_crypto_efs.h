/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_CRYPTO_EFS_H
#define NTFSDRV_NTFS_CRYPTO_EFS_H

int ntfsdrv_get_efs_info(ntfsdrv_inode *ni, char *value, size_t size);

int ntfsdrv_set_efs_info(ntfsdrv_inode *ni,
			const char *value, size_t size,	int flags);
int ntfsdrv_efs_fixup_attribute(ntfsdrv_attr_search_ctx *ctx, ntfsdrv_attr *na);

#endif /* NTFSDRV_NTFS_CRYPTO_EFS_H */
