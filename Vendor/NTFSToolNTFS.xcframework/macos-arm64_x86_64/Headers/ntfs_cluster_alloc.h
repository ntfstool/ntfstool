/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_CLUSTER_ALLOC_H
#define NTFSDRV_NTFS_CLUSTER_ALLOC_H

#include "ntfs_core_types.h"
#include "ntfs_runlist_map.h"
#include "ntfs_vol_context.h"

/**
 * enum NTFS_CLUSTER_ALLOCATION_ZONES - enumeration of cluster allocation zones
 */
typedef enum {
	FIRST_ZONE	= 0,	/* for sanity checking */
	MFT_ZONE	= 0,	/* allocate from the MFT zone */
	DATA_ZONE	= 1,	/* allocate from the data zone */
	LAST_ZONE	= 1,	/* for sanity checking */
} NTFS_CLUSTER_ALLOCATION_ZONES;

extern runlist *ntfsdrv_cluster_alloc(ntfsdrv_volume *vol, VCN start_vcn, s64 count,
		LCN start_lcn, const NTFS_CLUSTER_ALLOCATION_ZONES zone);

extern int ntfsdrv_cluster_free_from_rl(ntfsdrv_volume *vol, runlist *rl);
extern int ntfsdrv_cluster_free_basic(ntfsdrv_volume *vol, s64 lcn, s64 count);

extern int ntfsdrv_cluster_free(ntfsdrv_volume *vol, ntfsdrv_attr *na, VCN start_vcn,
		s64 count);

#endif /* defined NTFSDRV_NTFS_CLUSTER_ALLOC_H */
