/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_VNODE_CORE_H
#define NTFSDRV_NTFS_VNODE_CORE_H

/* Forward declarations */
typedef struct _ntfs_inode ntfsdrv_inode;

#include "ntfs_core_types.h"
#include "ntfs_fs_layout.h"
#include "ntfs_platform_support.h"
#include "ntfs_vol_context.h"
#include "ntfs_time_utils.h"

/**
 * enum ntfsdrv_inode_state_bits -
 *
 * Bit definitions for the state field in struct ntfsdrv_inode.
 * (f) = (f) = files only, (d) = directories only
 */
typedef enum {
	NI_Dirty,		/* 1: MFT 1: MFT record needs to be written to disk.*/
	NI_AttrList,		/* 1: MFT 1: MFT record contains an attribute list.*/
	NI_AttrListDirty,	/* 1: 1: attribute list needs to be written to the MFT record and then to disk.*/
	NI_FileNameDirty,	/* 1: FILE_NAME 1: FILE_NAME attribute needs to be updated in the index.*/
	NI_v3_Extensions,	/* 1: 1: v3.x extension fields are present.*/
	NI_TimesSet,		/* 1: 1: use the timestamps already set */
	NI_KnownSize,		/* 1: 1: set when the size fields are meaningful */
} ntfsdrv_inode_state_bits;

#define  test_nino_flag(ni, flag)	   test_bit(NI_##flag, (ni)->state)
#define   set_nino_flag(ni, flag)	    set_bit(NI_##flag, (ni)->state)
#define clear_nino_flag(ni, flag)	  clear_bit(NI_##flag, (ni)->state)

#define test_and_set_nino_flag(ni, flag)	\
				   test_and_set_bit(NI_##flag, (ni)->state)
#define test_and_clear_nino_flag(ni, flag)	\
				 test_and_clear_bit(NI_##flag, (ni)->state)

#define NInoDirty(ni)				  test_nino_flag(ni, Dirty)
#define NInoSetDirty(ni)			   set_nino_flag(ni, Dirty)
#define NInoClearDirty(ni)			 clear_nino_flag(ni, Dirty)
#define NInoTestAndSetDirty(ni)		  test_and_set_nino_flag(ni, Dirty)
#define NInoTestAndClearDirty(ni)	test_and_clear_nino_flag(ni, Dirty)

#define NInoAttrList(ni)			  test_nino_flag(ni, AttrList)
#define NInoSetAttrList(ni)			   set_nino_flag(ni, AttrList)
#define NInoClearAttrList(ni)			 clear_nino_flag(ni, AttrList)

#define  test_nino_al_flag(ni, flag)	 test_nino_flag(ni, AttrList##flag)
#define   set_nino_al_flag(ni, flag)	  set_nino_flag(ni, AttrList##flag)
#define clear_nino_al_flag(ni, flag)	clear_nino_flag(ni, AttrList##flag)

#define test_and_set_nino_al_flag(ni, flag)	\
				 test_and_set_nino_flag(ni, AttrList##flag)
#define test_and_clear_nino_al_flag(ni, flag)	\
			       test_and_clear_nino_flag(ni, AttrList##flag)

#define NInoAttrListDirty(ni)			    test_nino_al_flag(ni, Dirty)
#define NInoAttrListSetDirty(ni)		     set_nino_al_flag(ni, Dirty)
#define NInoAttrListClearDirty(ni)		   clear_nino_al_flag(ni, Dirty)
#define NInoAttrListTestAndSetDirty(ni)	    test_and_set_nino_al_flag(ni, Dirty)
#define NInoAttrListTestAndClearDirty(ni) test_and_clear_nino_al_flag(ni, Dirty)

#define NInoFileNameDirty(ni)                 test_nino_flag(ni, FileNameDirty)
#define NInoFileNameSetDirty(ni)               set_nino_flag(ni, FileNameDirty)
#define NInoFileNameClearDirty(ni)           clear_nino_flag(ni, FileNameDirty)
#define NInoFileNameTestAndSetDirty(ni)		\
				      test_and_set_nino_flag(ni, FileNameDirty)
#define NInoFileNameTestAndClearDirty(ni)	\
				    test_and_clear_nino_flag(ni, FileNameDirty)

/**
 * struct _ntfs_inode - NTFS in-memory NTFS inode structure.
 *
 * Used as an extension of the fields already present in a VFS inode.
 */
struct _ntfs_inode {
	u64 mft_no;		/* inode / MFT inode / MFT record number.*/
	MFT_RECORD *mrec;	/* actual MFT record of this inode.*/
	ntfsdrv_volume *vol;	/* pointer to the NTFS volume this inode belongs to.*/
	unsigned long state;	/* NTFS-specific flags describing this inode.
				   See ntfsdrv_inode_state_bits above.*/
	FILE_ATTR_FLAGS flags;	/* flags describing the file.
				   (copied from STANDARD_INFORMATION)*/
	/*
	 * Attribute list support (used by the attribute lookup functions).
	 * Initialized for all inodes with an attribute list during ntfsdrv_open_inode().
	 * Only valid when NI_AttrList is set in state.
	 */
	u32 attr_list_size;	/* length in bytes of the attribute list value.*/
	u8 *attr_list;		/* the attribute list value itself.*/
	/* The following fields are always valid.*/
	s32 nr_extents;		/* for a base MFT record, the number of attached extent inodes
				   (0 if none); -1 for an extent record.*/
	union {		/* this union is only used when nr_extents != 0.*/
		ntfsdrv_inode **extent_nis;/* nr_extents > 0 when nr_extents > 0, array of ntfs inodes of the loaded
					   inode extent MFT records belonging to this base
					   ntfs inode inode.*/
		ntfsdrv_inode *base_ni;	/* nr_extents == -1 when nr_extents == -1, the ntfs inode of the base MFT MFT
					   record.*/
	};

	/* The following fields are only valid for base inodes.*/

	/*
	 * These two fields are used to keep the filename index in sync and are guaranteed correct,
	 * but the values in the index itself may be wrong (Windows does not update them correctly either).
	 * For directories, holds the index size when the KnownSize flag is set.
	 */
	s64 data_size;		/* data size of the unnamed DATA attribute
				   (INDEX_ROOT for directories)*/
	s64 allocated_size;	/* allocated size stored in the filename index.
				   (Note: for normal or encrypted files it equals the allocated size of the unnamed data attribute;
				   for sparse or compressed files it equals the compressed size of the unnamed
				   data data attribute.)*/

	/*
	 * The following four fields are copies of the related fields of the STANDARD_INFORMATION attribute,
	 * used to synchronize that attribute with the FILE_NAME attribute in the index.
	 */
	ntfsdrv_time creation_time;
	ntfsdrv_time last_data_change_time;
	ntfsdrv_time last_mft_change_time;
	ntfsdrv_time last_access_time;
				/* NTFS 3.x NTFS 3.x extension fields */
				/* only valid when NI_v3_Extensions is set in state */
 	le32 owner_id;
	le32 security_id;
	le64 quota_charged;
	le64 usn;
};

typedef enum {
	NTFS_UPDATE_ATIME = 1 << 0,
	NTFS_UPDATE_MTIME = 1 << 1,
	NTFS_UPDATE_CTIME = 1 << 2,
} ntfsdrv_time_update_flags;

#define NTFS_UPDATE_MCTIME  (NTFS_UPDATE_MTIME | NTFS_UPDATE_CTIME)
#define NTFS_UPDATE_AMCTIME (NTFS_UPDATE_ATIME | NTFS_UPDATE_MCTIME)

extern ntfsdrv_inode *ntfsdrv_inode_base(ntfsdrv_inode *ni);

extern ntfsdrv_inode *ntfsdrv_inode_allocate(ntfsdrv_volume *vol);

extern ntfsdrv_inode *ntfsdrv_inode_open(ntfsdrv_volume *vol, const MFT_REF mref);

extern int ntfsdrv_inode_close(ntfsdrv_inode *ni);
extern int ntfsdrv_inode_close_in_dir(ntfsdrv_inode *ni, ntfsdrv_inode *dir_ni);

#if CACHE_NIDATA_SIZE

struct CACHED_GENERIC;

extern int ntfsdrv_inode_real_close(ntfsdrv_inode *ni);
extern void ntfsdrv_inode_invalidate(ntfsdrv_volume *vol, const MFT_REF mref);
extern void ntfsdrv_inode_nidata_free(const struct CACHED_GENERIC *cached);
extern int ntfsdrv_inode_nidata_hash(const struct CACHED_GENERIC *item);

#endif

extern ntfsdrv_inode *ntfsdrv_extent_inode_open(ntfsdrv_inode *base_ni,
		const leMFT_REF mref);

extern int ntfsdrv_inode_attach_all_extents(ntfsdrv_inode *ni);

extern void ntfsdrv_inode_mark_dirty(ntfsdrv_inode *ni);

extern void ntfsdrv_inode_update_times(ntfsdrv_inode *ni, ntfsdrv_time_update_flags mask);

extern int ntfsdrv_inode_sync(ntfsdrv_inode *ni);

extern int ntfsdrv_inode_add_attrlist(ntfsdrv_inode *ni);

extern int ntfsdrv_inode_free_space(ntfsdrv_inode *ni, int size);

extern int ntfsdrv_inode_get_times(ntfsdrv_inode *ni, char *value, size_t size);

extern int ntfsdrv_inode_set_times(ntfsdrv_inode *ni, const char *value,
			size_t size, int flags);

/* Debug */
#define debug_double_inode(num, type)
#define debug_cached_inode(ni)

#endif /* defined NTFSDRV_NTFS_VNODE_CORE_H */
