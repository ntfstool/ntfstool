/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_ATTR_CORE_H
#define NTFSDRV_NTFS_ATTR_CORE_H

/* Forward declarations */
typedef struct _ntfs_attr ntfsdrv_attr;
typedef struct _ntfs_attr_search_ctx ntfsdrv_attr_search_ctx;

#include "ntfs_core_types.h"
#include "ntfs_vnode_core.h"
#include "ntfs_unicode_trans.h"
#include "ntfs_runlist_map.h"
#include "ntfs_vol_context.h"
#include "ntfs_diag_debug.h"
#include "ntfs_logger.h"

extern ntfschar AT_UNNAMED[];
extern ntfschar STREAM_SDS[];

/* Global constant for the little-endian Unicode string $TXF_DATA. */
extern ntfschar TXF_DATA[10];

/**
 * enum ntfsdrv_lcn_special_values - ntfsdrv_rl_vcn_to_lcn() and
 * ntfsdrv_attr_vcn_to_lcn() special return values.
 */
typedef enum {
	LCN_HOLE		= -1,	/* Must stay the largest value. */
	LCN_RL_NOT_MAPPED	= -2,
	LCN_ENOENT		= -3,
	LCN_EINVAL		= -4,
	LCN_EIO			= -5,
} ntfsdrv_lcn_special_values;

typedef enum {			/* How holes are handled when extending */
	HOLES_NO,
	HOLES_OK,
	HOLES_DELAY,
	HOLES_NONRES
} hole_type;

/**
 * struct ntfsdrv_attr_search_ctx - search context used by attribute search functions
 * @mrec:	buffer containing the MFT record to search
 * @attr:	@mrec attribute record in @mrec at which to start/continue the search
 * @is_first:	if TRUE, search starts at @attr, otherwise after @attr
 *
 * Before the first call to an attribute search function, the structure must be zeroed. Point @mrec at the MFT record to search,
 * MFT @attr at the first attribute in @mrec (not needed for _first() functions),
 * and set @is_first to TRUE (not needed for _first() functions).
 *
 * If @is_first is TRUE, the search starts at @attr; if FALSE, it starts
 * after @attr. This allows calling again after the first call without modifying the search context
 * to automatically get the next matching attribute.
 */
struct _ntfs_attr_search_ctx {
	MFT_RECORD *mrec;
	ATTR_RECORD *attr;
	BOOL is_first;
	ntfsdrv_inode *ntfsdrv_ino;
	ATTR_LIST_ENTRY *al_entry;
	ntfsdrv_inode *base_ntfs_ino;
	MFT_RECORD *base_mrec;
	ATTR_RECORD *base_attr;
};

extern void ntfsdrv_attr_reinit_search_ctx(ntfsdrv_attr_search_ctx *ctx);
extern ntfsdrv_attr_search_ctx *ntfsdrv_attr_get_search_ctx(ntfsdrv_inode *ni,
		MFT_RECORD *mrec);
extern void ntfsdrv_attr_put_search_ctx(ntfsdrv_attr_search_ctx *ctx);

extern int ntfsdrv_attr_lookup(const ATTR_TYPES type, const ntfschar *name,
		const u32 name_len, const IGNORE_CASE_BOOL ic,
		const VCN lowest_vcn, const u8 *val, const u32 val_len,
		ntfsdrv_attr_search_ctx *ctx);

extern int ntfsdrv_attr_position(const ATTR_TYPES type, ntfsdrv_attr_search_ctx *ctx);

extern ATTR_DEF *ntfsdrv_attr_find_in_attrdef(const ntfsdrv_volume *vol,
		const ATTR_TYPES type);

/**
 * ntfsdrv_attrs_walk - syntactic sugar for walking all attributes in an inode
 * @ctx:	initialized attribute search context
 *
 * Returns 0 on success, -1 on error with errno set.
 *
 * Example: enumerate all attributes of an open ntfs inode @ni:
 *
 *	int err;
 *	ntfsdrv_attr_search_ctx *ctx = ntfsdrv_attr_get_search_ctx(ni, NULL);
 *	if (!ctx)
 *		// // error code is in errno, handle it here.
 *	while (!(err = ntfsdrv_attrs_walk(ctx))) {
 *		ATTR_RECORD *attr = ctx->attr;
 *		// attr // attr now holds the next attribute, process as needed.
 *	}
 *	if (err && errno != ENOENT)
 *		// // an error occurred, handle it.
 *	// // all attributes have been walked.
 */
static __inline__ int ntfsdrv_attrs_walk(ntfsdrv_attr_search_ctx *ctx)
{
	return ntfsdrv_attr_lookup(AT_UNUSED, NULL, 0, CASE_SENSITIVE, 0,
			NULL, 0, ctx);
}

/**
 * struct ntfsdrv_attr - in-memory non-resident attribute structure
 * @rl:			if not NULL, the decompressed runlist
 * @ni:			base ntfs inode this attribute belongs to
 * @type:		attribute type
 * @name:		Unicode name of the attribute
 * @name_len:		@name number of Unicode characters in @name
 * @state:		NTFS attribute-specific flags describing this attribute
 * @allocated_size:	allocated size copied from the attribute record
 * @data_size:		data size copied from the attribute record
 * @initialized_size:	initialized size copied from the attribute record
 * @compressed_size:	compressed size copied from the attribute record
 * @compression_block_size:		compression block size
 * @compression_block_size_bits:	log2 of the compression block size
 * @compression_block_clusters:		number of clusters per compression block
 *
 * This structure is purely for caching an attribute's runlist. It should not be used
 * to manipulate a specific attribute extent or a resident attribute.
 * Use NAttrNonResident() as a safety check.
 *
 * To manipulate a resident attribute or a specific attribute extent, use ntfsdrv_lookup_attr()
 * to get the attribute (extent) record, edit it, and write it back to the MFT record.
 *
 * @rl @rl is the decompressed runlist. It is only meaningful when the attribute is non-resident (NAttrNonResident() is TRUE).
 * @rl is NULL when the runlist is not decompressed.
 */
struct _ntfs_attr {
	runlist_element *rl;
	ntfsdrv_inode *ni;
	ATTR_TYPES type;
	ATTR_FLAGS data_flags;
	ntfschar *name;
	u32 name_len;
	unsigned long state;
	s64 allocated_size;
	s64 data_size;
	s64 initialized_size;
	s64 compressed_size;
	u32 compression_block_size;
	u8 compression_block_size_bits;
	u8 compression_block_clusters;
	s8 unused_runs; /* Reserved free empty entries */
};

/**
 * enum ntfsdrv_attr_state_bits - ntfsdrv_attr bit definitions of the state field in struct ntfsdrv_attr
 */
typedef enum {
	NA_Initialized,		/* 1: 1: structure is initialized. */
	NA_NonResident,		/* 1: 1: attribute is non-resident. */
	NA_BeingNonResident,	/* 1: 1: attribute is becoming non-resident. */
	NA_FullyMapped,		/* 1: 1: attribute is fully mapped. */
	NA_DataAppending,	/* 1: 1: attribute is appending data. */
	NA_ComprClosing,	/* 1: 1: compressed attribute is closing. */
	NA_RunlistDirty,	/* 1: Runlist 1: runlist has been updated. */
} ntfsdrv_attr_state_bits;

#define  test_nattr_flag(na, flag)	 test_bit(NA_##flag, (na)->state)
#define   set_nattr_flag(na, flag)	  set_bit(NA_##flag, (na)->state)
#define clear_nattr_flag(na, flag)	clear_bit(NA_##flag, (na)->state)

#define NAttrInitialized(na)		 test_nattr_flag(na, Initialized)
#define NAttrSetInitialized(na)		  set_nattr_flag(na, Initialized)
#define NAttrClearInitialized(na)	clear_nattr_flag(na, Initialized)

#define NAttrNonResident(na)		 test_nattr_flag(na, NonResident)
#define NAttrSetNonResident(na)		  set_nattr_flag(na, NonResident)
#define NAttrClearNonResident(na)	clear_nattr_flag(na, NonResident)

#define NAttrBeingNonResident(na)	test_nattr_flag(na, BeingNonResident)
#define NAttrSetBeingNonResident(na)	set_nattr_flag(na, BeingNonResident)
#define NAttrClearBeingNonResident(na)	clear_nattr_flag(na, BeingNonResident)

#define NAttrFullyMapped(na)		test_nattr_flag(na, FullyMapped)
#define NAttrSetFullyMapped(na)		set_nattr_flag(na, FullyMapped)
#define NAttrClearFullyMapped(na)	clear_nattr_flag(na, FullyMapped)

#define NAttrDataAppending(na)		test_nattr_flag(na, DataAppending)
#define NAttrSetDataAppending(na)	set_nattr_flag(na, DataAppending)
#define NAttrClearDataAppending(na)	clear_nattr_flag(na, DataAppending)

#define NAttrRunlistDirty(na)		test_nattr_flag(na, RunlistDirty)
#define NAttrSetRunlistDirty(na)	set_nattr_flag(na, RunlistDirty)
#define NAttrClearRunlistDirty(na)	clear_nattr_flag(na, RunlistDirty)

#define NAttrComprClosing(na)		test_nattr_flag(na, ComprClosing)
#define NAttrSetComprClosing(na)	set_nattr_flag(na, ComprClosing)
#define NAttrClearComprClosing(na)	clear_nattr_flag(na, ComprClosing)

#define GenNAttrIno(func_name, flag)			\
extern int NAttr##func_name(ntfsdrv_attr *na);		\
extern void NAttrSet##func_name(ntfsdrv_attr *na);		\
extern void NAttrClear##func_name(ntfsdrv_attr *na);

GenNAttrIno(Compressed, FILE_ATTR_COMPRESSED)
GenNAttrIno(Encrypted, 	FILE_ATTR_ENCRYPTED)
GenNAttrIno(Sparse, 	FILE_ATTR_SPARSE_FILE)
#undef GenNAttrIno

/**
 * union attr_val - union of all known attribute values
 *
 * for convenience in attribute structures.
 */
typedef union {
	u8 _default;	/* Unnamed u8 as the default field, taken when a_val is
			   used directly without specifying a field. */
	STANDARD_INFORMATION std_inf;
	ATTR_LIST_ENTRY al_entry;
	FILE_NAME_ATTR filename;
	OBJECT_ID_ATTR obj_id;
	SECURITY_DESCRIPTOR_ATTR sec_desc;
	VOLUME_NAME vol_name;
	VOLUME_INFORMATION vol_inf;
	DATA_ATTR data;
	INDEX_ROOT index_root;
	INDEX_BLOCK index_blk;
	BITMAP_ATTR bmp;
	REPARSE_POINT reparse;
	EA_INFORMATION ea_inf;
	EA_ATTR ea;
	PROPERTY_SET property_set;
	LOGGED_UTILITY_STREAM logged_util_stream;
	EFS_ATTR_HEADER efs;
} attr_val;

extern void ntfsdrv_attr_init(ntfsdrv_attr *na, const BOOL non_resident,
		const ATTR_FLAGS data_flags, const BOOL encrypted,
		const BOOL sparse,
		const s64 allocated_size, const s64 data_size,
		const s64 initialized_size, const s64 compressed_size,
		const u8 compression_unit);

	/* Note: the "name" below must be freeable */
	/* or be one of the constants AT_UNNAMED, NTFS_INDEX_I30, STREAM_SDS */
extern ntfsdrv_attr *ntfsdrv_attr_open(ntfsdrv_inode *ni, const ATTR_TYPES type,
		ntfschar *name, u32 name_len);
extern void ntfsdrv_attr_close(ntfsdrv_attr *na);

extern s64 ntfsdrv_attr_pread(ntfsdrv_attr *na, const s64 pos, s64 count,
		void *b);
extern s64 ntfsdrv_attr_pwrite(ntfsdrv_attr *na, const s64 pos, s64 count,
		const void *b);
extern int ntfsdrv_attr_pclose(ntfsdrv_attr *na);

extern void *ntfsdrv_attr_readall(ntfsdrv_inode *ni, const ATTR_TYPES type,
			       ntfschar *name, u32 name_len, s64 *data_size);

extern s64 ntfsdrv_attr_mst_pread(ntfsdrv_attr *na, const s64 pos,
		const s64 bk_cnt, const u32 bk_size, void *dst);
extern s64 ntfsdrv_attr_mst_pwrite(ntfsdrv_attr *na, const s64 pos,
		s64 bk_cnt, const u32 bk_size, void *src);

extern int ntfsdrv_attr_map_runlist(ntfsdrv_attr *na, VCN vcn);
extern int ntfsdrv_attr_map_whole_runlist(ntfsdrv_attr *na);

extern runlist_element *ntfsdrv_attr_find_vcn(ntfsdrv_attr *na, const VCN vcn);

extern int ntfsdrv_attr_size_bounds_check(const ntfsdrv_volume *vol,
		const ATTR_TYPES type, const s64 size);
extern int ntfsdrv_attr_can_be_resident(const ntfsdrv_volume *vol,
		const ATTR_TYPES type);
int ntfsdrv_attr_make_non_resident(ntfsdrv_attr *na,
		ntfsdrv_attr_search_ctx *ctx);
int ntfsdrv_attr_force_non_resident(ntfsdrv_attr *na);
extern int ntfsdrv_make_room_for_attr(MFT_RECORD *m, u8 *pos, u32 size);

extern int ntfsdrv_resident_attr_record_add(ntfsdrv_inode *ni, ATTR_TYPES type,
		const ntfschar *name, u8 name_len, const u8 *val, u32 size,
		ATTR_FLAGS flags);
extern int ntfsdrv_non_resident_attr_record_add(ntfsdrv_inode *ni, ATTR_TYPES type,
		const ntfschar *name, u8 name_len, VCN lowest_vcn,
		int dataruns_size, ATTR_FLAGS flags);
extern int ntfsdrv_attr_record_rm(ntfsdrv_attr_search_ctx *ctx);

extern int ntfsdrv_attr_add(ntfsdrv_inode *ni, ATTR_TYPES type,
		ntfschar *name, u8 name_len, const u8 *val, s64 size);
extern int ntfsdrv_attr_set_flags(ntfsdrv_inode *ni, ATTR_TYPES type,
		const ntfschar *name, u8 name_len, ATTR_FLAGS flags,
		ATTR_FLAGS mask);
extern int ntfsdrv_attr_rm(ntfsdrv_attr *na);

extern int ntfsdrv_attr_record_resize(MFT_RECORD *m, ATTR_RECORD *a, u32 new_size);

extern int ntfsdrv_resident_attr_value_resize(MFT_RECORD *m, ATTR_RECORD *a,
		const u32 new_size);

extern int ntfsdrv_attr_record_move_to(ntfsdrv_attr_search_ctx *ctx, ntfsdrv_inode *ni);
extern int ntfsdrv_attr_record_move_away(ntfsdrv_attr_search_ctx *ctx, int extra);

extern int ntfsdrv_attr_update_mapping_pairs(ntfsdrv_attr *na, VCN from_vcn);

extern int ntfsdrv_attr_truncate(ntfsdrv_attr *na, const s64 newsize);

/**
 * ntfsdrv_get_attribute_value_length - return the length of an attribute value
 * @a:	pointer to the attribute record buffer
 *
 * Returns the byte size of the value of attribute @a (after decompression and filling sparse holes, if needed).
 * On a return of 0 check errno: 0 means the length really is 0, otherwise it describes the error.
 */
extern s64 ntfsdrv_get_attribute_value_length(const ATTR_RECORD *a);

/**
 * ntfsdrv_get_attribute_value - return an attribute value
 * @vol:	volume the attribute is on
 * @a:		attribute whose value to get
 * @b:		destination buffer for the attribute value
 *
 * Copies the value of attribute @a into destination buffer @b. Note that the size of @b must be at least
 * ntfsdrv_get_attribute_value_length(@a) the return value of ntfsdrv_get_attribute_value_length(@a).
 *
 * Returns the number of bytes copied. On a return of 0 check errno: 0 means the attribute value
 * has zero length, otherwise it describes the error.
 */
extern s64 ntfsdrv_get_attribute_value(const ntfsdrv_volume *vol,
				    const ATTR_RECORD *a, u8 *b);

extern void  ntfsdrv_attr_name_free(char **name);
extern char *ntfsdrv_attr_name_get(const ntfschar *uname, const int uname_len);
extern int   ntfsdrv_attr_exist(ntfsdrv_inode *ni, const ATTR_TYPES type,
		const ntfschar *name, u32 name_len);
extern int   ntfsdrv_attr_remove(ntfsdrv_inode *ni, const ATTR_TYPES type,
			      ntfschar *name, u32 name_len);
extern s64   ntfsdrv_attr_get_free_bits(ntfsdrv_attr *na);
extern int ntfsdrv_attr_data_read(ntfsdrv_inode *ni,
		ntfschar *stream_name, int stream_name_len,
		char *buf, size_t size, off_t offset);
extern int ntfsdrv_attr_data_write(ntfsdrv_inode *ni,
		ntfschar *stream_name, int stream_name_len,
		const char *buf, size_t size, off_t offset);
extern int ntfsdrv_attr_shrink_size(ntfsdrv_inode *ni, ntfschar *stream_name,
		int stream_name_len, off_t offset);
extern int ntfsdrv_attr_inconsistent(const ATTR_RECORD *a, const MFT_REF mref);

#endif /* defined NTFSDRV_NTFS_ATTR_CORE_H */
