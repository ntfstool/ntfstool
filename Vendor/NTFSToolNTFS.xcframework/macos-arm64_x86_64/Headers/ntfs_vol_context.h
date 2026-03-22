/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_VOL_CONTEXT_H
#define NTFSDRV_NTFS_VOL_CONTEXT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
	/* do not #include <sys/mount.h> here: it conflicts with <linux/fs.h> */
#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif

/* Forward declarations */
typedef struct _ntfs_volume ntfsdrv_volume;

#include "ntfs_driver_params.h"
#include "ntfs_core_types.h"
#include "ntfs_platform_support.h"
#include "ntfs_block_dev.h"
#include "ntfs_vnode_core.h"
#include "ntfs_attr_core.h"
#include "ntfs_btree_index.h"

/**
 * enum ntfsdrv_mount_flags - mount flags for ntfsdrv_mount().
 */
enum {
	NTFS_MNT_NONE                   = 0x00000000,
	NTFS_MNT_RDONLY                 = 0x00000001,
	NTFS_MNT_MAY_RDONLY             = 0x02000000, /* allow fallback to read-only */
	NTFS_MNT_FORENSIC               = 0x04000000, /* do not modify the volume during mount */
	NTFS_MNT_EXCLUSIVE              = 0x08000000,
	NTFS_MNT_RECOVER                = 0x10000000,
	NTFS_MNT_IGNORE_HIBERFILE       = 0x20000000,
};
typedef unsigned long ntfsdrv_mount_flags;

/**
 * enum ntfsdrv_mounted_flags - flags returned by ntfsdrv_check_if_mounted().
 */
typedef enum {
	NTFS_MF_MOUNTED		= 1,	/* device is mounted.*/
	NTFS_MF_ISROOT		= 2,	/* device is mounted as the system root.*/
	NTFS_MF_READONLY	= 4,	/* device is mounted read-only.*/
} ntfsdrv_mounted_flags;

extern int ntfsdrv_check_if_mounted(const char *file, unsigned long *mnt_flags);

typedef enum {
	NTFS_VOLUME_OK			= 0,
	NTFS_VOLUME_SYNTAX_ERROR	= 11,
	NTFS_VOLUME_NOT_NTFS		= 12,
	NTFS_VOLUME_CORRUPT		= 13,
	NTFS_VOLUME_HIBERNATED		= 14,
	NTFS_VOLUME_UNCLEAN_UNMOUNT	= 15,
	NTFS_VOLUME_LOCKED		= 16,
	NTFS_VOLUME_RAID		= 17,
	NTFS_VOLUME_UNKNOWN_REASON	= 18,
	NTFS_VOLUME_NO_PRIVILEGE	= 19,
	NTFS_VOLUME_OUT_OF_MEMORY	= 20,
	NTFS_VOLUME_FUSE_ERROR		= 21,
	NTFS_VOLUME_INSECURE		= 22
} ntfsdrv_volume_status;

typedef enum {
	NTFS_FILES_INTERIX,
	NTFS_FILES_WSL,
} ntfsdrv_volume_special_files;

/**
 * enum ntfsdrv_volume_state_bits - bit definitions for the state field in struct ntfsdrv_volume.
 */
typedef enum {
	NV_ReadOnly,		/* 1: 1: volume is read-only.*/
	NV_CaseSensitive,	/* 1: 1: volume is mounted case-sensitive.*/
	NV_LogFileEmpty,	/* 1: $logFile 1: $logFile journal is empty.*/
	NV_ShowSysFiles,	/* 1: 1: show NTFS metadata files.*/
	NV_ShowHidFiles,	/* 1: 1: show files marked hidden.*/
	NV_HideDotFiles,	/* 1: 1: set the hidden flag on files starting with a dot.*/
	NV_Compression,		/* 1: 1: compression is allowed.*/
	NV_NoFixupWarn,		/* 1: 1: do not log fixup errors.*/
	NV_FreeSpaceKnown,	/* 1: 1: free space size is known.*/
} ntfsdrv_volume_state_bits;

#define  test_nvol_flag(nv, flag)	 test_bit(NV_##flag, (nv)->state)
#define   set_nvol_flag(nv, flag)	  set_bit(NV_##flag, (nv)->state)
#define clear_nvol_flag(nv, flag)	clear_bit(NV_##flag, (nv)->state)

#define NVolReadOnly(nv)		 test_nvol_flag(nv, ReadOnly)
#define NVolSetReadOnly(nv)		  set_nvol_flag(nv, ReadOnly)
#define NVolClearReadOnly(nv)		clear_nvol_flag(nv, ReadOnly)

#define NVolCaseSensitive(nv)		 test_nvol_flag(nv, CaseSensitive)
#define NVolSetCaseSensitive(nv)	  set_nvol_flag(nv, CaseSensitive)
#define NVolClearCaseSensitive(nv)	clear_nvol_flag(nv, CaseSensitive)

#define NVolLogFileEmpty(nv)		 test_nvol_flag(nv, LogFileEmpty)
#define NVolSetLogFileEmpty(nv)		  set_nvol_flag(nv, LogFileEmpty)
#define NVolClearLogFileEmpty(nv)	clear_nvol_flag(nv, LogFileEmpty)

#define NVolShowSysFiles(nv)		 test_nvol_flag(nv, ShowSysFiles)
#define NVolSetShowSysFiles(nv)		  set_nvol_flag(nv, ShowSysFiles)
#define NVolClearShowSysFiles(nv)	clear_nvol_flag(nv, ShowSysFiles)

#define NVolShowHidFiles(nv)		 test_nvol_flag(nv, ShowHidFiles)
#define NVolSetShowHidFiles(nv)		  set_nvol_flag(nv, ShowHidFiles)
#define NVolClearShowHidFiles(nv)	clear_nvol_flag(nv, ShowHidFiles)

#define NVolHideDotFiles(nv)		 test_nvol_flag(nv, HideDotFiles)
#define NVolSetHideDotFiles(nv)		  set_nvol_flag(nv, HideDotFiles)
#define NVolClearHideDotFiles(nv)	clear_nvol_flag(nv, HideDotFiles)

#define NVolCompression(nv)		 test_nvol_flag(nv, Compression)
#define NVolSetCompression(nv)		  set_nvol_flag(nv, Compression)
#define NVolClearCompression(nv)	clear_nvol_flag(nv, Compression)

#define NVolNoFixupWarn(nv)		 test_nvol_flag(nv, NoFixupWarn)
#define NVolSetNoFixupWarn(nv)		  set_nvol_flag(nv, NoFixupWarn)
#define NVolClearNoFixupWarn(nv)	clear_nvol_flag(nv, NoFixupWarn)

#define NVolFreeSpaceKnown(nv)		 test_nvol_flag(nv, FreeSpaceKnown)
#define NVolSetFreeSpaceKnown(nv)	  set_nvol_flag(nv, FreeSpaceKnown)
#define NVolClearFreeSpaceKnown(nv)	clear_nvol_flag(nv, FreeSpaceKnown)

/*
 * NTFS NTFS versions 1.1 and 1.2 are used by Windows NT4.
 * NTFS NTFS versions 2.x are used by Windows 2000 Beta.
 * NTFS NTFS version 3.0 is used by Windows 2000.
 * NTFS NTFS version 3.1 is used by Windows XP, 2003 and Vista.
 */

#define NTFS_V1_1(major, minor) ((major) == 1 && (minor) == 1)
#define NTFS_V1_2(major, minor) ((major) == 1 && (minor) == 2)
#define NTFS_V2_X(major, minor) ((major) == 2)
#define NTFS_V3_0(major, minor) ((major) == 3 && (minor) == 0)
#define NTFS_V3_1(major, minor) ((major) == 3 && (minor) == 1)

#define NTFS_BUF_SIZE 8192

/**
 * struct _ntfs_volume - structure describing an open volume in memory.
 */
struct _ntfs_volume {
	union {
		struct ntfsdrv_device *dev;	/* NTFS device associated with the volume.*/
		void *sb;	/* kernel porting compatibility.*/
	};
	char *vol_name;		/* volume name.*/
	unsigned long state;	/* NTFS-specific flags describing this volume.
				   See ntfsdrv_volume_state_bits above.*/

	ntfsdrv_inode *vol_ni;	/* ntfsdrv_inode structure of FILE_Volume.*/
	u8 major_ver;		/* NTFS major version of the volume.*/
	u8 minor_ver;		/* NTFS minor version of the volume.*/
	le16 flags;		/* VOLUME_* array of VOLUME_* flag bits.*/

	u16 sector_size;	/* sector size in bytes.*/
	u8 sector_size_bits;	/* log2 of the sector size.*/
	u32 cluster_size;	/* cluster size in bytes.*/
	u32 mft_record_size;	/* MFT MFT record size in bytes.*/
	u32 indx_record_size;	/* INDX INDX record size in bytes.*/
	u8 cluster_size_bits;	/* log2 of the cluster size.*/
	u8 mft_record_size_bits;/* MFT log2 of the MFT record size.*/
	u8 indx_record_size_bits;/* INDX log2 of the INDX record size.*/

	/* Variables used by the cluster and MFT allocators.*/
	u8 mft_zone_multiplier;	/* initial MFT zone multiplier.*/
	u8 full_zones;		/* cluster zones that are full.*/
	s64 mft_data_pos;	/* allocation start position for the next MFT record.*/
	LCN mft_zone_start;	/* MFT first cluster of the MFT zone.*/
	LCN mft_zone_end;	/* MFT first cluster after the MFT zone.*/
	LCN mft_zone_pos;	/* MFT current position in the MFT zone.*/
	LCN data1_zone_pos;	/* current position in the first data zone.*/
	LCN data2_zone_pos;	/* current position in the second data zone.*/

	s64 nr_clusters;	/* total clusters in the volume, also the number of bits in lcn_bitmap.
				   */
	ntfsdrv_inode *lcnbmp_ni;	/* ntfsdrv_inode structure of FILE_Bitmap.*/
	ntfsdrv_attr *lcnbmp_na;	/* ntfsdrv_attr structure of the data attribute of FILE_Bitmap. Each bit
				   stands for one cluster on the volume,
				   bit 0 is LCN 0, and a set bit means allocated.*/

	LCN mft_lcn;		/* FILE_MFT data logical cluster number of the FILE_MFT data attribute.*/
	ntfsdrv_inode *mft_ni;	/* ntfsdrv_inode structure of FILE_MFT.*/
	ntfsdrv_attr *mft_na;	/* ntfsdrv_attr structure of the FILE_MFT data attribute.*/
	ntfsdrv_attr *mftbmp_na;	/* ntfsdrv_attr structure of the FILE_MFT bitmap attribute.
				   Each bit stands for one MFT record in $DATA,
				   bit 0 is MFT record 0, and a set bit means in use.*/

	ntfsdrv_inode *secure_ni;	/* ntfsdrv_inode structure of the $Secure file.*/
	ntfsdrv_index_context *secure_xsii; /* $Secure:$SII index context.*/
	ntfsdrv_index_context *secure_xsdh; /* $Secure:$SDH index context.*/
	int secure_reentry;  /* re-entry guard */
	unsigned int secure_flags;  /* security flags, see security.h for values.*/

	int mftmirr_size;	/* FILE_MFTMirr size of FILE_MFTMirr in MFT records.*/
	LCN mftmirr_lcn;	/* FILE_MFTMirr data logical cluster number of the FILE_MFTMirr data attribute.*/
	ntfsdrv_inode *mftmirr_ni;	/* ntfsdrv_inode structure of FILE_MFTMirr.*/
	ntfsdrv_attr *mftmirr_na;	/* ntfsdrv_attr structure of the FILE_MFTMirr data
				   attribute.*/

	ntfschar *upcase;	/* uppercase equivalence table for all 65536 two-byte Unicode characters.
				   Taken from FILE_UpCase.*/
	u32 upcase_len;		/* number of Unicode characters in the uppercase table.*/
	ntfschar *locase;	/* lowercase equivalence table for all 65536 two-byte Unicode characters.
				   Only present when the case_ignore option is set.*/

	ATTR_DEF *attrdef;	/* attribute definitions, taken from FILE_AttrDef.*/
	s32 attrdef_len;	/* size in bytes of the attribute definition table.*/

	s64 free_clusters; 	/* tracks free clusters, greatly improving statfs() performance.*/
	s64 free_mft_records; 	/* likewise, tracks free MFT records.*/
	BOOL efs_raw;		/* volume mounted raw to access EFS-encrypted files.*/
	ntfsdrv_volume_special_files special_files; /* how special files are implemented.*/
	const char *abs_mnt_point; /* mount point.*/
#ifdef XATTR_MAPPINGS
	struct XATTRMAPPING *xattr_mapping;
#endif /* XATTR_MAPPINGS */
#if CACHE_INODE_SIZE
	struct CACHE_HEADER *xinode_cache;
#endif
#if CACHE_NIDATA_SIZE
	struct CACHE_HEADER *nidata_cache;
#endif
#if CACHE_LOOKUP_SIZE
	struct CACHE_HEADER *lookup_cache;
#endif
#if CACHE_SECURID_SIZE
	struct CACHE_HEADER *securid_cache;
#endif
#if CACHE_LEGACY_SIZE
	struct CACHE_HEADER *legacy_cache;
#endif
};

extern const char *ntfsdrv_home;

extern ntfsdrv_volume *ntfsdrv_volume_alloc(void);

extern ntfsdrv_volume *ntfsdrv_volume_startup(struct ntfsdrv_device *dev,
		ntfsdrv_mount_flags flags);

extern ntfsdrv_volume *ntfsdrv_device_mount(struct ntfsdrv_device *dev,
		ntfsdrv_mount_flags flags);

extern ntfsdrv_volume *ntfsdrv_mount(const char *name, ntfsdrv_mount_flags flags);
extern int ntfsdrv_umount(ntfsdrv_volume *vol, const BOOL force);

extern int ntfsdrv_version_is_supported(ntfsdrv_volume *vol);
extern int ntfsdrv_volume_check_hiberfile(ntfsdrv_volume *vol, int verbose);
extern int ntfsdrv_logfile_reset(ntfsdrv_volume *vol);

extern int ntfsdrv_volume_write_flags(ntfsdrv_volume *vol, const le16 flags);

extern int ntfsdrv_volume_error(int err);
extern void ntfsdrv_mount_error(const char *vol, const char *mntpoint, int err);

extern int ntfsdrv_volume_get_free_space(ntfsdrv_volume *vol);
extern int ntfsdrv_volume_rename(ntfsdrv_volume *vol, const ntfschar *label,
		int label_len);

extern int ntfsdrv_set_shown_files(ntfsdrv_volume *vol,
		BOOL show_sys_files, BOOL show_hid_files, BOOL hide_dot_files);
extern int ntfsdrv_set_locale(void);

#endif /* defined NTFSDRV_NTFS_VOL_CONTEXT_H */
