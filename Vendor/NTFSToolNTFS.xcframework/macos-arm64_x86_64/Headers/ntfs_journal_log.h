/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_JOURNAL_LOG_H
#define NTFSDRV_NTFS_JOURNAL_LOG_H

#include "ntfs_core_types.h"
#include "ntfs_endian_utils.h"
#include "ntfs_fs_layout.h"

/*
 * Log file ($LogFile) layout:
 *
 * The first two pages are restart pages (one restart area each). After a clean unmount the two restart areas should be identical,
 * except that the update sequence number in the update sequence array may differ.
 *
 * After the restart pages come the log record pages (each starting with a log record header), up to the log file size.
 * A freshly formatted volume does not use every page, but all pages get used as the volume ages.
 * When the log file is full, old records are cleared from the head (by raising oldest_lsn),
 * and writing continues from the start of the file. The log file is effectively treated as a circular structure.
 *
 * Note: Windows NT/2000/XP all use log file version 1.1 but accept versions <= 1.x.
 * We only support version 1.1. If the restart page marks the log file as clean, the version does not matter,
 * and the log file can be reinitialized and restarted at version 1.1.
 */

/* $LogFile constants.*/
#define MaxLogFileSize		0x100000000ULL
#define DefaultLogPageSize	4096
#define MinLogRecordPages	48

/**
 * struct RESTART_PAGE_HEADER - log file restart page header.
 *
 * Starting structure of a restart area.
 */
typedef struct {
/*Ofs*/
/*  0	NTFS_RECORD; -- expanded because gcc does not support anonymous structs.*/
/*  0*/	NTFS_RECORD_TYPES magic;/* magic is "RSTR".*/
/*  4*/	le16 usa_ofs;		/* See the NTFS_RECORD definition. On creation, set to
				   immediately after this header (no alignment).*/
/*  6*/	le16 usa_count;		/* See the NTFS_RECORD definition.*/

/*  8*/	leLSN chkdsk_lsn;	/* chkdsk found the last log sequence number.
				   Only used when the magic becomes "CHKD",
				   zero otherwise.*/
/* 16*/	le32 system_page_size;	/* System page size in bytes when the log file was created,
				   must be >= 512 and a power of 2. Used to compute
				   USA size, add it to usa_ofs,
				   and verify the result is < restart_area_offset.*/
/* 20*/	le32 log_page_size;	/* Log file page size in bytes,
				   must be >= 512 and a power of 2.
				   defaults to 4096 when the system page size is between 4096 and 8192,
				   otherwise set to the system page size.*/
/* 24*/	le16 restart_area_offset;/* Byte offset from this header to the RESTART_AREA,
				   must be 8-byte aligned. Set to just after the USA on creation.*/
/* 26*/	sle16 minor_ver;	/* Log file minor version number. Only 1
				   checked when the major version is 1.*/
/* 28*/	sle16 major_ver;	/* Log file major version number. Only 1.1 is supported.*/
/* 30*/	le16 usn;
/* sizeof() = 32 (0x20) bytes */
} __attribute__((__packed__)) RESTART_PAGE_HEADER;

/*
 * Log client index constant meaning no client record in this client array.
 * Inside a client record it also means no other client record precedes or follows this one.
 */
#define LOGFILE_NO_CLIENT_CPU	0xffff
#define LOGFILE_NO_CLIENT	const_cpu_to_le16(LOGFILE_NO_CLIENT_CPU)

/*
 * Known RESTART_AREA_* flags (16-bit) describing log file state.
 */
enum {
	RESTART_VOLUME_IS_CLEAN	= const_cpu_to_le16(0x0002),
	RESTART_SPACE_FILLER	= 0xffff, /* gcc: force the enum width to 16 bits.*/
} __attribute__((__packed__));

typedef le16 RESTART_AREA_FLAGS;

/**
 * struct RESTART_AREA - log file restart area record.
 *
 * Offset of this record = RESTART_PAGE_HEADER offset + restart_area_offset.
 */
typedef struct {
/*Ofs*/
/*  0*/	leLSN current_lsn;	/* Current (latest) LSN in the log when the restart area was last written.
				   Set to 0 on creation.*/
/*  8*/	le16 log_clients;	/* Number of client records in the following log client record array.
				   Must be 1.*/
/* 10*/	le16 client_free_list;	/* Index of the first free record in the free client record array.
				   LOGFILE_NO_CLIENT means no free records.
				   Win2k and earlier: 0 for a clean volume (i.e. the only
				   client record is free, log closed = clean).
				   Dirty volume: LOGFILE_NO_CLIENT (log still open).
				   WinXP and later: the log is always open, so this value is always
				   LOGFILE_NO_CLIENT.*/
/* 12*/	le16 client_in_use_list;/* Index of the first in-use record in the in-use client record array.
				   LOGFILE_NO_CLIENT means no in-use records.
				   Win2k and earlier: LOGFILE_NO_CLIENT for a clean volume.
				   Dirty volume: 0.
				   WinXP and later: always 0.*/
/* 14*/	RESTART_AREA_FLAGS flags;/* Flags that modify LFS behavior.
				   Win2k and earlier: always 0.
				   WinXP and later: on clean shutdown set the
				   RESTART_VOLUME_IS_CLEAN bit, cleared on mount.
				   So checking whether the log is clean does not require the Windows version:
				   log closed = clean; open with this bit set = clean;
				   open with this bit clear = almost certainly dirty.*/
/* 16*/	le32 seq_number_bits;	/* Number of bits used by sequence numbers. Computed as:
				   67 - the number of bits needed to store the log file size in bytes.
				   Can be used with file_size for a consistency check.*/
/* 20*/	le16 restart_area_length;/* Total length of the restart area (including the client array).
				   Verify when versions match:
				   restart_area_offset + restart_area_length
				   <= system_page_size;
				   restart_area_length >= client_array_offset +
				   (log_clients * sizeof(client record)).*/
/* 22*/	le16 client_array_offset;/* Offset from the start of this record to the first client record.
				   Must be 8-byte aligned.
				   Win2k and earlier: 0x30;
				   WinXP and later: 0x40 (16 extra bytes,
				   probably because the RESTART_AREA record grew after XP).*/
/* 24*/	sle64 file_size;	/* Usable size of the log file in bytes.
				   Must be rounded down to a multiple of log_page_size,
				   and large enough for at least two restart pages and 48 log record pages.*/
/* 32*/	le32 last_lsn_data_length;/* Data length of the last LSN (excluding the log record header).
				   Set to 0 on creation.*/
/* 36*/	le16 log_record_header_length;/* Size of the log record header in bytes.
				   Must be a multiple of 8 when versions match.
				   Set to sizeof(LOG_RECORD_HEADER) on creation sizeof(LOG_RECORD_HEADER)
				   and 8-byte aligned.*/
/* 38*/	le16 log_page_data_offset;/* Offset where data starts in a log record page, must be a multiple of 8.
				   Set to just after the log record page's USA on creation.*/
/* 40*/	le32 restart_log_open_count;/* Counter incremented on each log file restart
				   (incremented each time the log file is opened on mount).
				   Set to a random value on creation.*/
/* 44*/	le32 reserved;		/* Reserved/8-byte alignment.*/
/* sizeof() = 48 (0x30) bytes */
} __attribute__((__packed__)) RESTART_AREA;

/**
 * struct LOG_CLIENT_RECORD - log client record.
 *
 * Offset of this record = RESTART_AREA offset + client_array_offset.
 */
typedef struct {
/*Ofs*/
/*  0*/	leLSN oldest_lsn;	/* Oldest LSN needed by this client. Set to 0 on creation.*/
/*  8*/	leLSN client_restart_lsn;/* LSN from which this client needs to restart the volume,
				   i.e. the current position in the log file.
				   Set to 0 on creation.*/
/* 16*/	le16 prev_client;	/* Offset of the previous log client record.
				   LOGFILE_NO_CLIENT means this is the first,
				   always LOGFILE_NO_CLIENT.*/
/* 18*/	le16 next_client;	/* Offset of the next log client record.
				   LOGFILE_NO_CLIENT means this is the last,
				   always LOGFILE_NO_CLIENT.*/
/* 20*/	le16 seq_number;	/* Win2k and earlier: zeroed on each log restart,
				   incremented on unmount (dirty=0, clean=1).
				   WinXP and later: always 0.*/
/* 22*/	u8 reserved[6];		/* Reserved/alignment.*/
/* 28*/	le32 client_name_length;/* Length of the client name in bytes, should always be 8.*/
/* 32*/	ntfschar client_name[64];/* Unicode format client name,
				   should always be "NTFS", remaining bytes 0.*/
/* sizeof() = 160 (0xa0) bytes */
} __attribute__((__packed__)) LOG_CLIENT_RECORD;

/**
 * struct RECORD_PAGE_HEADER - log page record page header.
 *
 * Each log page starts with this header, followed by multiple LOG_RECORD structures
 *(starting at offset 0x40, i.e. this structure plus the USA, 8-byte aligned).
 */
typedef struct {
/*  0	NTFS_RECORD; -- expanded because gcc does not support anonymous structs.*/
	NTFS_RECORD_TYPES magic;/* magic is usually "RCRD".*/
	le16 usa_ofs;		/* See the NTFS_RECORD definition. On creation, set to
				   immediately after this header (no alignment).*/
	le16 usa_count;		/* See the NTFS_RECORD definition.*/

	union {
		leLSN last_lsn;
		sle64 file_offset;
	} __attribute__((__packed__)) copy;
	le32 flags;
	le16 page_count;
	le16 page_position;
	le16 next_record_offset;
	le16 reserved[3];
	leLSN last_end_lsn;
} __attribute__((__packed__)) RECORD_PAGE_HEADER;

/**
 * enum LOG_RECORD_FLAGS - possible 16-bit flags of a log record.
 *
 * Some flags describe the type of update being logged.
 */
typedef enum {
	LOG_RECORD_MULTI_PAGE = const_cpu_to_le16(0x0001),
		/* The following flags were introduced in Windows 10 */
	LOG_RECORD_DELETING =	const_cpu_to_le16(0x0002),
	LOG_RECORD_ADDING =	const_cpu_to_le16(0x0004),
	LOG_RECORD_SIZE_PLACE_HOLDER = 0xffff,
		/* Only used to make gcc define the flags as 16-bit.*/
} __attribute__((__packed__)) LOG_RECORD_FLAGS;

/**
 * struct LOG_CLIENT_ID - structure identifying a log client.
 */
typedef struct {
	le16 seq_number;
	le16 client_index;
} __attribute__((__packed__)) LOG_CLIENT_ID;

/*
 * LOG_RECORD_TYPE: log record type
 */

enum {
	LOG_STANDARD =		const_cpu_to_le32(1),
	LOG_CHECKPOINT =	const_cpu_to_le32(2),
	LOG_RECORD_TYPE_PLACE_HOLDER = 0xffffffffU
} ;
typedef le32 LOG_RECORD_TYPE;

/*
 * ATTRIBUTE_FLAGS: flags describing the type of NTFS record being updated.
 * These flags were introduced in Vista; two are currently known.
 */

enum {
	ACTS_ON_MFT =		const_cpu_to_le16(2),
	ACTS_ON_INDX =		const_cpu_to_le16(8),
	ATTRIBUTE_FLAGS_PLACE_HOLDER = 0xffff,
} ;
typedef le16 ATTRIBUTE_FLAGS;

#define LOG_RECORD_HEAD_SZ 0x30 /* struct LOG_RECORD header size */

/**
 * struct LOG_RECORD - log record header.
 *
 * Each log record has a fixed size of 0x70 bytes.
 */
typedef struct {
	leLSN this_lsn;
	leLSN client_previous_lsn;
	leLSN client_undo_next_lsn;
	le32 client_data_length;
	LOG_CLIENT_ID client_id;
	LOG_RECORD_TYPE record_type;
	le32 transaction_id;
	LOG_RECORD_FLAGS log_record_flags;
	le16 reserved_or_alignment[3];
/* Offset 0x30 0x30 */
	le16 redo_operation;
	le16 undo_operation;
	le16 redo_offset;
	le16 redo_length;
	union {
		struct {
			le16 undo_offset;
			le16 undo_length;
			le16 target_attribute;
			le16 lcns_to_follow;   /* Number of lcn_list lcn_list
					      entries following this one.*/
/* Offset 0x40 0x40 */
			le16 record_offset;
			le16 attribute_offset;
			le16 cluster_index;
			ATTRIBUTE_FLAGS attribute_flags;
			leVCN target_vcn;
/* Offset 0x50 0x50 */
			leLCN lcn_list[0]; /* Only present when lcns_to_follow lcns_to_follow
						is non-zero.*/
		} __attribute__((__packed__));
		struct {
			leLSN transaction_lsn;
			leLSN attributes_lsn;
			leLSN names_lsn;
			leLSN dirty_pages_lsn;
			le64 unknown_list[0];
		} __attribute__((__packed__));
	} __attribute__((__packed__));
} __attribute__((__packed__)) LOG_RECORD;

/**
 * struct BITMAP_ACTION - bitmap change being logged
 */

struct BITMAP_ACTION {
	le32 firstbit;
	le32 count;
} ;

/**
 * struct ATTR - attribute record.
 *
 * The attribute record format changed in Windows 10.
 * The old format is 44 bytes long and has unaligned 8-byte fields, causing alignment problems in arrays.
 * The new format is shorter and has no such problem. The format can generally be told from the size.
 */
typedef struct {	/* Win10 and earlier format (44 bytes)*/
	le64 unknown1;
	le64 unknown2;
	le64 inode;
	leLSN lsn;
	le32 unknown3;
	le32 type;
	le32 unknown4;
} __attribute__((__packed__)) ATTR_OLD;

typedef struct {	/* Win10 and later format (40 bytes)*/
	le64 unknown1;
	le64 unknown2;
	le32 type;
	le32 unknown3;
	le64 inode;
	leLSN lsn;
} __attribute__((__packed__)) ATTR_NEW;

extern BOOL ntfsdrv_check_logfile(ntfsdrv_attr *log_na, RESTART_PAGE_HEADER **rp);
extern BOOL ntfsdrv_is_logfile_clean(ntfsdrv_attr *log_na, RESTART_PAGE_HEADER *rp);
extern int ntfsdrv_empty_logfile(ntfsdrv_attr *na);

#endif /* defined NTFSDRV_NTFS_JOURNAL_LOG_H */
