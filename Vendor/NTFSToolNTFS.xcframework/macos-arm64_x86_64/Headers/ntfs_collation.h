/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_COLLATION_H
#define NTFSDRV_NTFS_COLLATION_H

#include "ntfs_core_types.h"
#include "ntfs_vol_context.h"

#define NTFS_COLLATION_ERROR -2

extern COLLATE ntfsdrv_get_collate_function(COLLATION_RULES);

#endif /* NTFSDRV_NTFS_COLLATION_H */
