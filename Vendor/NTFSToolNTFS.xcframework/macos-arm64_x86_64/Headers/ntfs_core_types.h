/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_CORE_TYPES_H
#define NTFSDRV_NTFS_CORE_TYPES_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_STDINT_H || !HAVE_CONFIG_H
#include <stdint.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

typedef uint8_t  u8;			/* exact-size unsigned types */
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;			/* exact-size signed types */
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef u16 le16;
typedef u32 le32;
typedef u64 le64;

typedef u16 be16;
typedef u32 be32;
typedef u64 be64;

/*
 * Declare s{l,b}e{16,32,64} as unsigned,
 * because sign extension is not wanted on big-endian architectures.
 */
typedef u16 sle16;
typedef u32 sle32;
typedef u64 sle64;

typedef u16 sbe16;
typedef u32 sbe32;
typedef u64 sbe64;

typedef le16 ntfschar;			/* 2-byte Unicode character type. */
#define UCHAR_T_SIZE_BITS 1

/*
 * Clusters on an NTFS volume are 64-bit signed values. The LCN and VCN
 * types are defined to support type checking and improve readability.
 */
typedef s64 VCN;
typedef sle64 leVCN;
typedef s64 LCN;
typedef sle64 leLCN;

/*
 * The NTFS log file $LogFile uses log sequence numbers,
 * which are 64-bit signed values. The LSN type is defined to support type checking
 * and improve readability.
 */
typedef s64 LSN;
typedef sle64 leLSN;

/*
 * Cygwin's BOOL conflicts with the one in <windef.h>.
 * This is fine as long as this file is included after <windows.h>.
 */
#ifndef _WINDEF_H
/**
 * enum BOOL - for code readability only.
 */
typedef enum {
#ifndef FALSE
	FALSE = 0,
#endif
#ifndef NO
	NO = 0,
#endif
#ifndef ZERO
	ZERO = 0,
#endif
#ifndef TRUE
	TRUE = 1,
#endif
#ifndef YES
	YES = 1,
#endif
#ifndef ONE
	ONE = 1,
#endif
} BOOL;
#endif /* defined _WINDEF_H */

/**
 * enum IGNORE_CASE_BOOL -
 */
typedef enum {
	CASE_SENSITIVE = 0,
	IGNORE_CASE = 1,
} IGNORE_CASE_BOOL;

#define STATUS_OK				(0)
#define STATUS_ERROR				(-1)
#define STATUS_RESIDENT_ATTRIBUTE_FILLED_MFT	(-2)
#define STATUS_KEEP_SEARCHING			(-3)
#define STATUS_NOT_FOUND			(-4)

/*
 *	Force structure alignment when the processor requires it
 */
union ALIGNMENT {
	u64 u64align;
	void *ptralign;
} ;

#endif /* defined NTFSDRV_NTFS_CORE_TYPES_H */
