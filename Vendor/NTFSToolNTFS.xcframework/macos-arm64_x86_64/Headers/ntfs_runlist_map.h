/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_RUNLIST_MAP_H
#define NTFSDRV_NTFS_RUNLIST_MAP_H

#include "ntfs_core_types.h"

/* Forward declarations */
typedef struct _runlist_element runlist_element;
typedef runlist_element runlist;

#include "ntfs_attr_core.h"
#include "ntfs_vol_context.h"

/**
 * struct _runlist_element - element of the in-memory VCN to LCN mapping array.
 * @vcn:	starting VCN of the current array element VCN
 * @lcn:	starting LCN of the current array element LCN
 * @length:	length in clusters of the current array element
 *
 * When length == 0, the last VCN has been reached (actually the last VCN + 1).
 *
 * When lcn == -1, the clusters starting at vcn are not physically allocated
 * (i.e. this is a hole / sparse data).
 */
struct _runlist_element {/* in-memory VCN to LCN mapping structure element.*/
	VCN vcn;	/* vcn = vcn = starting virtual cluster number.*/
	LCN lcn;	/* lcn = lcn = starting logical cluster number.*/
	s64 length;	/* run length in clusters.*/
};

extern runlist_element *ntfsdrv_rl_extend(ntfsdrv_attr *na, runlist_element *rl,
			int more_entries);

extern LCN ntfsdrv_rl_vcn_to_lcn(const runlist_element *rl, const VCN vcn);

extern s64 ntfsdrv_rl_pread(const ntfsdrv_volume *vol, const runlist_element *rl,
		const s64 pos, s64 count, void *b);
extern s64 ntfsdrv_rl_pwrite(const ntfsdrv_volume *vol, const runlist_element *rl,
		s64 ofs, const s64 pos, s64 count, void *b);

extern runlist_element *ntfsdrv_runlists_merge(runlist_element *drl,
		runlist_element *srl);

extern runlist_element *ntfsdrv_mapping_pairs_decompress(const ntfsdrv_volume *vol,
		const ATTR_RECORD *attr, runlist_element *old_rl);

extern int ntfsdrv_get_nr_significant_bytes(const s64 n);

extern int ntfsdrv_get_size_for_mapping_pairs(const ntfsdrv_volume *vol,
		const runlist_element *rl, const VCN start_vcn, int max_size);

extern int ntfsdrv_write_significant_bytes(u8 *dst, const u8 *dst_max,
		const s64 n);

extern int ntfsdrv_mapping_pairs_build(const ntfsdrv_volume *vol, u8 *dst,
		const int dst_len, const runlist_element *rl,
		const VCN start_vcn, runlist_element const **stop_rl);

extern int ntfsdrv_rl_truncate(runlist **arl, const VCN start_vcn);

extern int ntfsdrv_rl_sparse(runlist *rl);
extern s64 ntfsdrv_rl_get_compressed_size(ntfsdrv_volume *vol, runlist *rl);

#ifdef NTFS_TEST
int test_rl_main(int argc, char *argv[]);
#endif

#endif /* defined NTFSDRV_NTFS_RUNLIST_MAP_H */

