/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_BITMAP_MGR_H
#define NTFSDRV_NTFS_BITMAP_MGR_H

#include "ntfs_core_types.h"
#include "ntfs_attr_core.h"

/*
 * Notes:
 *
 * - - Operations only support 8 bits, so the functions work correctly on both little- and big-endian machines!
 *     Therefore do not change them to 32-bit operations!
 * - - A bitmap starts at bit = 0 and ends at bit = bitmap size - 1.
 * - - The caller must ensure the bit operated on is less than the bitmap size.
 */

extern void ntfsdrv_bit_set(u8 *bitmap, const u64 bit, const u8 new_value);
extern char ntfsdrv_bit_get(const u8 *bitmap, const u64 bit);
extern int  ntfsdrv_bitmap_set_run(ntfsdrv_attr *na, s64 start_bit, s64 count);
extern int  ntfsdrv_bitmap_clear_run(ntfsdrv_attr *na, s64 start_bit, s64 count);

/**
 * ntfsdrv_bitmap_set_bit - set a bit in a bitmap
 * @na:		attribute containing the bitmap
 * @bit:	bit to set
 *
 * Set @bit in the bitmap described by attribute @na.
 *
 * Return 0 on success, or -1 with errno set on error.
 */
static __inline__ int ntfsdrv_bitmap_set_bit(ntfsdrv_attr *na, s64 bit)
{
	return ntfsdrv_bitmap_set_run(na, bit, 1);
}

/**
 * ntfsdrv_bitmap_clear_bit - clear a bit in a bitmap
 * @na:		attribute containing the bitmap
 * @bit:	bit to clear
 *
 * Clear @bit in the bitmap described by attribute @na.
 *
 * Return 0 on success, or -1 with errno set on error.
 */
static __inline__ int ntfsdrv_bitmap_clear_bit(ntfsdrv_attr *na, s64 bit)
{
	return ntfsdrv_bitmap_clear_run(na, bit, 1);
}

/*
 * rol32 - rotate a 32-bit value left
 *
 * @word: value to rotate
 * @shift: number of bits to rotate by
 */
static __inline__ u32 ntfsdrv_rol32(u32 word, unsigned int shift)
{
        return (word << shift) | (word >> (32 - shift));
}

/*
 * ror32 - rotate a 32-bit value right
 *
 * @word: value to rotate
 * @shift: number of bits to rotate by
 */
static __inline__ u32 ntfsdrv_ror32(u32 word, unsigned int shift)
{
        return (word >> shift) | (word << (32 - shift));
}

#endif /* defined NTFSDRV_NTFS_BITMAP_MGR_H */

