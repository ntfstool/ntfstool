/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_MFT_TABLE_H
#define NTFSDRV_NTFS_MFT_TABLE_H

#include "ntfs_vol_context.h"
#include "ntfs_vnode_core.h"
#include "ntfs_fs_layout.h"
#include "ntfs_logger.h"

extern int ntfsdrv_mft_records_read(const ntfsdrv_volume *vol, const MFT_REF mref,
		const s64 count, MFT_RECORD *b);

/**
 * ntfsdrv_mft_record_read - read one record from the MFT
 * @vol:	volume to read from
 * @mref:	MFT record number to read
 * @b:		output data buffer
 *
 * Read the MFT record specified by @mref from volume @vol into buffer @b.
 * Returns 0 on success, or -1 with errno set on failure.
 *
 * The MFT record read has already been MST-unprotected and can be used directly.
 * Callers should check the record with is_baad_record()
 * in case MST unprotection failed.
 *
 * Note: @b must be at least vol->mft_record_size bytes.
 */
static __inline__ int ntfsdrv_mft_record_read(const ntfsdrv_volume *vol,
		const MFT_REF mref, MFT_RECORD *b)
{
	int ret;

	ntfsdrv_log_enter("Entering for inode %lld\n", (long long)MREF(mref));
	ret = ntfsdrv_mft_records_read(vol, mref, 1, b);
	ntfsdrv_log_leave("\n");
	return ret;
}

extern int ntfsdrv_mft_record_check(const ntfsdrv_volume *vol, const MFT_REF mref,
		MFT_RECORD *m);

extern int ntfsdrv_file_record_read(const ntfsdrv_volume *vol, const MFT_REF mref,
		MFT_RECORD **mrec, ATTR_RECORD **attr);

extern int ntfsdrv_mft_records_write(const ntfsdrv_volume *vol, const MFT_REF mref,
		const s64 count, MFT_RECORD *b);

/**
 * ntfsdrv_mft_record_write - write an MFT record to disk
 * @vol:	volume to write to
 * @mref:	MFT record number to write
 * @b:		data buffer containing the MFT record to write
 *
 * Write the MFT record specified by @mref in buffer @b to volume @vol.
 * Returns 0 on success, or -1 with errno set on failure.
 *
 * The MFT record is MST-protected before writing and unprotected afterwards,
 * so the update sequence number in buffer @b is incremented.
 *
 * Note: @b must be at least vol->mft_record_size bytes.
 */
static __inline__ int ntfsdrv_mft_record_write(const ntfsdrv_volume *vol,
		const MFT_REF mref, MFT_RECORD *b)
{
	int ret;

	ntfsdrv_log_enter("Entering for inode %lld\n", (long long)MREF(mref));
	ret = ntfsdrv_mft_records_write(vol, mref, 1, b);
	ntfsdrv_log_leave("\n");
	return ret;
}

/**
 * ntfsdrv_mft_record_get_data_size - return the number of bytes in use in MFT record @m
 * @m:		MFT record to get the data size of
 *
 * Takes MFT record @m and returns the number of bytes in use in the record,
 * or 0 on error (i.e. @m is not a valid MFT record).
 * 0 is not a valid size for an MFT record, as it contains at least
 * MFT_RECORD itself and a zero-length attribute of type AT_END,
 * so the minimum size is 56 bytes.
 *
 * Note: the size does not depend on NTFS version 1.x/3.x, because the first attribute's
 * 8 byte alignment masks the difference between NTFS 1.x and 3.x in MFT_RECORD MFT_RECORD
 * size. Also, every MFT record is expected to contain an update sequence array,
 * although in theory it may be absent.
 */
static __inline__ u32 ntfsdrv_mft_record_get_data_size(const MFT_RECORD *m)
{
	if (!m || !ntfsdrv_is_mft_record(m->magic))
		return 0;
	/* Get the number of bytes in use and return it.*/
	return le32_to_cpu(m->bytes_in_use);
}

extern int ntfsdrv_mft_record_layout(const ntfsdrv_volume *vol, const MFT_REF mref,
		MFT_RECORD *mrec);

extern int ntfsdrv_mft_record_format(const ntfsdrv_volume *vol, const MFT_REF mref);

extern ntfsdrv_inode *ntfsdrv_mft_record_alloc(ntfsdrv_volume *vol, ntfsdrv_inode *base_ni);

extern ntfsdrv_inode *ntfsdrv_mft_rec_alloc(ntfsdrv_volume *vol, BOOL mft_data);

extern int ntfsdrv_mft_record_free(ntfsdrv_volume *vol, ntfsdrv_inode *ni);

extern int ntfsdrv_mft_usn_dec(MFT_RECORD *mrec);

#endif /* defined NTFSDRV_NTFS_MFT_TABLE_H */
