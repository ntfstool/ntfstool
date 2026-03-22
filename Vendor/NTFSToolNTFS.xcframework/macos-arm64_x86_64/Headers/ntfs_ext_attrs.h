/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */

#ifndef NTFSDRV_NTFS_EXT_ATTRS_H
#define NTFSDRV_NTFS_EXT_ATTRS_H

int ntfsdrv_ea_check_wsldev(ntfsdrv_inode *ni, dev_t *rdevp);

int ntfsdrv_ea_set_wsl_not_symlink(ntfsdrv_inode *ni, mode_t mode, dev_t dev);

int ntfsdrv_get_ntfs_ea(ntfsdrv_inode *ni, char *value, size_t size);

int ntfsdrv_set_ntfs_ea(ntfsdrv_inode *ni, const char *value, size_t size, int flags);

int ntfsdrv_remove_ntfs_ea(ntfsdrv_inode *ni);

#endif /* NTFSDRV_NTFS_EXT_ATTRS_H */
