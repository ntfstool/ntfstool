/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_COMPRESSION_H
#define NTFSDRV_NTFS_COMPRESSION_H

#include "ntfs_core_types.h"
#include "ntfs_attr_core.h"

extern s64 ntfsdrv_compressed_attr_pread(ntfsdrv_attr *na, s64 pos, s64 count,
		void *b);

extern s64 ntfsdrv_compressed_pwrite(ntfsdrv_attr *na, runlist_element *brl, s64 wpos,
				s64 offs, s64 to_write, s64 rounded,
				const void *b, int compressed_part,
				VCN *update_from);

extern int ntfsdrv_compressed_close(ntfsdrv_attr *na, runlist_element *brl,
				s64 offs, VCN *update_from);

#endif /* defined NTFSDRV_NTFS_COMPRESSION_H */

