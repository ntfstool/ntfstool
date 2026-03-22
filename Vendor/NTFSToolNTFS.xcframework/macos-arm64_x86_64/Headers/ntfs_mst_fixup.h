/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_MST_FIXUP_H
#define NTFSDRV_NTFS_MST_FIXUP_H

#include "ntfs_core_types.h"
#include "ntfs_fs_layout.h"
#include "ntfs_vol_context.h"

extern int ntfsdrv_mst_post_read_fixup(NTFS_RECORD *b, const u32 size);
extern int ntfsdrv_mst_post_read_fixup_warn(NTFS_RECORD *b, const u32 size,
					BOOL warn);
extern int ntfsdrv_mst_pre_write_fixup(NTFS_RECORD *b, const u32 size);
extern void ntfsdrv_mst_post_write_fixup(NTFS_RECORD *b);

#endif /* defined NTFSDRV_NTFS_MST_FIXUP_H */

