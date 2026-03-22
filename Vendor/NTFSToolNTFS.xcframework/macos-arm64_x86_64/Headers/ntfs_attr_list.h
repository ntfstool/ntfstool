/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_ATTR_LIST_H
#define NTFSDRV_NTFS_ATTR_LIST_H

#include "ntfs_attr_core.h"

extern int ntfsdrv_attrlist_need(ntfsdrv_inode *ni);

extern int ntfsdrv_attrlist_entry_add(ntfsdrv_inode *ni, ATTR_RECORD *attr);
extern int ntfsdrv_attrlist_entry_rm(ntfsdrv_attr_search_ctx *ctx);

/**
 * ntfsdrv_attrlist_mark_dirty - mark the attribute list dirty
 * @ni:		NTFS inode whose base inode holds the dirty attribute list
 *
 * Mark the attribute list dirty so it is written back to disk later (at the latest
 * in ntfsdrv_inode_close()).
 *
 * This function cannot fail.
 */
static __inline__ void ntfsdrv_attrlist_mark_dirty(ntfsdrv_inode *ni)
{
	if (ni->nr_extents == -1)
		NInoAttrListSetDirty(ni->base_ni);
	else
		NInoAttrListSetDirty(ni);
}

#endif /* defined NTFSDRV_NTFS_ATTR_LIST_H */
