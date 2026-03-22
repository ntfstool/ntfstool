/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_BOOT_SECTOR_H
#define NTFSDRV_NTFS_BOOT_SECTOR_H

#include "ntfs_core_types.h"
#include "ntfs_vol_context.h"
#include "ntfs_fs_layout.h"

/**
 * ntfsdrv_boot_sector_is_ntfs - check whether a boot sector describes a valid NTFS volume
 * @b:		buffer containing the boot sector
 *
 * This function checks whether the boot sector in @b describes a valid NTFS volume.
 * Return TRUE if @b is a valid NTFS boot sector, otherwise FALSE.
 */
extern BOOL ntfsdrv_boot_sector_is_ntfs(NTFS_BOOT_SECTOR *b);
extern int ntfsdrv_boot_sector_parse(ntfsdrv_volume *vol, const NTFS_BOOT_SECTOR *bs);

#endif /* defined NTFSDRV_NTFS_BOOT_SECTOR_H */

