/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_FS_LAYOUT_H
#define NTFSDRV_NTFS_FS_LAYOUT_H

#include "ntfs_core_types.h"
#include "ntfs_endian_utils.h"
#include "ntfs_platform_support.h"

/* NTFS OEM identifier */
#define magicNTFS	const_cpu_to_le64(0x202020205346544e)	/* "NTFS    " */
#define NTFS_SB_MAGIC	0x5346544e				/* 'NTFS' */

/*
 * Location of the boot sector on the partition:
 * The standard NTFS_BOOT_SECTOR is at sector 0 of the partition.
 * On NT4 and later there is a backup copy in the last sector of the partition (normally not accessible from within Windows,
 * because the sector count recorded in the boot sector is one less than the real value).
 * On NT 3.51 and earlier, the backup is at sector count / 2 (integer division), i.e. in the middle of the volume.
 */

/**
 * struct BIOS_PARAMETER_BLOCK - BIOS Parameter Block (BPB) structure.
 */
typedef struct {
	le16 bytes_per_sector;		/* sector size in bytes. */
	u8  sectors_per_cluster;	/* sectors per cluster. */
	le16 reserved_sectors;		/* zero */
	u8  fats;			/* zero */
	le16 root_entries;		/* zero */
	le16 sectors;			/* zero */
	u8  media_type;			/* 0xf8 = hard disk */
	le16 sectors_per_fat;		/* zero */
/*0x0d*/le16 sectors_per_track;		/* required for Windows boot. */
/*0x0f*/le16 heads;			/* required for Windows boot. */
/*0x11*/le32 hidden_sectors;		/* sector offset of the partition start relative to the disk.
					   Required for Windows boot. */
/*0x15*/le32 large_sectors;		/* zero */
/* sizeof() = 25 (0x19) bytes */
} __attribute__((__packed__)) BIOS_PARAMETER_BLOCK;

/**
 * struct NTFS_BOOT_SECTOR - NTFS boot sector structure.
 */
typedef struct {
	u8  jump[3];			/* jump instruction (jumps to the boot code). */
	le64 oem_id;			/* magic "NTFS    ". */
/*0x0b*/BIOS_PARAMETER_BLOCK bpb;	/* See BIOS_PARAMETER_BLOCK. */
	u8 physical_drive;		/* 0x00 floppy, 0x80 hard disk */
	u8 current_head;		/* zero */
	u8 extended_boot_signature; 	/* 0x80 */
	u8 reserved2;			/* zero */
/*0x28*/sle64 number_of_sectors;		/* total number of sectors in the volume. The maximum volume size is
					   2^63 sectors. With standard 512-byte sectors,
					   that is about 4.7×10^21 bytes. */
	sle64 mft_lcn;			/* cluster location of the MFT data. */
	sle64 mftmirr_lcn;		/* cluster location of the MFT mirror. */
	s8  clusters_per_mft_record;	/* MFT record size (in clusters). */
	u8  reserved0[3];		/* zero */
	s8  clusters_per_index_record;	/* index block size (in clusters). */
	u8  reserved1[3];		/* zero */
	le64 volume_serial_number;	/* volume serial number. */
	le32 checksum;			/* boot sector checksum. */
/*0x54*/u8  bootstrap[426];		/* boot code. */
	le16 end_of_sector_marker;	/* end-of-sector marker of the boot sector, always
					   0xaa55 in little-endian. */
/* sizeof() = 512 (0x200) bytes */
} __attribute__((__packed__)) NTFS_BOOT_SECTOR;

/**
 * enum NTFS_RECORD_TYPES - NTFS record type magic numbers.
 *
 * Magic identifiers at the start of all NTFS structures that contain records (such as MFT records).
 */
typedef enum {
	/* Found in $MFT/$DATA. */
	magic_FILE = const_cpu_to_le32(0x454c4946), /* MFT entry. */
	magic_INDX = const_cpu_to_le32(0x58444e49), /* Index buffer. */
	magic_HOLE = const_cpu_to_le32(0x454c4f48), /* NTFS 3.0+ */

	/* Found in $LogFile/$DATA. */
	magic_RSTR = const_cpu_to_le32(0x52545352), /* Restart page. */
	magic_RCRD = const_cpu_to_le32(0x44524352), /* Log record page. */

	/* Found in $LogFile/$DATA (and possibly also in $MFT/$DATA). */
	magic_CHKD = const_cpu_to_le32(0x444b4843), /* Modified by chkdsk. */

	/* Found in all NTFS structures that contain records. */
	magic_BAAD = const_cpu_to_le32(0x44414142), /* Multi-sector transfer failure
						       detected. */

	/*
	 * Found in $LogFile/$DATA when a page is filled entirely with 0xff bytes,
	 * meaning it is uninitialized. The page must be initialized before use.
	 */
	magic_empty = const_cpu_to_le32(0xffffffff),/* Record is empty and must be
						       initialized before use. */
} NTFS_RECORD_TYPES;

/*
 * Generic magic number comparison macro.
 */
#define ntfsdrv_is_magic(x, m)	(   (u32)(x) == (u32)magic_##m )
#define ntfsdrv_is_magicp(p, m)	( *(u32*)(p) == (u32)magic_##m )

/*
 * Specialized magic number comparison macros for the NTFS_RECORD_TYPES above.
 */
#define ntfsdrv_is_file_record(x)	( ntfsdrv_is_magic (x, FILE) )
#define ntfsdrv_is_file_recordp(p)	( ntfsdrv_is_magicp(p, FILE) )
#define ntfsdrv_is_mft_record(x)	( ntfsdrv_is_file_record(x) )
#define ntfsdrv_is_mft_recordp(p)	( ntfsdrv_is_file_recordp(p) )
#define ntfsdrv_is_indx_record(x)	( ntfsdrv_is_magic (x, INDX) )
#define ntfsdrv_is_indx_recordp(p)	( ntfsdrv_is_magicp(p, INDX) )
#define ntfsdrv_is_hole_record(x)	( ntfsdrv_is_magic (x, HOLE) )
#define ntfsdrv_is_hole_recordp(p)	( ntfsdrv_is_magicp(p, HOLE) )

#define ntfsdrv_is_rstr_record(x)	( ntfsdrv_is_magic (x, RSTR) )
#define ntfsdrv_is_rstr_recordp(p)	( ntfsdrv_is_magicp(p, RSTR) )
#define ntfsdrv_is_rcrd_record(x)	( ntfsdrv_is_magic (x, RCRD) )
#define ntfsdrv_is_rcrd_recordp(p)	( ntfsdrv_is_magicp(p, RCRD) )

#define ntfsdrv_is_chkd_record(x)	( ntfsdrv_is_magic (x, CHKD) )
#define ntfsdrv_is_chkd_recordp(p)	( ntfsdrv_is_magicp(p, CHKD) )

#define ntfsdrv_is_baad_record(x)	( ntfsdrv_is_magic (x, BAAD) )
#define ntfsdrv_is_baad_recordp(p)	( ntfsdrv_is_magicp(p, BAAD) )

#define ntfsdrv_is_empty_record(x)		( ntfsdrv_is_magic (x, empty) )
#define ntfsdrv_is_empty_recordp(p)	( ntfsdrv_is_magicp(p, empty) )

/*
 * Logical sector size in bytes, used as the sequence number stride for multi-sector transfers.
 * Should be less than or equal to the physical sector size; if it is larger than the physical sector size,
 * incomplete multi-sector transfers may go undetected.
 */
#define NTFS_BLOCK_SIZE		512
#define NTFS_BLOCK_SIZE_BITS	9

/**
 * struct NTFS_RECORD - Generic NTFS record header.
 *
 * The update sequence array (USA) is an array of le16 values, one at the end of each sector protected by
 * the update-sequenced record. The first entry is the update sequence number (USN), a cyclic counter of the number of times
 * the protected record has been written to disk. The values 0 and -1 (0xffff) are not used. When reading, the last le16 of each sector must equal the USN;
 * when writing, it is set to the USN. A mismatch indicates an incomplete multi-sector transfer.
 * The maximum size of the update sequence array is fixed at:
 *	max size = usa_ofs + (usa_count * 2) = 510 bytes
 * The 510 bytes come from the array's last le16 having to end before the last le16
 * of the first 512-byte sector. usa_ofs + (usa_count * 2) must be less than or equal to 510, usable as a consistency check.
 */
typedef struct {
	NTFS_RECORD_TYPES magic;/* four-byte magic number identifying the record type and/or state. */
	le16 usa_ofs;		/* byte offset of the update sequence array (USA) relative to
				   the start of the record. */
	le16 usa_count;		/* number of le16 entries in the USA (including the USN),
				   so the number of fixups is usa_count - 1. */
} __attribute__((__packed__)) NTFS_RECORD;

/**
 * enum NTFS_SYSTEM_FILES - MFT record numbers of the system files.
 *
 * All system files are always marked as in use in the MFT bitmap attribute,
 * to avoid being accidentally allocated to other MFT records. The sequence number of each system file
 * is always equal to its MFT record number and is never modified.
 */
typedef enum {
	FILE_MFT	= 0,	/* Master file table (MFT). The data attribute contains all entries,
				   and the bitmap attribute records which entries are in use
				   (bit==1 means in use). */
	FILE_MFTMirr	= 1,	/* MFT mirror: the data attribute stores copies of the first four MFT records.
				   If the cluster size is > 4KiB, the first N records are stored
				   (N = cluster size / MFT record size). */
	FILE_LogFile	= 2,	/* The log in the data attribute. */
	FILE_Volume	= 3,	/* Volume name attribute and volume information attribute (flags and NTFS version).
				   Windows calls this file the volume DASD (direct access
				   storage device). */
	FILE_AttrDef	= 4,	/* The attribute definition array in the data attribute. */
	FILE_root	= 5,	/* The root directory. */
	FILE_Bitmap	= 6,	/* The allocation bitmap of all clusters (LCN) in the data attribute. */
	FILE_Boot	= 7,	/* The boot sector in the data attribute (always at cluster 0). */
	FILE_BadClus	= 8,	/* The non-resident data attribute contains all bad clusters. */
	FILE_Secure	= 9,	/* Shared security descriptors and two indexes in the data attribute.
				   Appeared in Windows 2000. Previously this file was named
				   $Quota but unused. */
	FILE_UpCase	= 10,	/* The uppercase equivalence mapping table of all 65536 Unicode characters
				   in the data attribute. */
	FILE_Extend	= 11,	/* A directory containing other system files
				   (such as $ObjId, $Quota, $Reparse, $UsnJrnl).
				   New in NTFS 3.0. */
	FILE_reserved12	= 12,	/* Reserved (records 12-15). */
	FILE_reserved13	= 13,
	FILE_reserved14	= 14,
	FILE_mft_data	= 15,	/* Reserved for the first extent of $MFT:$DATA. */
	FILE_first_user	= 16,	/* The first user file, used as the boundary for deciding whether
				   a file may be opened. */
} NTFS_SYSTEM_FILES;

/**
 * enum MFT_RECORD_FLAGS - MFT record flags (16 bits).
 *
 * Flag bits describing MFT record information.
 *
 * MFT_RECORD_IS_4 is present on all $Extend subfiles,
 * and seems to mark metadata files with MFT record number > 24.
 *
 * MFT_RECORD_IS_VIEW_INDEX is present on every metadata file that has a non-directory index,
 * i.e. with an INDEX_ROOT and INDEX_ALLOCATION whose name is not "$I30".
 */
typedef enum {
	MFT_RECORD_IN_USE		= const_cpu_to_le16(0x0001),
	MFT_RECORD_IS_DIRECTORY		= const_cpu_to_le16(0x0002),
	MFT_RECORD_IS_4			= const_cpu_to_le16(0x0004),
	MFT_RECORD_IS_VIEW_INDEX	= const_cpu_to_le16(0x0008),
	MFT_REC_SPACE_FILLER		= 0xffff, /* Only used to force the flags to be
						     16 bits wide. */
} __attribute__((__packed__)) MFT_RECORD_FLAGS;

/*
 * MFT reference (also called file reference or file record segment reference), used when a structure needs to refer to a
 * record in the MFT.
 *
 * A reference consists of a 48-bit MFT index and a 16-bit sequence number, the latter used to detect stale references.
 *
 * For easier error reporting, the 48-bit index is treated as signed.
 *
 * The sequence number is a cyclic counter (skipping 0) describing the number of times the referenced MFT record has been
 * (re)used. It must match the sequence number of the referenced MFT record, otherwise the reference is considered stale and is removed.
 *
 * When the sequence number is zero, no consistency check is performed.
 *
 * FIXME: Since inodes are currently 32-bit, the driver needs to always check that high_part is 0.
 * This is not a problem in practice, because the MFT records alone would need 4TB of space to exceed the 32-bit range.
 *
 * FIXME: The MFT zone is defined as the first 12% of the volume. This space is reserved so that the MFT can grow contiguously.
 * When the free 88% of the volume is used up, the MFT zone is halved to free up more space. This process repeats
 * when there is no more free space.
 */

/*
 * Define MFT_REF as a 64-bit value for ease of handling.
 * Two unpacking macros are also defined to obtain the reference (MREF) and the sequence number (MSEQNO).
 * The _LE versions are for little-endian MFT_REF and return values in CPU endian format.
 */
#define MFT_REF_MASK_CPU 0x0000ffffffffffffULL
#define MFT_REF_MASK_LE const_cpu_to_le64(MFT_REF_MASK_CPU)

typedef u64 MFT_REF;
typedef le64 leMFT_REF;   /* little-endian MFT_REF */

#define MK_MREF(m, s)	((MFT_REF)(((MFT_REF)(s) << 48) |		\
					((MFT_REF)(m) & MFT_REF_MASK_CPU)))
#define MK_LE_MREF(m, s) const_cpu_to_le64(((MFT_REF)(((MFT_REF)(s) << 48) | \
					((MFT_REF)(m) & MFT_REF_MASK_CPU))))

#define MREF(x)		((u64)((x) & MFT_REF_MASK_CPU))
#define MSEQNO(x)	((u16)(((x) >> 48) & 0xffff))
#define MREF_LE(x)	((u64)(const_le64_to_cpu(x) & MFT_REF_MASK_CPU))
#define MSEQNO_LE(x)	((u16)((const_le64_to_cpu(x) >> 48) & 0xffff))

#define IS_ERR_MREF(x)	(((x) & 0x0000800000000000ULL) ? 1 : 0)
#define ERR_MREF(x)	((u64)((s64)(x)))
#define MREF_ERR(x)	((int)((s64)(x)))

/**
 * struct MFT_RECORD - MFT record layout (NTFS 3.1+)
 *
 * The record header at the start of each MFT record, followed by a sequence of variable-length attribute records,
 * terminated by an attribute of type AT_END (which is truncated and contains only the type code).
 */
typedef struct {
/*Ofs*/
/*  0	NTFS_RECORD; -- expanded because gcc does not support anonymous structs.*/
	NTFS_RECORD_TYPES magic;/* magic number, usually "FILE". */
	le16 usa_ofs;		/* See the NTFS_RECORD definition above. */
	le16 usa_count;		/* See the NTFS_RECORD definition above. */

/*  8*/	leLSN lsn;		/* $LogFile sequence number of this record.
				   Updated every time the record is modified. */
/* 16*/	le16 sequence_number;	/* number of times this MFT record has been reused.
				   (See the MFT_REF description above.)
				   Note: the increment (skipping zero) happens when the file is deleted.
				   Note: if zero, it stays zero. */
/* 18*/	le16 link_count;		/* number of hard links, i.e. the number of directory entries referring to this record.
				   Note: only used in base MFT records.
				   Note: when a directory entry is deleted, this value is checked; if it is 1,
				   the file is deleted, otherwise the referenced FILE_NAME_ATTR is removed
				   from the MFT record and this value is decremented. */
/* 20*/	le16 attrs_offset;	/* byte offset of the first attribute relative to the start of the MFT record.
				   Note: must be 8-byte aligned. */
/* 22*/	MFT_RECORD_FLAGS flags;	/* MFT_RECORD_FLAGS bit array. When a file is deleted, the
				   MFT_RECORD_IN_USE flag is set to zero. */
/* 24*/	le32 bytes_in_use;	/* number of bytes in use in this MFT record.
				   Note: must be 8-byte aligned. */
/* 28*/	le32 bytes_allocated;	/* number of bytes allocated for this MFT record,
				   should equal the MFT record size. */
/* 32*/	leMFT_REF base_mft_record;
				/* This is zero in a base record. When non-zero, it is a reference to
				   the base MFT record it belongs to (used to locate the attribute list
				   attribute in the base record, which describes this extension record
				   and may need to be updated in sync when the extension
				   record is modified). */
/* 40*/	le16 next_attr_instance; /* instance number that will be assigned to the next newly added attribute.
				   Note: incremented after each use.
				   Note: reset to zero when the MFT record is reused.
				   Note: the first instance number is always 0. */
/* The following fields are NTFS 3.1+ only (Windows XP and later): */
/* 42*/ le16 reserved;		/* reserved/alignment. */
/* 44*/ le32 mft_record_number;	/* number of this MFT record. */
/* sizeof() = 48 bytes */
/*
 * When an MFT record is (re)used, the update sequence array is placed at this offset, i.e. before the attributes.
 * This also makes sense, otherwise the update sequence array itself could contain the last two bytes of a sector,
 * defeating multi-sector transfer protection. When reading, use the data in the NTFS record header.
 */
} __attribute__((__packed__)) MFT_RECORD;

/**
 * struct MFT_RECORD_OLD - MFT record layout (NTFS <=3.0)
 *
 * The version without the NTFS 3.1+ specific fields.
 */
typedef struct {
/*Ofs*/
/*  0	NTFS_RECORD; -- expanded because gcc does not support anonymous structs.*/
	NTFS_RECORD_TYPES magic;/* magic number, usually "FILE". */
	le16 usa_ofs;		/* See the NTFS_RECORD definition above. */
	le16 usa_count;		/* See the NTFS_RECORD definition above. */

/*  8*/	leLSN lsn;		/* $LogFile sequence number of this record.
				   Updated every time the record is modified. */
/* 16*/	le16 sequence_number;	/* number of times this MFT record has been reused.
				   Note: the increment (skipping zero) happens when the file is deleted.
				   Note: if zero, it stays zero. */
/* 18*/	le16 link_count;		/* number of hard links, i.e. the number of directory entries referring to this record.
				   Note: only used in base MFT records.
				   Note: when a directory entry is deleted, this value is checked; if it is 1,
				   the file is deleted, otherwise the corresponding
				   FILE_NAME_ATTR is removed and this value is decremented. */
/* 20*/	le16 attrs_offset;	/* byte offset of the first attribute relative to the start of the MFT record.
				   Note: must be 8-byte aligned. */
/* 22*/	MFT_RECORD_FLAGS flags;	/* MFT_RECORD_FLAGS bit array. When a file is deleted, the
				   MFT_RECORD_IN_USE flag is set to zero. */
/* 24*/	le32 bytes_in_use;	/* number of bytes in use in this MFT record.
				   Note: must be 8-byte aligned. */
/* 28*/	le32 bytes_allocated;	/* number of bytes allocated for this MFT record,
				   should equal the MFT record size. */
/* 32*/	leMFT_REF base_mft_record;
				/* This is zero in a base record. When non-zero, it is a reference to
				   the base MFT record it belongs to (used to locate the attribute list
				   attribute in the base record, which describes this extension record
				   and may need to be updated in sync when the extension
				   record is modified). */
/* 40*/	le16 next_attr_instance; /* instance number that will be assigned to the next newly added attribute.
				   Note: incremented after each use.
				   Note: reset to zero when the MFT record is reused.
				   Note: the first instance number is always 0. */
/* sizeof() = 42 bytes */
/*
 * When an MFT record is (re)used, the update sequence array is placed at this offset, i.e. before the attributes.
 * This also makes sense, otherwise the update sequence array itself could contain the last two bytes of a sector,
 * defeating multi-sector transfer protection. When reading, use the data in the NTFS record header.
 */
} __attribute__((__packed__)) MFT_RECORD_OLD;

/**
 * enum ATTR_TYPES - System-defined attribute types (32 bits).
 *
 * Each attribute type has a corresponding attribute name (a Unicode string of at most 64 characters),
 * described by the attribute definition in the data attribute of the $AttrDef system file.
 *
 * On NTFS 3.0 volumes the attribute name is the type name in the enum below, with AT_ replaced by $.
 */
typedef enum {
	AT_UNUSED			= const_cpu_to_le32(         0),
	AT_STANDARD_INFORMATION		= const_cpu_to_le32(      0x10),
	AT_ATTRIBUTE_LIST		= const_cpu_to_le32(      0x20),
	AT_FILE_NAME			= const_cpu_to_le32(      0x30),
	AT_OBJECT_ID			= const_cpu_to_le32(      0x40),
	AT_SECURITY_DESCRIPTOR		= const_cpu_to_le32(      0x50),
	AT_VOLUME_NAME			= const_cpu_to_le32(      0x60),
	AT_VOLUME_INFORMATION		= const_cpu_to_le32(      0x70),
	AT_DATA				= const_cpu_to_le32(      0x80),
	AT_INDEX_ROOT			= const_cpu_to_le32(      0x90),
	AT_INDEX_ALLOCATION		= const_cpu_to_le32(      0xa0),
	AT_BITMAP			= const_cpu_to_le32(      0xb0),
	AT_REPARSE_POINT		= const_cpu_to_le32(      0xc0),
	AT_EA_INFORMATION		= const_cpu_to_le32(      0xd0),
	AT_EA				= const_cpu_to_le32(      0xe0),
	AT_PROPERTY_SET			= const_cpu_to_le32(      0xf0),
	AT_LOGGED_UTILITY_STREAM	= const_cpu_to_le32(     0x100),
	AT_FIRST_USER_DEFINED_ATTRIBUTE	= const_cpu_to_le32(    0x1000),
	AT_END				= const_cpu_to_le32(0xffffffff),
} ATTR_TYPES;

/**
 * enum COLLATION_RULES - Collation rules for views/indexes (32 bits).
 *
 * COLLATION_BINARY - Sort by binary comparison, first byte most significant.
 * COLLATION_FILE_NAME - Compare Unicode strings by 16-bit code units,
 *	preferably ignoring case using the volume's $UpCase table, falling back to a case-sensitive comparison on equality.
 * COLLATION_UNICODE_STRING - TODO: not yet implemented.
 * COLLATION_NTOFS_ULONG - Sort ascending by le32 key value.
 *	For example used for the $SII index in FILE_Secure (sorted by security_id).
 * COLLATION_NTOFS_SID - Sort ascending by SID value.
 *	For example used for the $O index in FILE_Extend/$Quota.
 * COLLATION_NTOFS_SECURITY_HASH - Sort ascending by hash value first, then ascending by security_id.
 *	For example used for the $SDH index in FILE_Secure.
 * COLLATION_NTOFS_ULONGS - Sort ascending by a sequence of le32 key values.
 *	For example used for the $O index in FILE_Extend/$ObjId (sorted by object_id),
 *	splitting the 16-byte object_id into four le32 values used as separate keys.
 */
typedef enum {
	COLLATION_BINARY		= const_cpu_to_le32(0),
	COLLATION_FILE_NAME		= const_cpu_to_le32(1),
	COLLATION_UNICODE_STRING	= const_cpu_to_le32(2),
	COLLATION_NTOFS_ULONG		= const_cpu_to_le32(16),
	COLLATION_NTOFS_SID		= const_cpu_to_le32(17),
	COLLATION_NTOFS_SECURITY_HASH	= const_cpu_to_le32(18),
	COLLATION_NTOFS_ULONGS		= const_cpu_to_le32(19),
} COLLATION_RULES;

/**
 * enum ATTR_DEF_FLAGS - Flags in the attribute definition structure describing the nature of an attribute (32 bits).
 *
 * The INDEXABLE flag should be correct, since only the file name attribute has it set,
 * and in NT4 it was the only indexed attribute.
 */
typedef enum {
	ATTR_DEF_INDEXABLE	= const_cpu_to_le32(0x02), /* The attribute can be indexed. */
	ATTR_DEF_MULTIPLE	= const_cpu_to_le32(0x04), /* Can appear multiple times in the
					MFT records of the same inode. */
	ATTR_DEF_NOT_ZERO	= const_cpu_to_le32(0x08), /* The attribute value must contain
					at least one non-zero byte. */
	ATTR_DEF_INDEXED_UNIQUE	= const_cpu_to_le32(0x10), /* Must be indexed and the attribute value
					must be unique across all MFT records of the
					same inode. */
	ATTR_DEF_NAMED_UNIQUE	= const_cpu_to_le32(0x20), /* Must have a name and the name
					must be unique across all MFT records of the
					same inode. */
	ATTR_DEF_RESIDENT	= const_cpu_to_le32(0x40), /* Must be a resident attribute. */
	ATTR_DEF_ALWAYS_LOG	= const_cpu_to_le32(0x80), /* Always log modifications of this attribute
					to the log, whether resident or not.
					Without this flag, only modifications of
					resident attributes are logged. */
} ATTR_DEF_FLAGS;

/**
 * struct ATTR_DEF - Attribute definition structure.
 *
 * The data attribute of FILE_AttrDef contains a sequence of attribute definitions for the NTFS volume.
 * Entries are sorted by attribute type. The flags describe whether an attribute can be resident/non-resident, etc.
 */
typedef struct {
/*hex ofs*/
/*  0*/	ntfschar name[0x40];		/* Unicode name of the attribute, zero-terminated. */
/* 80*/	ATTR_TYPES type;		/* attribute type. */
/* 84*/	le32 display_rule;		/* default display rule. */
/* 88*/ COLLATION_RULES collation_rule;	/* default collation rule. */
/* 8c*/	ATTR_DEF_FLAGS flags;		/* flags describing the attribute. */
/* 90*/	sle64 min_size;			/* optional minimum attribute size. */
/* 98*/	sle64 max_size;			/* maximum size of the attribute. */
/* sizeof() = 0xa0, i.e. 160 bytes */
} __attribute__((__packed__)) ATTR_DEF;

/**
 * enum ATTR_FLAGS - Attribute flags (16 bits).
 */
typedef enum {
	ATTR_IS_COMPRESSED	= const_cpu_to_le16(0x0001),
	ATTR_COMPRESSION_MASK	= const_cpu_to_le16(0x00ff),  /* Mask of the compression method,
						also the first invalid value. */
	ATTR_IS_ENCRYPTED	= const_cpu_to_le16(0x4000),
	ATTR_IS_SPARSE		= const_cpu_to_le16(0x8000),
} __attribute__((__packed__)) ATTR_FLAGS;

/*
 * Attribute compression.
 *
 * In the current Windows NTFS driver only the data attribute is compressed, and only when the data attribute
 * is non-resident. When compression is used, the maximum allowed cluster size of the volume is 4KiB.
 *
 * The compression method is based on independently compressing blocks of X clusters, where X is determined by the
 * compression_unit value in the non-resident attribute record header (precisely, X = 2^compression_unit clusters).
 * On Windows NT/2k, X is always 16 clusters (compression_unit = 4).
 *
 * A compressed block can be stored in three ways:
 *
 *   1) All data in the block is zero (a sparse block):
 *	  Stored in the runlist as a sparse block, i.e. entry length = X and lcn = -1.
 *	  The delta_lcn length in the mapping pairs array is 0 (absent),
 *	  which the driver interprets as lcn = -1.
 *	  Note: on NTFS 3.0 volumes, uncompressed files can also be sparse.
 *
 *   2) The data in the block is not compressed:
 *	  Happens when compression fails to reduce the number of clusters the block occupies. The runlist entry
 *	  has length = X and lcn >= 0.
 *
 *   3) The data in the block is compressed:
 *	  The common case. The runlist entry has length L < X and lcn >= 0, followed by
 *	  a sparse entry with length = X - L and lcn = -1, filling up the compressed block size X.
 *
 * In practice it is more complicated, because adjacent entries of the same type can be merged. The number of clusters processed
 * must be tracked in blocks of X clusters. For example, length L > X means this entry contains one block of length X
 * and part of one or more blocks of length L - X.
 *
 * Note: at the end of a compressed attribute value there may not be exactly enough data to form a complete compressed block;
 * no attempt is made to compress such data unless compression really reduces the number of clusters occupied.
 */

/**
 * enum RESIDENT_ATTR_FLAGS - Resident attribute flags (8 bits).
 */
typedef enum {
	RESIDENT_ATTR_IS_INDEXED = 0x01, /* The attribute is referenced in an index
					    (affects delete and modify operations). */
} __attribute__((__packed__)) RESIDENT_ATTR_FLAGS;

/**
 * struct ATTR_RECORD - Attribute record header.
 *
 * Always 8-byte aligned.
 */
typedef struct {
/*Ofs*/
/*  0*/	ATTR_TYPES type;	/* 32-bit type code of the attribute. */
/*  4*/	le32 length;		/* size in bytes of the resident part of the attribute
				   (8-byte aligned), used to locate the next attribute. */
/*  8*/	u8 non_resident;	/* 0 = resident attribute, 1 = non-resident attribute. */
/*  9*/	u8 name_length;		/* number of Unicode characters in the attribute name, 0 = unnamed. */
/* 10*/	le16 name_offset;	/* if name_length != 0, the byte offset of the name relative to the start of
				   the attribute record. The name is stored as a Unicode
				   string. On creation the offset is set at the end of the
				   record header, followed by the attribute value or the mapping pairs
				   array (resident or non-resident), 8-byte aligned. */
/* 12*/	ATTR_FLAGS flags;	/* flags describing the attribute. */
/* 14*/	le16 instance;		/* instance number of this attribute record, unique within the MFT record
				   (see MFT_RECORD/next_attribute_instance above for details). */
/* 16*/	union {
		/* Resident attribute. */
		struct {
/* 16 */		le32 value_length; /* size of the attribute value in bytes. */
/* 20 */		le16 value_offset; /* byte offset of the attribute value relative to the start of
					       the attribute record. On creation,
					       8-byte aligned if there is a name. */
/* 22 */		RESIDENT_ATTR_FLAGS resident_flags; /* see above. */
/* 23 */		s8 reservedR;	    /* reserved/8-byte alignment. */
/* 24 */		void *resident_end[0]; /* use offsetof(ATTR_RECORD,
						  resident_end) to get the resident
						  attribute size. */
		} __attribute__((__packed__));
		/* Non-resident attribute. */
		struct {
/* 16*/			leVCN lowest_vcn;	/* lowest valid VCN of this part of the attribute value,
				0 when there is only one extent (usually the case).
				lowest_vcn != 0 only when an attribute list is used. */
/* 24*/			leVCN highest_vcn; /* highest valid VCN of this extent.
				Usually equals the number of clusters of the attribute value minus 1. -1 for a zero-length file,
				may be 0 for a "single extent" attribute. */
/* 32*/			le16 mapping_pairs_offset; /* byte offset of the mapping pairs array
				relative to the start of the structure, containing the VCN to
				LCN mapping. On creation, 8-byte aligned at the end of the record header. */
/* 34*/			u8 compression_unit; /* compression unit, expressed as the base-2 logarithm
				of the number of clusters. 0 = uncompressed. WinNT4 only uses the value 4. */
/* 35*/			u8 reserved1[5];	/* 8-byte alignment. */
/* The following size fields are only used when lowest_vcn is zero. */
/* 40*/			sle64 allocated_size;	/* number of bytes of disk
				space allocated to store the attribute value, always a multiple of the cluster size.
				In compressed files this is a multiple of the compression block size,
				representing the logical allocated space rather than actual disk usage
				(see compressed_size for actual usage). */
/* 48*/			sle64 data_size;	/* size of the attribute value in bytes. Compressed or
				sparse attributes can be larger than allocated_size. */
/* 56*/			sle64 initialized_size;	/* size in bytes of the initialized part of the
				attribute value, usually equal to data_size. */
/* 64 */		void *non_resident_end[0]; /* use offsetof(ATTR_RECORD,
						      non_resident_end) to get the
						      non-resident attribute size. */
/* sizeof(uncompressed attribute) = 64*/
/* 64*/			sle64 compressed_size;	/* size of the compressed attribute value in bytes.
				Only present for compressed attributes. Always a multiple of the cluster size,
				representing actual disk usage. */
/* 72 */		void *compressed_end[0];
				/* use offsetof(ATTR_RECORD, compressed_end)
				   to get the compressed attribute size. */
/* sizeof(compressed attribute) = 72*/
		} __attribute__((__packed__));
	} __attribute__((__packed__));
} __attribute__((__packed__)) ATTR_RECORD;

typedef ATTR_RECORD ATTR_REC;

/**
 * enum FILE_ATTR_FLAGS - File attribute flags (32 bits).
 */
typedef enum {
	/*
	 * These flags only exist in the
	 * file_attributes field of the STANDARD_INFORMATION attribute.
	 */
	FILE_ATTR_READONLY		= const_cpu_to_le32(0x00000001),
	FILE_ATTR_HIDDEN		= const_cpu_to_le32(0x00000002),
	FILE_ATTR_SYSTEM		= const_cpu_to_le32(0x00000004),
	/* Old DOS volume label. Unused in NT. = const_cpu_to_le32(0x00000008), */

	FILE_ATTR_DIRECTORY		= const_cpu_to_le32(0x00000010),
	/* FILE_ATTR_DIRECTORY is not considered valid in NT, reserved for the DOS subdirectory flag.*/
	FILE_ATTR_ARCHIVE		= const_cpu_to_le32(0x00000020),
	FILE_ATTR_DEVICE		= const_cpu_to_le32(0x00000040),
	FILE_ATTR_NORMAL		= const_cpu_to_le32(0x00000080),

	FILE_ATTR_TEMPORARY		= const_cpu_to_le32(0x00000100),
	FILE_ATTR_SPARSE_FILE		= const_cpu_to_le32(0x00000200),
	FILE_ATTR_REPARSE_POINT		= const_cpu_to_le32(0x00000400),
	FILE_ATTR_COMPRESSED		= const_cpu_to_le32(0x00000800),

	FILE_ATTR_OFFLINE		= const_cpu_to_le32(0x00001000),
	FILE_ATTR_NOT_CONTENT_INDEXED	= const_cpu_to_le32(0x00002000),
	FILE_ATTR_ENCRYPTED		= const_cpu_to_le32(0x00004000),
	FILE_ATTRIBUTE_RECALL_ON_OPEN	= const_cpu_to_le32(0x00040000),

	FILE_ATTR_VALID_FLAGS		= const_cpu_to_le32(0x00047fb7),
	/* FILE_ATTR_VALID_FLAGS masks the old DOS volume label and FILE_ATTR_DEVICE,
	   keeping all other flags. Used to obtain all flags that are valid when reading. */
	FILE_ATTR_VALID_SET_FLAGS	= const_cpu_to_le32(0x000031a7),
	/* FILE_ATTR_VALID_SET_FLAGS masks the old DOS volume label, DEVICE, DIRECTORY,
	   SPARSE_FILE, REPARSE_POINT, COMPRESSED and ENCRYPTED,
	   keeping the rest. Used to obtain all flags that are valid when setting. */

	/**
	 * FILE_ATTR_I30_INDEX_PRESENT - Is it a directory?
	 *
	 * The MFT_RECORD_IS_DIRECTORY bit copied from the MFT record, indicating whether it is a directory.
	 * i.e. whether it has an index root attribute named "$I30".
	 * This flag only exists in the file_attributes field of the FILE_NAME attribute.
	 */
	FILE_ATTR_I30_INDEX_PRESENT	= const_cpu_to_le32(0x10000000),

	/**
	 * FILE_ATTR_VIEW_INDEX_PRESENT - Does it have a non-directory index?
	 *
	 * The MFT_RECORD_IS_VIEW_INDEX bit copied from the MFT record, indicating whether the file
	 * has a view index (such as the object id index, quota index, security index,
	 * reparse point index).
	 * This flag only exists in the $STANDARD_INFORMATION and $FILE_NAME attributes.
	 */
	FILE_ATTR_VIEW_INDEX_PRESENT	= const_cpu_to_le32(0x20000000),
} __attribute__((__packed__)) FILE_ATTR_FLAGS;

/*
 * NTFS time note: all times are in Microsoft standard time format, i.e. the number of 100-nanosecond intervals since January 1, 1601
 * 00:00:00 UTC.
 */

/**
 * struct STANDARD_INFORMATION - Attribute: standard information (0x10).
 *
 * Note: always resident.
 * Note: present in all base file records on the volume.
 * Note: the meaning of the time fields has been verified on real Windows NT4 SP6a.
 */
typedef struct {
/*Ofs*/
/*  0*/	sle64 creation_time;		/* file creation time.
					   Updated when the file name changes (?). */
/*  8*/	sle64 last_data_change_time;	/* last modification time of the data attribute. */
/* 16*/	sle64 last_mft_change_time;	/* last modification time of this MFT record. */
/* 24*/	sle64 last_access_time;		/* approximate time of last access to the file
					   (not updated on read-only volumes). In Windows it is
					   only updated when more than a certain interval has passed since
					   the last update, and it can be disabled entirely for speed. */
/* 32*/	FILE_ATTR_FLAGS file_attributes; /* flags describing the file. */
/* 36*/	union {
		/* NTFS 1.2 (and earlier) */
		struct {
		/* 36 */ u8 reserved12[12];	/* reserved/8-byte alignment. */
		/* 48 */ void *v1_end[0];	/* offsetof() marker. */
		} __attribute__((__packed__));
/* sizeof() = 48 bytes */
		/* NTFS 3.0 */
		struct {
/*
 * If the volume was upgraded from an older NTFS version, the following fields only exist after the file has been accessed following the upgrade.
 * Distinguished by the resident attribute value length: 48 means the following fields are absent, 72 means they are present.
 */
		/* 36*/	le32 maximum_versions;	/* maximum number of versions allowed for the file.
				Zero when versioning is disabled. */
		/* 40*/	le32 version_number;	/* current version number of the file.
				Also zero when maximum_versions is zero. */
		/* 44*/	le32 class_id;		/* class ID in the bidirectional class ID index. */
		/* 48*/	le32 owner_id;		/* owner_id of the user who owns this file.
				Converted to a quota control entry through the $Q index in FILE_Extend/$Quota.
				Zero when quotas are disabled. */
		/* 52*/	le32 security_id;	/* security_id of the file.
				Converted to a security descriptor through the $SII index in FILE_Secure and the $SDS
				data stream. */
		/* 56*/	le64 quota_charged;	/* number of bytes charged against the quota for all streams of the file.
				Zero when quotas are disabled. */
		/* 64*/	le64 usn;		/* last update sequence number (USN) of the file.
				A direct index into the change journal file.
				Zero when the USN journal is disabled.
				Note: disabling the journal requires deleting the journal file itself and walking
				the whole MFT to zero all USN entries.
				The journal is FILE_Extend/$UsnJrnl.
				Win2k automatically rebuilds the journal when mounting the partition. */
		/* 72*/ void *v3_end[0]; /* offsetof() marker. */
		} __attribute__((__packed__));
	} __attribute__((__packed__));
/* sizeof() = 72 bytes (NTFS 3.0) */
} __attribute__((__packed__)) STANDARD_INFORMATION;

/**
 * struct ATTR_LIST_ENTRY - Attribute: attribute list (0x20).
 *
 * - May be resident or non-resident.
 * - The value consists of a sequence of variable-length, 8-byte aligned ATTR_LIST_ENTRY records.
 * - The attribute list contains one entry for each attribute of the file (except the list attribute itself).
 *   Sort order: first by attribute type, then by attribute name (if any), then by instance number.
 *   Multiple extents of the same non-resident attribute immediately follow the initial extent, sorted by lowest_vcn,
 *   with the instance number set to zero. Two attributes with completely identical sort keys are not allowed.
 * - Further restrictions:
 *	- If non-resident, the VCN to LCN mapping array must fit in the base MFT record.
 *	- The attribute list value is at most 256KB (Windows cache manager limit).
 * - The attribute list is only used when the MFT record still does not fit after all attributes that can be made non-resident
 *   have been made non-resident. This can happen when:
 *	- The file has a large number of hard links (many file name attributes).
 *	- The mapping pairs array of a non-resident attribute overflows the MFT record due to fragmentation.
 *	- There are many named streams.
 */
typedef struct {
/*Ofs*/
/*  0*/	ATTR_TYPES type;	/* type of the referenced attribute. */
/*  4*/	le16 length;		/* size of this entry in bytes. */
/*  6*/	u8 name_length;		/* number of Unicode characters in the attribute name, 0 if unnamed. */
/*  7*/	u8 name_offset;		/* byte offset of the attribute name (always set to
				   the start of the name, even if unnamed). */
/*  8*/	leVCN lowest_vcn;	/* lowest VCN of this part of the attribute value, usually 0.
				   Non-zero when an attribute spans multiple MFT records,
				   with each record holding one extent and each extent
				   having one attribute list entry. Note: this really is a signed value,
				   the Windows driver compares using cmp + jg. */
/* 16*/	leMFT_REF mft_reference;/* MFT reference of the MFT record holding the ATTR_RECORD
				   for this part of the attribute value. */
/* 24*/	le16 instance;		/* if lowest_vcn = 0, the instance number of the
				   referenced attribute, otherwise 0. */
/* 26*/	ntfschar name[0];	/* only used at creation time. When reading, use name_offset
				   to determine the name position. */
/* sizeof() = 26 + (attribute name length * 2) bytes */
} __attribute__((__packed__)) ATTR_LIST_ENTRY;

/*
 * Maximum allowed length of a file name.
 */
#define NTFS_MAX_NAME_LEN	255

/**
 * enum FILE_NAME_TYPE_FLAGS - Namespace of an NTFS file name (8 bits).
 */
typedef enum {
	FILE_NAME_POSIX			= 0x00,
		/* The largest namespace. Case-sensitive, allows all Unicode characters except '\0' and '/'.
		   Note: WinNT/2k standard tools do not distinguish
		   file names that differ only in case. */
	FILE_NAME_WIN32			= 0x01,
		/* Standard WinNT/2k NTFS long file name. Case-insensitive.
		   Forbidden characters: '\0', '"', '*', '/', ':', '<', '>',
		   '?', '\' and '|'. Trailing dots and spaces are allowed (but on Windows
		   can only be accessed through \\?\ prefixed paths). */
	FILE_NAME_DOS			= 0x02,
		/* Standard DOS file name (8.3 format). Uppercase only.
		   Allows all 8-bit characters greater than space, forbids: '"', '*', '+',
		   ',', '/', ':', ';', '<', '=', '>', '?' and '\'.
		   Trailing dots and spaces are forbidden. */
	FILE_NAME_WIN32_AND_DOS		= 0x03,
		/* 3 means the Win32 and DOS file names are identical, so they are stored merged
		   in this single record. */
} __attribute__((__packed__)) FILE_NAME_TYPE_FLAGS;

/**
 * struct FILE_NAME_ATTR - Attribute: file name (0x30).
 *
 * Note: always resident.
 * Note: all fields except parent_directory are only updated when the file name changes.
 *     Until then they are out of sync with reality; more accurate values are in the standard information attribute.
 * Note: the meaning of the time fields has been verified on real Windows NT4 SP6a.
 */
typedef struct {
/*hex ofs*/
/*  0*/	leMFT_REF parent_directory;	/* the directory that references this file name. */
/*  8*/	sle64 creation_time;		/* file creation time. */
/* 10*/	sle64 last_data_change_time;	/* last modification time of the data attribute. */
/* 18*/	sle64 last_mft_change_time;	/* last modification time of this MFT record. */
/* 20*/	sle64 last_access_time;		/* last access time of this MFT record. */
/* 28*/	sle64 allocated_size;		/* number of bytes of disk space allocated for the data attribute.
					   For an ordinary $DATA it is the allocated_size of the unnamed
					   $DATA;
					   for a compressed/sparse $DATA it is
					   compressed_size.
					   Note: a multiple of the cluster size. */
/* 30*/	sle64 data_size;			/* actual size in bytes of the data in the data attribute. */
/* 38*/	FILE_ATTR_FLAGS file_attributes;	/* flags describing the file. */
/* 3c*/	union {
	/* 3c*/	struct {
		/* 3c*/	le16 packed_ea_size;	/* size of the buffer needed for packed extended attributes
						   (EA), if any EAs are present. */
		/* 3e*/	le16 reserved;		/* reserved/alignment. */
		} __attribute__((__packed__));
	/* 3c*/	le32 reparse_point_tag;		/* reparse point type,
						   only present in reparse points
						   without EAs. */
	} __attribute__((__packed__));
/* 40*/	u8 file_name_length;			/* file name length (in Unicode characters). */
/* 41*/	FILE_NAME_TYPE_FLAGS file_name_type;	/* namespace of the file name. */
/* 42*/	ntfschar file_name[0];			/* Unicode file name. */
} __attribute__((__packed__)) FILE_NAME_ATTR;

/**
 * struct GUID - Globally unique identifier (GUID) structure.
 *
 * A GUID is a 128-bit value consisting of one group of 8 hex digits, three groups of 4 hex digits
 * and one group of 12 hex digits. GUID is Microsoft's implementation of the DCE UUID.
 *
 * Example: 1F010768-5A73-BC91-0010-A52216A7227B
 */
typedef struct {
	le32 data1;	/* first eight hex digits. */
	le16 data2;	/* first group of four hex digits. */
	le16 data3;	/* second group of four hex digits. */
	u8 data4[8];	/* the first two bytes are the third group of four hex digits,
			   the remaining six bytes are the last 12 hex digits. */
} __attribute__((__packed__)) GUID;

/**
 * struct OBJ_ID_INDEX_DATA - $O index data of FILE_Extend/$ObjId.
 *
 * This index is keyed by object_id, and the data is the corresponding MFT record number.
 *
 * The data part also contains three object_ids:
 *	birth_volume_id - object_id of the volume the file was first created on (optional, may be zero).
 *	birth_object_id - object_id the file had when first created, usually equal to the current
 *			  object_id (optional, may be zero).
 *	domain_id	- reserved (always zero).
 */
typedef struct {
	leMFT_REF mft_reference;	/* MFT record containing the object_id
					   in the index entry key. */
	union {
		struct {
			GUID birth_volume_id;
			GUID birth_object_id;
			GUID domain_id;
		} __attribute__((__packed__));
		u8 extended_info[48];
	} __attribute__((__packed__));
} __attribute__((__packed__)) OBJ_ID_INDEX_DATA;

/**
 * struct OBJECT_ID_ATTR - Attribute: object ID (NTFS 3.0+) (0x40).
 *
 * Note: always resident.
 */
typedef struct {
	GUID object_id;				/* unique ID assigned to the file. */
	/* The following fields are optional. If absent, the attribute value size is 16 bytes (sizeof(GUID)).
	   Note: the entries may exist but be all zero, meaning the corresponding value is undefined.
	   If these fields are missing, they may be indexed in the $Extend/$ObjId system file
	   keyed by object_id. */
	union {
		struct {
			GUID birth_volume_id;	/* unique ID of the volume the file
						   was first created on. */
			GUID birth_object_id;	/* unique ID the file had when first created. */
			GUID domain_id;		/* reserved, zero. */
		} __attribute__((__packed__));
		u8 extended_info[48];
	} __attribute__((__packed__));
} __attribute__((__packed__)) OBJECT_ID_ATTR;

#if 0
/**
 * enum IDENTIFIER_AUTHORITIES - Predefined identifier authorities,
 * used as the SID_IDENTIFIER_AUTHORITY in the SID structure.
 */
typedef enum {					/* SID string prefix. */
	SECURITY_NULL_SID_AUTHORITY	= {0, 0, 0, 0, 0, 0},	/* S-1-0 */
	SECURITY_WORLD_SID_AUTHORITY	= {0, 0, 0, 0, 0, 1},	/* S-1-1 */
	SECURITY_LOCAL_SID_AUTHORITY	= {0, 0, 0, 0, 0, 2},	/* S-1-2 */
	SECURITY_CREATOR_SID_AUTHORITY	= {0, 0, 0, 0, 0, 3},	/* S-1-3 */
	SECURITY_NON_UNIQUE_AUTHORITY	= {0, 0, 0, 0, 0, 4},	/* S-1-4 */
	SECURITY_NT_SID_AUTHORITY	= {0, 0, 0, 0, 0, 5},	/* S-1-5 */
} IDENTIFIER_AUTHORITIES;
#endif

/**
 * enum RELATIVE_IDENTIFIERS - Relative identifiers (RID).
 *
 * These RIDs are used together with the identifier authorities above to form the common well-known SIDs.
 *
 * Note: a RID is the part of a SID that identifies a user or group (relative to the authority that issued the SID).
 * For example, the Creator Owner ID (S-1-3-0) consists of the authority
 * SECURITY_CREATOR_SID_AUTHORITY (3) and the RID
 * SECURITY_CREATOR_OWNER_RID (0).
 */
typedef enum {					/* identifier authority. */
	SECURITY_NULL_RID		  = 0,	/* S-1-0 */
	SECURITY_WORLD_RID		  = 0,	/* S-1-1 */
	SECURITY_LOCAL_RID		  = 0,	/* S-1-2 */

	SECURITY_CREATOR_OWNER_RID	  = 0,	/* S-1-3 */
	SECURITY_CREATOR_GROUP_RID	  = 1,	/* S-1-3 */

	SECURITY_CREATOR_OWNER_SERVER_RID = 2,	/* S-1-3 */
	SECURITY_CREATOR_GROUP_SERVER_RID = 3,	/* S-1-3 */

	SECURITY_DIALUP_RID		  = 1,
	SECURITY_NETWORK_RID		  = 2,
	SECURITY_BATCH_RID		  = 3,
	SECURITY_INTERACTIVE_RID	  = 4,
	SECURITY_SERVICE_RID		  = 6,
	SECURITY_ANONYMOUS_LOGON_RID	  = 7,
	SECURITY_PROXY_RID		  = 8,
	SECURITY_ENTERPRISE_CONTROLLERS_RID=9,
	SECURITY_SERVER_LOGON_RID	  = 9,
	SECURITY_PRINCIPAL_SELF_RID	  = 0xa,
	SECURITY_AUTHENTICATED_USER_RID	  = 0xb,
	SECURITY_RESTRICTED_CODE_RID	  = 0xc,
	SECURITY_TERMINAL_SERVER_RID	  = 0xd,

	SECURITY_LOGON_IDS_RID		  = 5,
	SECURITY_LOGON_IDS_RID_COUNT	  = 3,

	SECURITY_LOCAL_SYSTEM_RID	  = 0x12,

	SECURITY_NT_NON_UNIQUE		  = 0x15,

	SECURITY_BUILTIN_DOMAIN_RID	  = 0x20,

	/*
	 * Well-known domain-relative sub-authority values (RID).
	 */

	/* Users. */
	DOMAIN_USER_RID_ADMIN		  = 0x1f4,
	DOMAIN_USER_RID_GUEST		  = 0x1f5,
	DOMAIN_USER_RID_KRBTGT		  = 0x1f6,

	/* Groups. */
	DOMAIN_GROUP_RID_ADMINS		  = 0x200,
	DOMAIN_GROUP_RID_USERS		  = 0x201,
	DOMAIN_GROUP_RID_GUESTS		  = 0x202,
	DOMAIN_GROUP_RID_COMPUTERS	  = 0x203,
	DOMAIN_GROUP_RID_CONTROLLERS	  = 0x204,
	DOMAIN_GROUP_RID_CERT_ADMINS	  = 0x205,
	DOMAIN_GROUP_RID_SCHEMA_ADMINS	  = 0x206,
	DOMAIN_GROUP_RID_ENTERPRISE_ADMINS= 0x207,
	DOMAIN_GROUP_RID_POLICY_ADMINS	  = 0x208,

	/* Aliases. */
	DOMAIN_ALIAS_RID_ADMINS		  = 0x220,
	DOMAIN_ALIAS_RID_USERS		  = 0x221,
	DOMAIN_ALIAS_RID_GUESTS		  = 0x222,
	DOMAIN_ALIAS_RID_POWER_USERS	  = 0x223,

	DOMAIN_ALIAS_RID_ACCOUNT_OPS	  = 0x224,
	DOMAIN_ALIAS_RID_SYSTEM_OPS	  = 0x225,
	DOMAIN_ALIAS_RID_PRINT_OPS	  = 0x226,
	DOMAIN_ALIAS_RID_BACKUP_OPS	  = 0x227,

	DOMAIN_ALIAS_RID_REPLICATOR	  = 0x228,
	DOMAIN_ALIAS_RID_RAS_SERVERS	  = 0x229,
	DOMAIN_ALIAS_RID_PREW2KCOMPACCESS = 0x22a,
} RELATIVE_IDENTIFIERS;

/*
 * Common well-known SIDs:
 *
 *	NULL_SID			S-1-0-0
 *	WORLD_SID			S-1-1-0
 *	LOCAL_SID			S-1-2-0
 *	CREATOR_OWNER_SID		S-1-3-0
 *	CREATOR_GROUP_SID		S-1-3-1
 *	CREATOR_OWNER_SERVER_SID	S-1-3-2
 *	CREATOR_GROUP_SERVER_SID	S-1-3-3
 *
 *	(non-unique ID)				S-1-4
 *
 * NT well-known SIDs:
 *
 *	NT_AUTHORITY_SID	S-1-5
 *	DIALUP_SID		S-1-5-1
 *	NETWORD_SID		S-1-5-2
 *	BATCH_SID		S-1-5-3
 *	INTERACTIVE_SID		S-1-5-4
 *	SERVICE_SID		S-1-5-6
 *	ANONYMOUS_LOGON_SID	S-1-5-7		(null logon session)
 *	PROXY_SID		S-1-5-8
 *	SERVER_LOGON_SID	S-1-5-9		(domain controller account)
 *	SELF_SID		S-1-5-10	(self RID)
 *	AUTHENTICATED_USER_SID	S-1-5-11
 *	RESTRICTED_CODE_SID	S-1-5-12	(restricted code execution)
 *	TERMINAL_SERVER_SID	S-1-5-13	(terminal server execution)
 *
 *	(logon ID)			S-1-5-5-X-Y
 *	(NT non-unique ID)		S-1-5-0x15-...
 *	(builtin domain)			S-1-5-0x20
 */

/**
 * 48-bit value used in the SID structure.
 *
 * Note: stored in big-endian.
 */
typedef union {
	struct {
		be16 high_part;		/* upper 16 bits. */
		be32 low_part;		/* lower 32 bits. */
	} __attribute__((__packed__));
	u8 value[6];			/* value of each byte. */
} __attribute__((__packed__)) SID_IDENTIFIER_AUTHORITY;

/**
 * struct SID - Security identifier (SID) structure.
 *
 * A SID is a variable-length structure used to uniquely identify a user or group.
 *
 * The standard text representation of a SID is:
 *	S-R-I-S-S...
 * where:
 *    - The leading "S" is the literal character 'S', identifying the following digits as a SID.
 *    - R is the SID revision level (decimal).
 *    - I is the 48-bit identifier_authority (decimal when I < 2^32,
 *	otherwise hexadecimal with a "0x" prefix).
 *    - S... is one or more sub_authority values (decimal).
 *
 * Example SID: the domain-relative SID of the local Administrators group on Windows NT/2k:
 *	S-1-5-32-544
 * corresponding to: revision = 1, sub_authority_count = 2,
 *	identifier_authority = {0,0,0,0,0,5},	// SECURITY_NT_AUTHORITY
 *	sub_authority[0] = 32,			// SECURITY_BUILTIN_DOMAIN_RID
 *	sub_authority[1] = 544			// DOMAIN_ALIAS_RID_ADMINS
 */
typedef struct {
	u8 revision;
	u8 sub_authority_count;
	SID_IDENTIFIER_AUTHORITY identifier_authority;
	le32 sub_authority[1];		/* at least one sub_authority. */
} __attribute__((__packed__)) SID;

/**
 * enum SID_CONSTANTS - Current constants for SIDs.
 */
typedef enum {
	SID_REVISION			=  1,	/* current revision level. */
	SID_MAX_SUB_AUTHORITIES		= 15,	/* maximum number of sub_authorities. */
	SID_RECOMMENDED_SUB_AUTHORITIES	=  1,	/* may change to about 6 in future versions. */
} SID_CONSTANTS;

/**
 * enum ACE_TYPES - Predefined ACE types (8 bits).
 */
typedef enum {
	ACCESS_MIN_MS_ACE_TYPE		= 0,
	ACCESS_ALLOWED_ACE_TYPE		= 0,
	ACCESS_DENIED_ACE_TYPE		= 1,
	SYSTEM_AUDIT_ACE_TYPE		= 2,
	SYSTEM_ALARM_ACE_TYPE		= 3, /* not yet implemented in Win2k. */
	ACCESS_MAX_MS_V2_ACE_TYPE	= 3,

	ACCESS_ALLOWED_COMPOUND_ACE_TYPE= 4,
	ACCESS_MAX_MS_V3_ACE_TYPE	= 4,

	/* The following are Win2k only. */
	ACCESS_MIN_MS_OBJECT_ACE_TYPE	= 5,
	ACCESS_ALLOWED_OBJECT_ACE_TYPE	= 5,
	ACCESS_DENIED_OBJECT_ACE_TYPE	= 6,
	SYSTEM_AUDIT_OBJECT_ACE_TYPE	= 7,
	SYSTEM_ALARM_OBJECT_ACE_TYPE	= 8,
	ACCESS_MAX_MS_OBJECT_ACE_TYPE	= 8,

	ACCESS_MAX_MS_V4_ACE_TYPE	= 8,

	/* WinNT and 2k. */
	ACCESS_MAX_MS_ACE_TYPE		= 8,

	/* Windows XP and later */
	ACCESS_ALLOWED_CALLBACK_ACE_TYPE	= 9,
	ACCESS_DENIED_CALLBACK_ACE_TYPE		= 10,
	ACCESS_ALLOWED_CALLBACK_OBJECT_ACE_TYPE	= 11,
	ACCESS_DENIED_CALLBACK_OBJECT_ACE_TYPE	= 12,
	SYSTEM_AUDIT_CALLBACK_ACE_TYPE		= 13,
	SYSTEM_ALARM_CALLBACK_ACE_TYPE		= 14, /* reserved */
	SYSTEM_AUDIT_CALLBACK_OBJECT_ACE_TYPE   = 15,
	SYSTEM_ALARM_CALLBACK_OBJECT_ACE_TYPE   = 16, /* reserved */

	/* Windows Vista and later */
	SYSTEM_MANDATORY_LABEL_ACE_TYPE		= 17,

	/* Windows 8 and later */
	SYSTEM_RESOURCE_ATTRIBUTE_ACE_TYPE	= 18,
	SYSTEM_SCOPED_POLICY_ID_ACE_TYPE	= 19,

	/* Windows 10 and later */
	SYSTEM_PROCESS_TRUST_LABEL_ACE_TYPE	= 20,
} __attribute__((__packed__)) ACE_TYPES;

/**
 * enum ACE_FLAGS - ACE flags (8 bits), used for auditing and inheritance.
 *
 * SUCCESSFUL_ACCESS_ACE_FLAG is only used for system audit and alarm ACE types,
 * indicating that a message is generated on successful access.
 *
 * FAILED_ACCESS_ACE_FLAG is only used for system audit and alarm ACE types,
 * indicating that a message is generated on failed access.
 */
typedef enum {
	/* Inheritance flags. */
	OBJECT_INHERIT_ACE		= 0x01,
	CONTAINER_INHERIT_ACE		= 0x02,
	NO_PROPAGATE_INHERIT_ACE	= 0x04,
	INHERIT_ONLY_ACE		= 0x08,
	INHERITED_ACE			= 0x10,	/* Win2k only. */
	VALID_INHERIT_FLAGS		= 0x1f,

	/* Audit flags. */
	SUCCESSFUL_ACCESS_ACE_FLAG	= 0x40,
	FAILED_ACCESS_ACE_FLAG		= 0x80,
} __attribute__((__packed__)) ACE_FLAGS;

/**
 * struct ACE_HEADER - ACE header structure.
 *
 * An ACE is an access control entry in an access control list (ACL).
 * An ACE defines the access rights of an object for a specific user or group, or defines the types of access that
 * generate system administration messages or alarms for a specific user or group. The user or group is identified by a SID.
 *
 * Each ACE starts with an ACE_HEADER structure (4-byte aligned) specifying the
 * type and size of the ACE. The format of the following data depends on the ACE type.
 */
typedef struct {
	ACE_TYPES type;		/* type of the ACE. */
	ACE_FLAGS flags;	/* flags describing the ACE. */
	le16 size;		/* size of the ACE in bytes. */
} __attribute__((__packed__)) ACE_HEADER;

/**
 * enum ACCESS_MASK - Access mask (32 bits) defining access rights.
 */
typedef enum {
	/*
	 * Specific rights (bits 0 to 15), depending on the type of object protected by the ACE.
	 */

	/* The specific rights for files and directories are as follows: */

	/* Right to read data from the file. (file) */
	FILE_READ_DATA			= const_cpu_to_le32(0x00000001),
	/* Right to list the contents of the directory. (directory) */
	FILE_LIST_DIRECTORY		= const_cpu_to_le32(0x00000001),

	/* Right to write data to the file. (file) */
	FILE_WRITE_DATA			= const_cpu_to_le32(0x00000002),
	/* Right to create files in the directory. (directory) */
	FILE_ADD_FILE			= const_cpu_to_le32(0x00000002),

	/* Right to append data to the file. (file) */
	FILE_APPEND_DATA		= const_cpu_to_le32(0x00000004),
	/* Right to create subdirectories. (directory) */
	FILE_ADD_SUBDIRECTORY		= const_cpu_to_le32(0x00000004),

	/* Right to read extended attributes. (file/directory) */
	FILE_READ_EA			= const_cpu_to_le32(0x00000008),

	/* Right to write extended attributes. (file/directory) */
	FILE_WRITE_EA			= const_cpu_to_le32(0x00000010),

	/* Right to execute the file. (file) */
	FILE_EXECUTE			= const_cpu_to_le32(0x00000020),
	/* Right to traverse the directory. (directory) */
	FILE_TRAVERSE			= const_cpu_to_le32(0x00000020),

	/*
	 * Right to delete a directory and all its child files (even if the files are read-only). (directory)
	 */
	FILE_DELETE_CHILD		= const_cpu_to_le32(0x00000040),

	/* Right to read file attributes. (file/directory) */
	FILE_READ_ATTRIBUTES		= const_cpu_to_le32(0x00000080),

	/* Right to modify file attributes. (file/directory) */
	FILE_WRITE_ATTRIBUTES		= const_cpu_to_le32(0x00000100),

	/*
	 * Standard rights (bits 16 to 23), independent of the type of the protected object.
	 */

	/* Right to delete the object. */
	DELETE				= const_cpu_to_le32(0x00010000),

	/*
	 * Right to read information in the object's security descriptor (excluding the SACL),
	 * i.e. the right to read the security descriptor and owner.
	 */
	READ_CONTROL			= const_cpu_to_le32(0x00020000),

	/* Right to modify the DACL in the object's security descriptor. */
	WRITE_DAC			= const_cpu_to_le32(0x00040000),

	/* Right to modify the owner in the object's security descriptor. */
	WRITE_OWNER			= const_cpu_to_le32(0x00080000),

	/*
	 * Right to use the object for synchronization. Allows a process to wait for the object to enter the signaled state.
	 * Some object types do not support this right.
	 */
	SYNCHRONIZE			= const_cpu_to_le32(0x00100000),

	/*
	 * The following STANDARD_RIGHTS_* are convenience combinations of the rights above,
	 * defined by the Win32 API.
	 */

	/* All currently defined as READ_CONTROL. */
	STANDARD_RIGHTS_READ		= const_cpu_to_le32(0x00020000),
	STANDARD_RIGHTS_WRITE		= const_cpu_to_le32(0x00020000),
	STANDARD_RIGHTS_EXECUTE		= const_cpu_to_le32(0x00020000),

	/* Combination of DELETE, READ_CONTROL, WRITE_DAC and WRITE_OWNER. */
	STANDARD_RIGHTS_REQUIRED	= const_cpu_to_le32(0x000f0000),

	/*
	 * Combination of DELETE, READ_CONTROL, WRITE_DAC, WRITE_OWNER
	 * and SYNCHRONIZE.
	 */
	STANDARD_RIGHTS_ALL		= const_cpu_to_le32(0x001f0000),

	/*
	 * Access to the system ACL and the maximum-allowed access type (bits 24 to 25,
	 * bits 26 to 27 are reserved).
	 */
	ACCESS_SYSTEM_SECURITY		= const_cpu_to_le32(0x01000000),
	MAXIMUM_ALLOWED			= const_cpu_to_le32(0x02000000),

	/*
	 * Generic rights (bits 28 to 31), mapped to standard and specific rights.
	 */

	/* Read, write and execute rights. */
	GENERIC_ALL			= const_cpu_to_le32(0x10000000),

	/* Execute right. */
	GENERIC_EXECUTE			= const_cpu_to_le32(0x20000000),

	/*
	 * Write right. For files it maps to:
	 *	FILE_APPEND_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_DATA |
	 *	FILE_WRITE_EA | STANDARD_RIGHTS_WRITE | SYNCHRONIZE
	 * The same numeric mapping applies to directories.
	 */
	GENERIC_WRITE			= const_cpu_to_le32(0x40000000),

	/*
	 * Read right. For files it maps to:
	 *	FILE_READ_ATTRIBUTES | FILE_READ_DATA | FILE_READ_EA |
	 *	STANDARD_RIGHTS_READ | SYNCHRONIZE
	 * The same numeric mapping applies to directories.
	 */
	GENERIC_READ			= const_cpu_to_le32(0x80000000),
} ACCESS_MASK;

/**
 * struct GENERIC_MAPPING - Generic mapping array.
 *
 * Used to express the mapping of each generic access right to a specific access mask.
 */
typedef struct {
	ACCESS_MASK generic_read;
	ACCESS_MASK generic_write;
	ACCESS_MASK generic_execute;
	ACCESS_MASK generic_all;
} __attribute__((__packed__)) GENERIC_MAPPING;

/*
 * The predefined ACE type structures are defined as follows.
 */

/**
 * struct ACCESS_DENIED_ACE -
 *
 * ACCESS_ALLOWED_ACE, ACCESS_DENIED_ACE, SYSTEM_AUDIT_ACE, SYSTEM_ALARM_ACE
 */
typedef struct {
/*  0	ACE_HEADER; -- expanded because gcc does not support anonymous structs.*/
	ACE_TYPES type;		/* type of the ACE. */
	ACE_FLAGS flags;	/* flags describing the ACE. */
	le16 size;		/* size of the ACE in bytes. */

/*  4*/	ACCESS_MASK mask;	/* access mask associated with this ACE. */
/*  8*/	SID sid;		/* SID associated with this ACE. */
} __attribute__((__packed__)) ACCESS_ALLOWED_ACE, ACCESS_DENIED_ACE,
			       SYSTEM_AUDIT_ACE, SYSTEM_ALARM_ACE;

/**
 * enum OBJECT_ACE_FLAGS - Object ACE flags (32 bits).
 */
typedef enum {
	ACE_OBJECT_TYPE_PRESENT			= const_cpu_to_le32(1),
	ACE_INHERITED_OBJECT_TYPE_PRESENT	= const_cpu_to_le32(2),
} OBJECT_ACE_FLAGS;

/**
 * struct ACCESS_ALLOWED_OBJECT_ACE - Object ACE structure.
 */
typedef struct {
/*  0	ACE_HEADER; -- expanded because gcc does not support anonymous structs.*/
	ACE_TYPES type;		/* type of the ACE. */
	ACE_FLAGS flags;	/* flags describing the ACE. */
	le16 size;		/* size of the ACE in bytes. */

/*  4*/	ACCESS_MASK mask;	/* access mask associated with this ACE. */
/*  8*/	OBJECT_ACE_FLAGS object_flags;	/* flags describing the object ACE. */
/* 12*/	GUID object_type;
/* 28*/	GUID inherited_object_type;
/* 44*/	SID sid;		/* SID associated with this ACE. */
} __attribute__((__packed__)) ACCESS_ALLOWED_OBJECT_ACE,
			       ACCESS_DENIED_OBJECT_ACE,
			       SYSTEM_AUDIT_OBJECT_ACE,
			       SYSTEM_ALARM_OBJECT_ACE;

/**
 * struct ACL - Access control list (ACL).
 *
 * An ACL starts with an ACL header structure specifying the ACL size and the number of ACEs it contains.
 * The header is followed by zero or more ACEs. The ACL and each ACE are 4-byte aligned.
 */
typedef struct {
	u8 revision;	/* revision level of the ACL. */
	u8 alignment1;
	le16 size;	/* space allocated for the ACL (in bytes), including this header, the ACEs and
			   any remaining free space. */
	le16 ace_count;	/* number of ACEs in the ACL. */
	le16 alignment2;
/* sizeof() = 8 bytes */
} __attribute__((__packed__)) ACL;

/**
 * enum ACL_CONSTANTS - Current constants for ACLs.
 */
typedef enum {
	/* Current revision. */
	ACL_REVISION		= 2,
	ACL_REVISION_DS		= 4,

	/* Revision history. */
	ACL_REVISION1		= 1,
	MIN_ACL_REVISION	= 2,
	ACL_REVISION2		= 2,
	ACL_REVISION3		= 3,
	ACL_REVISION4		= 4,
	MAX_ACL_REVISION	= 4,
} ACL_CONSTANTS;

/**
 * enum SECURITY_DESCRIPTOR_CONTROL - Security descriptor control flags (16 bits).
 *
 * SE_OWNER_DEFAULTED - When set, indicates that the SID pointed to by the Owner field was provided by a default mechanism
 *	rather than explicitly specified by the original provider. May affect owner inheritance.
 *
 * SE_GROUP_DEFAULTED - When set, indicates that the SID in the Group field was provided by a default mechanism.
 *	May affect primary group inheritance.
 *
 * SE_DACL_PRESENT - When set, indicates that the security descriptor contains a DACL. If set but the Dacl
 *	field is empty, an empty ACL is explicitly specified.
 *
 * SE_DACL_DEFAULTED - When set, indicates that the DACL was provided by a default mechanism.
 *	Ignored when DaclPresent is not set.
 *
 * SE_SACL_PRESENT - When set, indicates that the security descriptor contains a SACL. If set but the Sacl
 *	field is empty, an empty (but present) ACL is specified.
 *
 * SE_SACL_DEFAULTED - When set, indicates that the SACL was provided by a default mechanism.
 *	Ignored when SaclPresent is not set.
 *
 * SE_SELF_RELATIVE - When set, indicates that the security descriptor is in self-relative format.
 *	In this format all fields are contiguous in memory, and pointer fields are represented as
 *	offsets relative to the start of the security descriptor.
 */
typedef enum {
	SE_OWNER_DEFAULTED		= const_cpu_to_le16(0x0001),
	SE_GROUP_DEFAULTED		= const_cpu_to_le16(0x0002),
	SE_DACL_PRESENT			= const_cpu_to_le16(0x0004),
	SE_DACL_DEFAULTED		= const_cpu_to_le16(0x0008),
	SE_SACL_PRESENT			= const_cpu_to_le16(0x0010),
	SE_SACL_DEFAULTED		= const_cpu_to_le16(0x0020),
	SE_DACL_AUTO_INHERIT_REQ	= const_cpu_to_le16(0x0100),
	SE_SACL_AUTO_INHERIT_REQ	= const_cpu_to_le16(0x0200),
	SE_DACL_AUTO_INHERITED		= const_cpu_to_le16(0x0400),
	SE_SACL_AUTO_INHERITED		= const_cpu_to_le16(0x0800),
	SE_DACL_PROTECTED		= const_cpu_to_le16(0x1000),
	SE_SACL_PROTECTED		= const_cpu_to_le16(0x2000),
	SE_RM_CONTROL_VALID		= const_cpu_to_le16(0x4000),
	SE_SELF_RELATIVE		= const_cpu_to_le16(0x8000),
} __attribute__((__packed__)) SECURITY_DESCRIPTOR_CONTROL;

/**
 * struct SECURITY_DESCRIPTOR_RELATIVE - Self-relative security descriptor.
 *
 * Contains the owner and group SIDs and the SACL and DACL ACLs inside
 * the security descriptor itself.
 */
typedef struct {
	u8 revision;	/* security descriptor revision level. */
	u8 alignment;
	SECURITY_DESCRIPTOR_CONTROL control; /* flags qualifying the descriptor type and
			   the following fields. */
	le32 owner;	/* byte offset of the owner SID. If NULL there is no
			   owner SID. */
	le32 group;	/* byte offset of the primary group SID. If NULL there is no
			   primary group SID. */
	le32 sacl;	/* byte offset of the system ACL. Only valid when SE_SACL_PRESENT
			   is set in control. If set but
			   sacl is NULL, an empty ACL is specified. */
	le32 dacl;	/* byte offset of the discretionary ACL. Only valid when SE_DACL_PRESENT
			   is set in control. If set but
			   dacl is NULL, an empty ACL is specified
			   (unconditionally allowing access). */
/* sizeof() = 0x14 bytes */
} __attribute__((__packed__)) SECURITY_DESCRIPTOR_RELATIVE;

/**
 * struct SECURITY_DESCRIPTOR - Absolute security descriptor.
 *
 * Instead of containing the owner and group SIDs and the SACL and DACL ACLs inside the descriptor,
 * it contains pointers to where these structures are located in memory.
 * Absolute security descriptors are only used for in-memory representation. On disk, self-relative descriptors are used.
 */
typedef struct {
	u8 revision;	/* security descriptor revision level. */
	u8 alignment;
	SECURITY_DESCRIPTOR_CONTROL control;	/* flags qualifying the descriptor type and
			   the following fields. */
	SID *owner;	/* pointer to the owner SID. If NULL there is no owner SID. */
	SID *group;	/* pointer to the primary group SID. If NULL there is no primary group SID. */
	ACL *sacl;	/* pointer to the system ACL. Only valid when SE_SACL_PRESENT
			   is set in control. If set but
			   sacl is NULL, an empty ACL is specified. */
	ACL *dacl;	/* pointer to the discretionary ACL. Only valid when SE_DACL_PRESENT
			   is set in control. If set but
			   dacl is NULL, an empty ACL is specified
			   (unconditionally allowing access). */
} __attribute__((__packed__)) SECURITY_DESCRIPTOR;

/**
 * enum SECURITY_DESCRIPTOR_CONSTANTS - Current constants for security descriptors.
 */
typedef enum {
	/* Current revision. */
	SECURITY_DESCRIPTOR_REVISION	= 1,
	SECURITY_DESCRIPTOR_REVISION1	= 1,

	/* Absolute and relative security descriptors have the same size (at least on ia32, where pointers are 32 bits). */
	SECURITY_DESCRIPTOR_MIN_LENGTH	= sizeof(SECURITY_DESCRIPTOR),
} SECURITY_DESCRIPTOR_CONSTANTS;

/*
 * Attribute: security descriptor (0x50).
 *
 * Standard self-relative security descriptor.
 *
 * Note: may be resident or non-resident.
 * Note: not used by NTFS 3.0+, where security descriptors are stored centrally in FILE_Secure
 * and looked up through the security_id in the standard information attribute.
 */
typedef SECURITY_DESCRIPTOR_RELATIVE SECURITY_DESCRIPTOR_ATTR;

/*
 * In NTFS 3.0+ all security descriptors are stored in FILE_Secure. Only one referenced instance of each unique
 * security descriptor is stored.
 *
 * FILE_Secure has no unnamed data attribute (zero length), but contains two indexes
 * ($SDH and $SII) and one named data stream ($SDS).
 *
 * Each unique security descriptor is assigned a unique security_id (do not confuse it with a SID).
 * The security_id is unique on the NTFS volume and is used as the key of the $SII index, which maps the security_id
 * to the storage location of the security descriptor in the $SDS data attribute. The $SII index is sorted by security_id
 * in ascending order.
 *
 * A simple hash value is computed from each security descriptor. This hash is used as the key of the $SDH index,
 * which maps the hash value to the storage location in $SDS. The $SDH index is sorted by hash value and
 * stored in a B+ tree. When searching $SDH, if the hash matches but the descriptor does not,
 * the search continues with the next matching hash.
 *
 * When an exact match is found, the security_id is read from the $SDH index entry and stored in the
 * $STANDARD_INFORMATION attribute of the target file/directory.
 *
 * If no match is found, a new unique security_id is allocated, the descriptor is added to $SDS,
 * and reference entries are added to the $SDH and $SII indexes.
 *
 * Note: entries in FILE_Secure are never deleted, even when they are no longer referenced.
 */

/**
 * struct SECURITY_DESCRIPTOR_HEADER - Security descriptor header.
 *
 * In the $SDS data stream, this header precedes each security descriptor.
 * It is also the index entry data part of the $SII and $SDH indexes.
 */
typedef struct {
	le32 hash;	   /* hash value of the security descriptor. */
	le32 security_id;   /* security_id assigned to this descriptor. */
	le64 offset;	   /* byte offset of this entry in the $SDS stream. */
	le32 length;	   /* size of this entry in the $SDS stream in bytes. */
} __attribute__((__packed__)) SECURITY_DESCRIPTOR_HEADER;

/**
 * struct SDH_INDEX_DATA - $SDH index entry data.
 */
typedef struct {
	le32 hash;          /* hash value of the security descriptor. */
	le32 security_id;   /* security_id assigned to this descriptor. */
	le64 offset;	   /* byte offset of this entry in the $SDS stream. */
	le32 length;	   /* size of this entry in the $SDS stream in bytes. */
	le32 reserved_II;   /* padding - always Unicode "II" or zero.
			      This field is not counted in the data_length of the INDEX_ENTRY. */
} __attribute__((__packed__)) SDH_INDEX_DATA;

/**
 * struct SII_INDEX_DATA - $SII index entry data.
 */
typedef SECURITY_DESCRIPTOR_HEADER SII_INDEX_DATA;

/**
 * struct SDS_ENTRY - $SDS data stream entry.
 *
 * The $SDS data stream contains security descriptors, 16-byte aligned, sorted by security_id in a B+ tree.
 * A security descriptor cannot span a 256KiB boundary (Windows cache manager limit).
 * Each security descriptor is stored twice in the $SDS stream, at a fixed offset of 0x40000 bytes (256KiB) apart;
 * i.e. if the SDS_ENTRY offset is 0x51d0, the first copy is at 0x51d0 and the second at 0x451d0.
 */
typedef struct {
/*  0	SECURITY_DESCRIPTOR_HEADER; -- expanded.*/
	le32 hash;	   /* hash value of the security descriptor. */
	le32 security_id;   /* security_id assigned to this descriptor. */
	le64 offset;	   /* byte offset of this entry in the $SDS stream. */
	le32 length;	   /* size of this entry in the $SDS stream in bytes. */
/* 20*/	SECURITY_DESCRIPTOR_RELATIVE sid; /* self-relative security descriptor. */
} __attribute__((__packed__)) SDS_ENTRY;

/**
 * struct SII_INDEX_KEY - Index entry key used by the $SII index.
 *
 * Collation rule is COLLATION_NTOFS_ULONG.
 */
typedef struct {
	le32 security_id;   /* security_id assigned to this descriptor. */
} __attribute__((__packed__)) SII_INDEX_KEY;

/**
 * struct SDH_INDEX_KEY - Index entry key used by the $SDH index.
 *
 * The key is sorted first by hash, then by security_id.
 * Collation rule is COLLATION_NTOFS_SECURITY_HASH.
 */
typedef struct {
	le32 hash;	   /* hash value of the security descriptor. */
	le32 security_id;   /* security_id assigned to this descriptor. */
} __attribute__((__packed__)) SDH_INDEX_KEY;

/**
 * struct VOLUME_NAME - Attribute: volume name (0x60).
 *
 * Note: always resident.
 * Note: only present in FILE_Volume.
 */
typedef struct {
	ntfschar name[0];	/* volume name in Unicode format. */
} __attribute__((__packed__)) VOLUME_NAME;

/**
 * enum VOLUME_FLAGS - Volume flags (16 bits).
 */
typedef enum {
	VOLUME_IS_DIRTY			= const_cpu_to_le16(0x0001),
	VOLUME_RESIZE_LOG_FILE		= const_cpu_to_le16(0x0002),
	VOLUME_UPGRADE_ON_MOUNT		= const_cpu_to_le16(0x0004),
	VOLUME_MOUNTED_ON_NT4		= const_cpu_to_le16(0x0008),
	VOLUME_DELETE_USN_UNDERWAY	= const_cpu_to_le16(0x0010),
	VOLUME_REPAIR_OBJECT_ID		= const_cpu_to_le16(0x0020),
	VOLUME_CHKDSK_UNDERWAY		= const_cpu_to_le16(0x4000),
	VOLUME_MODIFIED_BY_CHKDSK	= const_cpu_to_le16(0x8000),
	VOLUME_FLAGS_MASK		= const_cpu_to_le16(0xc03f),
} __attribute__((__packed__)) VOLUME_FLAGS;

/**
 * struct VOLUME_INFORMATION - Attribute: volume information (0x70).
 *
 * Note: always resident.
 * Note: only present in FILE_Volume.
 * Note: Windows 2000 uses NTFS 3.0, Windows NT4 SP6a uses NTFS 1.2.
 */
typedef struct {
	le64 reserved;		/* unused (for now). */
	u8 major_ver;		/* major version number of the NTFS format. */
	u8 minor_ver;		/* minor version number of the NTFS format. */
	VOLUME_FLAGS flags;	/* VOLUME_* flags bit array. */
} __attribute__((__packed__)) VOLUME_INFORMATION;

/**
 * struct DATA_ATTR - Attribute: data (0x80).
 *
 * Note: may be resident or non-resident.
 *
 * The data content of the file (i.e. the unnamed stream) or of a named stream.
 */
typedef struct {
	u8 data[0];		/* data content of the file. */
} __attribute__((__packed__)) DATA_ATTR;

/**
 * enum INDEX_HEADER_FLAGS - Index header flags (8 bits).
 */
typedef enum {
	/* When the index header is in the index root attribute: */
	SMALL_INDEX	= 0, /* the index is small enough to fit inside the index root attribute,
				with no index allocation attribute. */
	LARGE_INDEX	= 1, /* the index is too large to fit in the index root attribute
				and/or an index allocation attribute is present. */
	/*
	 * When the index header is in an index block (i.e. part of the index allocation attribute):
	 */
	LEAF_NODE	= 0, /* leaf node, no branch child nodes. */
	INDEX_NODE	= 1, /* non-leaf node, indexes other nodes. */
	NODE_MASK	= 1, /* mask for accessing the *_NODE bits. */
} __attribute__((__packed__)) INDEX_HEADER_FLAGS;

/**
 * struct INDEX_HEADER - Index header.
 *
 * The header describing the INDEX_ENTRY records that follow. The index header and the index entries together form the complete index.
 *
 * Important: the offset, length and size members are computed relative to the start of the index header structure,
 * not relative to the start of the index root or index allocation structure.
 */
typedef struct {
/*  0*/	le32 entries_offset;	/* byte offset from the INDEX_HEADER to the first INDEX_ENTRY,
				   8-byte aligned. */
/*  4*/	le32 index_length;	/* data size of the INDEX_ENTRYs (including the INDEX_HEADER),
				   8-byte aligned. */
/*  8*/	le32 allocated_size;	/* allocated size of this index (block) in bytes, a multiple of 8.
				   See the note below. */
	/*
	   For the index root attribute, the two numbers above are always equal,
	   because the attribute is resident and is resized as needed.

	   For the index allocation attribute, the attribute is non-resident,
	   and allocated_size equals the index_block_size specified by the corresponding INDEX_ROOT attribute
	   minus the size of the part of the INDEX_BLOCK before the INDEX_HEADER
	   (i.e. minus 24).
	 */
/* 12*/	INDEX_HEADER_FLAGS ih_flags;	/* INDEX_HEADER_FLAGS bit field. */
/* 13*/	u8 reserved[3];			/* reserved/8-byte alignment. */
/* sizeof() == 16 */
} __attribute__((__packed__)) INDEX_HEADER;

/**
 * struct INDEX_ROOT - Attribute: index root (0x90).
 *
 * Note: always resident.
 *
 * Followed by a sequence of index entries (INDEX_ENTRY) described by the index header.
 *
 * When a directory is small enough to fit in the index root, this is the only attribute describing the directory.
 * When the directory is too large, there are two more attributes: the index allocation attribute (containing the child nodes
 * of the B+ tree) and the bitmap attribute (describing which VCNs in the index allocation
 * are occupied by index blocks).
 *
 * Note: the root directory (FILE_root) contains an entry for itself; other directories do not.
 */
typedef struct {
/*  0*/	ATTR_TYPES type;		/* type of the indexed attribute. $FILE_NAME for directories,
					   zero for view indexes.
					   No other values are allowed. */
/*  4*/	COLLATION_RULES collation_rule;	/* collation rule used to sort the index entries.
					   If type is $FILE_NAME, it must be
					   COLLATION_FILE_NAME. */
/*  8*/	le32 index_block_size;		/* size of an index block in bytes (in the index
					   allocation attribute). */
/* 12*/	s8 clusters_per_index_block;	/* size of an index block in clusters, when an index block is
					   >= one cluster; otherwise the number of sectors
					   per index block. */
/* 13*/	u8 reserved[3];			/* reserved/8-byte alignment. */
/* 16*/	INDEX_HEADER index;		/* index header describing the index entries that follow. */
/* sizeof()= 32 bytes */
} __attribute__((__packed__)) INDEX_ROOT;

/**
 * struct INDEX_BLOCK - Attribute: index allocation (0xa0).
 *
 * Note: always non-resident.
 *
 * This is an array of index blocks. Each index block starts with an INDEX_BLOCK structure (including the index header),
 * followed by a sequence of index entries (INDEX_ENTRY) described by the INDEX_HEADER.
 */
typedef struct {
/*  0	NTFS_RECORD; -- expanded because gcc does not support anonymous structs.*/
	NTFS_RECORD_TYPES magic;/* the magic number is "INDX". */
	le16 usa_ofs;		/* See the NTFS_RECORD definition. */
	le16 usa_count;		/* See the NTFS_RECORD definition. */

/*  8*/	leLSN lsn;		/* $LogFile sequence number of the last modification of this index block. */
/* 16*/	leVCN index_block_vcn;	/* virtual cluster number of the index block. */
/* 24*/	INDEX_HEADER index;	/* describes the index entries that follow. */
/* sizeof()= 40 (0x28) bytes */
/*
 * When an index block is created, the update sequence array is placed at this offset, i.e. before the index entries.
 * Otherwise the update sequence array itself could contain the last two bytes of a sector, defeating multi-sector transfer protection.
 * When reading, use the data in the NTFS record header.
 */
} __attribute__((__packed__)) INDEX_BLOCK;

typedef INDEX_BLOCK INDEX_ALLOCATION;

/**
 * struct REPARSE_INDEX_KEY - $R index key of FILE_Extend/$Reparse.
 *
 * The system file FILE_Extend/$Reparse contains an index named $R that lists all
 * reparse points on the volume. The index entry key is defined as follows. The index entries have no associated data.
 * Entries are sorted by file_id, with collation rule COLLATION_NTOFS_ULONGS.
 */
typedef struct {
	le32 reparse_tag;	/* reparse point type (including flags). */
	leMFT_REF file_id;	/* MFT record of the file containing the
				   reparse point attribute. */
} __attribute__((__packed__)) REPARSE_INDEX_KEY;

/**
 * enum QUOTA_FLAGS - Quota flags (32 bits).
 */
typedef enum {
	/* User quota flags. */
	QUOTA_FLAG_DEFAULT_LIMITS	= const_cpu_to_le32(0x00000001),
	QUOTA_FLAG_LIMIT_REACHED	= const_cpu_to_le32(0x00000002),
	QUOTA_FLAG_ID_DELETED		= const_cpu_to_le32(0x00000004),

	QUOTA_FLAG_USER_MASK		= const_cpu_to_le32(0x00000007),
		/* Bit mask of the user quota flags. */

	/* The following flags only exist in the quota defaults index entry
	   (i.e. the entry with owner_id = QUOTA_DEFAULTS_ID). */
	QUOTA_FLAG_TRACKING_ENABLED	= const_cpu_to_le32(0x00000010),
	QUOTA_FLAG_ENFORCEMENT_ENABLED	= const_cpu_to_le32(0x00000020),
	QUOTA_FLAG_TRACKING_REQUESTED	= const_cpu_to_le32(0x00000040),
	QUOTA_FLAG_LOG_THRESHOLD	= const_cpu_to_le32(0x00000080),
	QUOTA_FLAG_LOG_LIMIT		= const_cpu_to_le32(0x00000100),
	QUOTA_FLAG_OUT_OF_DATE		= const_cpu_to_le32(0x00000200),
	QUOTA_FLAG_CORRUPT		= const_cpu_to_le32(0x00000400),
	QUOTA_FLAG_PENDING_DELETES	= const_cpu_to_le32(0x00000800),
} QUOTA_FLAGS;

/**
 * struct QUOTA_CONTROL_ENTRY - Quota control entry.
 *
 * FILE_Extend/$Quota contains two indexes, $O and $Q. Quotas are per user per volume.
 *
 * The $Q index contains one entry for each user_id on the volume. The index key is the user_id (i.e. owner_id) of the user/group that owns this quota control
 * entry. The owner_id of a file is in the
 * standard information attribute. The collation rule of $Q is COLLATION_NTOFS_ULONG.
 *
 * The $O index contains one entry for each user/group that has been assigned a quota. The index key is the SID of the corresponding
 * user_id. The collation rule of $O is COLLATION_NTOFS_SID.
 *
 * The $O index entry data is the user_id corresponding to the SID, used as the index key of $Q
 * to find the associated quota control entry.
 *
 * The $Q index entry data is the quota control entry, defined as follows.
 */
typedef struct {
	le32 version;		/* currently 2. */
	QUOTA_FLAGS flags;	/* flags describing this quota entry. */
	le64 bytes_used;		/* number of quota bytes used. */
	sle64 change_time;	/* time this quota entry was last modified. */
	sle64 threshold;		/* soft quota limit (-1 means unlimited). */
	sle64 limit;		/* hard quota limit (-1 means unlimited). */
	sle64 exceeded_time;	/* duration for which the soft quota has been exceeded. */
/* The following fields are absent in the quota defaults entry. */
	SID sid;		/* SID of the user/object associated with this quota entry.
				   If this field is missing, the INDEX_ENTRY is zero-padded to
				   a multiple of 8 (not counted in data_length). If the SID
				   is present, the structure is zero-padded to a multiple of 8 (counted in
				   the data_length of the INDEX_ENTRY). */
} __attribute__((__packed__)) QUOTA_CONTROL_ENTRY;

/**
 * struct QUOTA_O_INDEX_DATA - $O index entry data.
 */
typedef struct {
	le32 owner_id;
	le32 unknown;		/* always 32. Seems to be padding, not counted in
				   the data_length of the INDEX_ENTRY. */
} __attribute__((__packed__)) QUOTA_O_INDEX_DATA;

/**
 * enum PREDEFINED_OWNER_IDS - Predefined owner_id values (32 bits).
 */
typedef enum {
	QUOTA_INVALID_ID	= const_cpu_to_le32(0x00000000),
	QUOTA_DEFAULTS_ID	= const_cpu_to_le32(0x00000001),
	QUOTA_FIRST_USER_ID	= const_cpu_to_le32(0x00000100),
} PREDEFINED_OWNER_IDS;

/**
 * enum INDEX_ENTRY_FLAGS - Index entry flags (16 bits).
 */
typedef enum {
	INDEX_ENTRY_NODE = const_cpu_to_le16(1), /* this entry contains a child node,
					i.e. a virtual cluster number reference to an index block
					(see below). */
	INDEX_ENTRY_END  = const_cpu_to_le16(2), /* marks the last entry in an index block. This entry does not
					represent a file, but may
					point to a child node. */
	INDEX_ENTRY_SPACE_FILLER = 0xffff, /* only used to force 16-bit width. */
} __attribute__((__packed__)) INDEX_ENTRY_FLAGS;

/**
 * struct INDEX_ENTRY_HEADER - Index entry header (see the INDEX_ENTRY field descriptions below).
 */
typedef struct {
/*  0*/	union {
		leMFT_REF indexed_file;
		struct {
			le16 data_offset;
			le16 data_length;
			le32 reservedV;
		} __attribute__((__packed__));
	} __attribute__((__packed__));
/*  8*/	le16 length;
/* 10*/	le16 key_length;
/* 12*/	INDEX_ENTRY_FLAGS flags;
/* 14*/	le16 reserved;
/* sizeof() = 16 bytes */
} __attribute__((__packed__)) INDEX_ENTRY_HEADER;

/**
 * struct INDEX_ENTRY - Index entry.
 *
 * A sequence of such entries follows each INDEX_HEADER, together forming the complete index.
 * The index follows the index root attribute or the index allocation attribute.
 *
 * Note: before NTFS 3.0 only the file name attribute was indexed.
 */
typedef struct {
/*  0	INDEX_ENTRY_HEADER; -- expanded because gcc does not support anonymous structs.*/
	union {		/* only valid when INDEX_ENTRY_END is not set. */
		leMFT_REF indexed_file;		/* MFT reference of the file described by
						   this index entry. Used for directory
						   indexes. */
		struct { /* used by views/indexes to locate the entry data. */
			le16 data_offset;	/* byte offset of the data relative to this
						   INDEX_ENTRY,
						   after the index key. */
			le16 data_length;	/* size of the data in bytes. */
			le32 reservedV;		/* reserved (zero). */
		} __attribute__((__packed__));
	} __attribute__((__packed__));
/*  8*/ le16 length;		 /* size of this index entry in bytes, a multiple of 8.
				    Includes the INDEX_ENTRY_HEADER and the optional child node
				    VCN. See below. */
/* 10*/ le16 key_length;		 /* size of the key value in the index entry in bytes,
				    after the reserved field. Not a multiple of 8. */
/* 12*/	INDEX_ENTRY_FLAGS ie_flags; /* INDEX_ENTRY_* flags bit field. */
/* 14*/	le16 reserved;		 /* reserved/8-byte alignment. */
/*	end of INDEX_ENTRY_HEADER */
/* 16*/	union {		/* key of the indexed attribute. Note: only present when the INDEX_ENTRY_END
			   bit in flags is not set. Before NTFS 3.0 the only valid key was
			   FILE_NAME_ATTR. NTFS 3.0+ also defines the following index keys: */
		FILE_NAME_ATTR file_name;/* $I30 index in directories. */
		SII_INDEX_KEY sii;	/* $SII index in $Secure. */
		SDH_INDEX_KEY sdh;	/* $SDH index in $Secure. */
		GUID object_id;		/* $O index in FILE_Extend/$ObjId:
					   object_id of the MFT record in the data part. */
		REPARSE_INDEX_KEY reparse;	/* $R index in
						   FILE_Extend/$Reparse. */
		SID sid;		/* $O index in FILE_Extend/$Quota:
					   SID of the owner of the user_id. */
		le32 owner_id;		/* $Q index in FILE_Extend/$Quota:
					   user_id of the owner of the quota control entry
					   in the data part. */
	} __attribute__((__packed__)) key;
	/* The (optional) index data is inserted here at creation time.
	leVCN vcn;	   if the INDEX_ENTRY_NODE bit is set in ie_flags,
			   the last eight bytes of this index entry contain the virtual cluster number
			   of the index block immediately preceding the current entry.

			   If key_length is zero, the vcn immediately follows the INDEX_ENTRY_HEADER.

			   The vcn address of the "ie" INDEX_ENTRY is
			   (char*)ie + le16_to_cpu(ie->length) - sizeof(VCN)
	*/
} __attribute__((__packed__)) INDEX_ENTRY;

/**
 * struct BITMAP_ATTR - Attribute: bitmap (0xb0).
 *
 * Contains a bit array (bit field).
 *
 * When used together with the index allocation attribute, each bit corresponds to one index block.
 * Therefore the number of bits in the bitmap × index block size / cluster size = the number of clusters in the index allocation
 * attribute.
 */
typedef struct {
	u8 bitmap[0];			/* bit array. */
} __attribute__((__packed__)) BITMAP_ATTR;

/**
 * enum PREDEFINED_REPARSE_TAGS - Predefined reparse point tags.
 *
 * A reparse point tag defines the type of a reparse point and also contains descriptive flags.
 *
 * A tag is an unsigned 32-bit value divided into three parts:
 *
 * 1. The least significant 16 bits (bits 0 to 15) specify the reparse point type.
 * 2. The next 12 bits (bits 16 to 27) are reserved.
 * 3. The most significant four bits are descriptive flags:
 *	Bit 28: directory bit. When set, indicates the directory is not a surrogate and can be used normally.
 *	Bit 29: name surrogate bit. When set, indicates the file name is an alias for another object in the system.
 *	Bit 30: high-latency bit. When set, indicates that accessing the first bytes of data will be slow
 *		(for example when the data is stored on tape).
 *	Bit 31: Microsoft bit. When set, the tag belongs to Microsoft. For user-defined tags this bit is zero.
 * 4. On Windows 10, bits 12 to 15 may be used to further describe the reparse point.
 */
typedef enum {
	IO_REPARSE_TAG_DIRECTORY	= const_cpu_to_le32(0x10000000),
	IO_REPARSE_TAG_IS_ALIAS		= const_cpu_to_le32(0x20000000),
	IO_REPARSE_TAG_IS_HIGH_LATENCY	= const_cpu_to_le32(0x40000000),
	IO_REPARSE_TAG_IS_MICROSOFT	= const_cpu_to_le32(0x80000000),

	IO_REPARSE_TAG_RESERVED_ZERO	= const_cpu_to_le32(0x00000000),
	IO_REPARSE_TAG_RESERVED_ONE	= const_cpu_to_le32(0x00000001),
	IO_REPARSE_TAG_RESERVED_RANGE	= const_cpu_to_le32(0x00000001),

	IO_REPARSE_TAG_CSV		= const_cpu_to_le32(0x80000009),
	IO_REPARSE_TAG_DEDUP		= const_cpu_to_le32(0x80000013),
	IO_REPARSE_TAG_DFS		= const_cpu_to_le32(0x8000000A),
	IO_REPARSE_TAG_DFSR		= const_cpu_to_le32(0x80000012),
	IO_REPARSE_TAG_HSM		= const_cpu_to_le32(0xC0000004),
	IO_REPARSE_TAG_HSM2		= const_cpu_to_le32(0x80000006),
	IO_REPARSE_TAG_MOUNT_POINT	= const_cpu_to_le32(0xA0000003),
	IO_REPARSE_TAG_NFS		= const_cpu_to_le32(0x80000014),
	IO_REPARSE_TAG_SIS		= const_cpu_to_le32(0x80000007),
	IO_REPARSE_TAG_SYMLINK		= const_cpu_to_le32(0xA000000C),
	IO_REPARSE_TAG_WIM		= const_cpu_to_le32(0x80000008),
	IO_REPARSE_TAG_DFM		= const_cpu_to_le32(0x80000016),
	IO_REPARSE_TAG_WOF		= const_cpu_to_le32(0x80000017),
	IO_REPARSE_TAG_WCI		= const_cpu_to_le32(0x80000018),
	IO_REPARSE_TAG_CLOUD		= const_cpu_to_le32(0x9000001A),
	IO_REPARSE_TAG_APPEXECLINK	= const_cpu_to_le32(0x8000001B),
	IO_REPARSE_TAG_GVFS		= const_cpu_to_le32(0x9000001C),
	IO_REPARSE_TAG_LX_SYMLINK	= const_cpu_to_le32(0xA000001D),
	IO_REPARSE_TAG_AF_UNIX		= const_cpu_to_le32(0x80000023),
	IO_REPARSE_TAG_LX_FIFO		= const_cpu_to_le32(0x80000024),
	IO_REPARSE_TAG_LX_CHR		= const_cpu_to_le32(0x80000025),
	IO_REPARSE_TAG_LX_BLK		= const_cpu_to_le32(0x80000026),

	IO_REPARSE_TAG_VALID_VALUES	= const_cpu_to_le32(0xf000ffff),
	IO_REPARSE_PLUGIN_SELECT	= const_cpu_to_le32(0xffff0fff),
} PREDEFINED_REPARSE_TAGS;

/**
 * struct REPARSE_POINT - Attribute: reparse point (0xc0).
 *
 * Note: may be resident or non-resident.
 */
typedef struct {
	le32 reparse_tag;		/* reparse point type (including flags). */
	le16 reparse_data_length;	/* size of the reparse data in bytes. */
	le16 reserved;			/* 8-byte alignment. */
	u8 reparse_data[0];		/* meaning depends on reparse_tag. */
} __attribute__((__packed__)) REPARSE_POINT;

/**
 * struct EA_INFORMATION - Attribute: extended attribute information (0xd0).
 *
 * Note: always resident.
 */
typedef struct {
	le16 ea_length;		/* size of the packed extended attributes in bytes. */
	le16 need_ea_count;	/* number of extended attributes with the NEED_EA bit set. */
	le32 ea_query_length;	/* buffer size in bytes needed when querying the
				   extended attributes with ZwQueryEaFile() in Windows NT/2k,
				   i.e. the size in bytes of the unpacked extended attributes. */
} __attribute__((__packed__)) EA_INFORMATION;

/**
 * enum EA_FLAGS - Extended attribute flags (8 bits).
 */
typedef enum {
	NEED_EA	= 0x80,		/* indicates that the file cannot be correctly parsed
				   without understanding the associated extended attributes. */
} __attribute__((__packed__)) EA_FLAGS;

/**
 * struct EA_ATTR - Attribute: extended attribute (EA) (0xe0).
 *
 * Like the attribute list and the index buffer list, the EA attribute value is a sequence of variable-length
 * EA_ATTR records.
 */
typedef struct {
	le32 next_entry_offset;	/* offset of the next EA_ATTR. */
	EA_FLAGS flags;		/* flags describing the EA. */
	u8 name_length;		/* length of the extended attribute name in bytes. */
	le16 value_length;	/* size of the EA value in bytes. */
	u8 name[0];		/* EA name. */
	u8 value[0];		/* EA value, immediately following the name. */
} __attribute__((__packed__)) EA_ATTR;

/**
 * struct PROPERTY_SET - Attribute: property set (0xf0).
 *
 * Originally intended to support Native Structure Storage (NSS), a feature that was removed during
 * the NTFS 3.0 beta.
 */
typedef struct {
	/* Not used by any feature, contents irrelevant. */
} __attribute__((__packed__)) PROPERTY_SET;

/**
 * struct LOGGED_UTILITY_STREAM - Attribute: logged utility stream (0x100).
 *
 * Note: may be resident or non-resident.
 *
 * Operations on this attribute are logged to the journal ($LogFile) like ordinary metadata changes.
 * Used by the Encrypting File System (EFS). All encrypted files have this attribute, with the name $EFS.
 */
typedef struct {
	/* Contents are determined by the creator. */
} __attribute__((__packed__)) LOGGED_UTILITY_STREAM;

/*
 * $EFS data structures:
 *
 * The following are the data structures contained in the logged utility stream (0x100) named "$EFS".
 *
 * The stream starts with an instance of EFS_ATTR_HEADER.
 *
 * At the offsets offset_to_ddf_array and offset_to_drf_array (if non-zero)
 * there is an EFS_DF_ARRAY_HEADER, followed by a number of data decryption/recovery fields.
 *
 * Each data decryption/recovery field starts with an EFS_DF_HEADER, and the next one can be found by
 * adding df_length to the current offset.
 *
 * A field contains an EFS_DF_CERTIFICATE_HEADER, a SID, an optional GUID, an optional container name,
 * a required user name and the encrypted FEK.
 *
 * Ntfs.sys treats EFS value lengths <= 0x54 or > 0x40000 as invalid.
 */

/**
 * struct EFS_ATTR_HEADER - "$EFS" header.
 *
 * Header of the logged utility stream (0x100) attribute named "$EFS".
 */
typedef struct {
/*  0*/	le32 length;		/* length of the EFS attribute in bytes. */
	le32 state;		/* always 0? */
	le32 version;		/* EFS version, always 2? */
	le32 crypto_api_version;	/* always 0? */
/* 16*/	u8 unknown4[16];	/* GUID of this encrypted file (created through
				   UuidCreate()). */
/* 32*/	u8 unknown5[16];	/* MD5 hash of the DDF? */
/* 48*/	u8 unknown6[16];	/* MD5 hash of the DRF? */
/* 64*/	le32 offset_to_ddf_array;/* byte offset to the data decryption field (DDF) array.
				   Zero = no DDF. */
	le32 offset_to_drf_array;/* byte offset to the data recovery field (DRF) array.
				   Zero = no DRF. */
	le32 reserved;		/* reserved. */
} __attribute__((__packed__)) EFS_ATTR_HEADER;

/**
 * struct EFS_DF_ARRAY_HEADER - DDF/DRF array header.
 */
typedef struct {
	le32 df_count;		/* number of data decryption/recovery fields in the array. */
} __attribute__((__packed__)) EFS_DF_ARRAY_HEADER;

/**
 * struct EFS_DF_HEADER - Data decryption/recovery field header.
 */
typedef struct {
/*  0*/	le32 df_length;		/* length of this data decryption/recovery field in bytes. */
	le32 cred_header_offset;	/* byte offset of the credential header. */
	le32 fek_size;		/* size of the encrypted file encryption key (FEK) in bytes. */
	le32 fek_offset;		/* byte offset of the FEK relative to the start of the field. */
/* 16*/	le32 unknown1;		/* always 0? Possibly padding. */
} __attribute__((__packed__)) EFS_DF_HEADER;

/**
 * struct EFS_DF_CREDENTIAL_HEADER - Credential header.
 */
typedef struct {
/*  0*/	le32 cred_length;	/* length of this credential in bytes. */
	le32 sid_offset;		/* byte offset of the user SID relative to the start of the structure.
				   Zero = no SID. */
/*  8*/	le32 type;		/* credential type:
					1 = CryptoAPI container.
					2 = unexpected type.
					3 = certificate thumbprint.
					other = unknown type. */
	union {
		/* CryptoAPI container. */
		struct {
/* 12*/			le32 container_name_offset;	/* byte offset of the container name relative to the
				   start of the structure (must not be zero). */
/* 16*/			le32 provider_name_offset;	/* byte offset of the provider name relative to the
				   start of the structure (must not be zero). */
			le32 public_key_blob_offset;	/* byte offset of the public key blob relative to
				   the start of the structure. */
/* 24*/			le32 public_key_blob_size;	/* size of the public key blob
				   in bytes. */
		} __attribute__((__packed__));
		/* Certificate thumbprint. */
		struct {
/* 12*/			le32 cert_thumbprint_header_size;	/* size of the certificate thumbprint
				   header in bytes. */
/* 16*/			le32 cert_thumbprint_header_offset;	/* byte offset of the certificate thumbprint
				   header relative to the start of the structure. */
			le32 unknown1;	/* always 0? Possibly padding. */
			le32 unknown2;	/* always 0? Possibly padding. */
		} __attribute__((__packed__));
	} __attribute__((__packed__));
} __attribute__((__packed__)) EFS_DF_CREDENTIAL_HEADER;

typedef EFS_DF_CREDENTIAL_HEADER EFS_DF_CRED_HEADER;

/**
 * struct EFS_DF_CERTIFICATE_THUMBPRINT_HEADER - Certificate thumbprint header.
 */
typedef struct {
/*  0*/	le32 thumbprint_offset;		/* byte offset of the thumbprint. */
	le32 thumbprint_size;		/* size of the thumbprint in bytes. */
/*  8*/	le32 container_name_offset;	/* byte offset of the container name relative to the start of the structure,
					   0 = no name. */
	le32 provider_name_offset;	/* byte offset of the cryptographic provider name relative to the start of
					   the structure, 0 = no name. */
/* 16*/	le32 user_name_offset;		/* byte offset of the user name relative to the start of the
					   structure, 0 = no user name.
					   (Also known as lpDisplayInformation.) */
} __attribute__((__packed__)) EFS_DF_CERTIFICATE_THUMBPRINT_HEADER;

typedef EFS_DF_CERTIFICATE_THUMBPRINT_HEADER EFS_DF_CERT_THUMBPRINT_HEADER;

typedef enum {
	INTX_SYMBOLIC_LINK =
		const_cpu_to_le64(0x014B4E4C78746E49ULL), /* "IntxLNK\1" */
	INTX_CHARACTER_DEVICE =
		const_cpu_to_le64(0x0052484378746E49ULL), /* "IntxCHR\0" */
	INTX_BLOCK_DEVICE =
		const_cpu_to_le64(0x004B4C4278746E49ULL), /* "IntxBLK\0" */
} INTX_FILE_TYPES;

typedef struct {
	INTX_FILE_TYPES magic;		/* Intx file magic number. */
	union {
		/* Character and block devices. */
		struct {
			le64 major;		/* major device number. */
			le64 minor;		/* minor device number. */
			void *device_end[0];	/* offsetof() marker. */
		} __attribute__((__packed__));
		/* Symbolic link. */
		ntfschar target[0];
	} __attribute__((__packed__));
} __attribute__((__packed__)) INTX_FILE;

#endif /* defined NTFSDRV_NTFS_FS_LAYOUT_H */
