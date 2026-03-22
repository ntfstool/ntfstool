/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_DIAG_DEBUG_H
#define NTFSDRV_NTFS_DIAG_DEBUG_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ntfs_logger.h"

struct _runlist_element;

#ifdef DEBUG
extern void ntfsdrv_debug_runlist_dump(const struct _runlist_element *rl);
#else
static __inline__ void ntfsdrv_debug_runlist_dump(const struct _runlist_element *rl __attribute__((unused))) {}
#endif

#define NTFS_BUG(msg)							\
{									\
	int ___i = 1;							\
	ntfsdrv_log_critical("Bug in %s(): %s\n", __FUNCTION__, msg);	\
	ntfsdrv_log_debug("Forcing segmentation fault!");			\
	___i = ((int*)NULL)[___i];					\
}

#endif /* defined NTFSDRV_NTFS_DIAG_DEBUG_H */
