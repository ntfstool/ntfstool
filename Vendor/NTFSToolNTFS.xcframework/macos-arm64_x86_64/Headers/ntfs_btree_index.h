/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_BTREE_INDEX_H
#define NTFSDRV_NTFS_BTREE_INDEX_H

/* Convenience macro for testing the gcc version.
 * Usage:
 *  #if __GNUC_PREREQ (2,8)
 *  ... code requiring gcc 2.8 or later ...
 *  #endif
 *  Note - does not work with gcc1 or glibc1, since the _MINOR macros were not defined then.
 */

#ifndef __GNUC_PREREQ
# if defined __GNUC__ && defined __GNUC_MINOR__
#  define __GNUC_PREREQ(maj, min) \
        ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
# else
#  define __GNUC_PREREQ(maj, min) 0
# endif
#endif

/* Allow warnings for unused return values of specific function calls */
#ifndef __attribute_warn_unused_result__
# if __GNUC_PREREQ (3,4)
#  define __attribute_warn_unused_result__ \
    __attribute__ ((__warn_unused_result__))
# else
#  define __attribute_warn_unused_result__ /* empty */
# endif
#endif

#include "ntfs_attr_core.h"
#include "ntfs_core_types.h"
#include "ntfs_fs_layout.h"
#include "ntfs_vnode_core.h"
#include "ntfs_mft_table.h"

#define  VCN_INDEX_ROOT_PARENT  ((VCN)-2)

#define  MAX_PARENT_VCN		32

typedef int (*COLLATE)(ntfsdrv_volume *vol, const void *data1, int len1,
					 const void *data2, int len2);

/**
 * struct ntfsdrv_index_context -
 * @ni:			inode containing the @entry described by this context
 * @name:		name of the index described by this context
 * @name_len:		length of the index name
 * @entry:		index entryindex entry (points into @ir or @ia)
 * @data:		index entry index entry data (points into @entry)
 * @data_len:		@data byte length of @data
 * @is_in_root:		TRUE TRUE if @entry is in @ir, FALSE if in @ia
 * @ir:			index root (valid if @is_in_root), otherwise NULL
 * @actx:		attribute search context (valid when in root), otherwise NULL
 * @ia:			@is_in_root index block when @is_in_root is FALSE, otherwise NULL
 * @ia_na:		opened INDEX_ALLOCATION attribute
 * @parent_pos:		position of the parent entry in the index block
 * @parent_vcn:		parent node of the entry, or VCN_INDEX_ROOT_PARENT
 * @new_vcn:            new VCN when a new index block must be created
 * @median:		median value moved to the parent when splitting an index block
 * @ib_dirty:		TRUE TRUE if the index block has been modified
 * @block_size:		index block index block size
 * @vcn_size_bits:	VCN size in bits of this index block
 *
 * @ni @ni is the inode this context belongs to.
 *
 * @entry @entry is the index entry described by this context. @data and @data_len
 * are the index entry data and its byte length. @data
 * points directly into @entry, which is probably what the user is interested in.
 *
 * If @is_in_root is TRUE, @entry is in the index root attribute @ir described by the attribute search context @actx
 * and inode @ni.
 * @ia and @ib_dirty are undefined in this case.
 *
 * If @is_in_root is FALSE, @entry is in an index allocation attribute,
 * @ia and @ia points to the index allocation block and VCN containing it.
 * @ir and @actx are NULL in this case. @ia_na is the opened
 * INDEX_ALLOCATION INDEX_ALLOCATION attribute. @ib_dirty is TRUE if the index block has been modified,
 * otherwise FALSE.
 *
 * Call ntfsdrv_index_ctx_get() to obtain a context.
 *
 * When done with @entry and its @data, call ntfsdrv_index_ctx_put()
 * to release the context and other associated resources.
 *
 * If the index entry was modified, call ntfsdrv_index_entry_mark_dirty() before
 * ntfsdrv_index_ctx_put() to ensure the changes are written to disk.
 */
typedef struct {
	ntfsdrv_inode *ni;
	ntfschar *name;
	u32 name_len;
	INDEX_ENTRY *entry;
	void *data;
	u16 data_len;
	COLLATE collate;
	BOOL is_in_root;
	INDEX_ROOT *ir;
	ntfsdrv_attr_search_ctx *actx;
	INDEX_BLOCK *ib;
	ntfsdrv_attr *ia_na;
	int parent_pos[MAX_PARENT_VCN];  /* position of the parent entry */
	VCN parent_vcn[MAX_PARENT_VCN]; /* parent node of the entry */
	int pindex;	     /* at most the number of parent nodes  */
	BOOL ib_dirty;
	BOOL bad_index;
	u32 block_size;
	u8 vcn_size_bits;
} ntfsdrv_index_context;

extern ntfsdrv_index_context *ntfsdrv_index_ctx_get(ntfsdrv_inode *ni,
						ntfschar *name, u32 name_len);
extern void ntfsdrv_index_ctx_put(ntfsdrv_index_context *ictx);
extern void ntfsdrv_index_ctx_reinit(ntfsdrv_index_context *ictx);

extern int ntfsdrv_index_block_inconsistent(const INDEX_BLOCK *ib, u32 block_size,
			u64 inum, VCN vcn);
extern int ntfsdrv_index_entry_inconsistent(const INDEX_ENTRY *ie,
			COLLATION_RULES collation_rule, u64 inum);
extern int ntfsdrv_index_lookup(const void *key, const int key_len,
		ntfsdrv_index_context *ictx) __attribute_warn_unused_result__;

extern INDEX_ENTRY *ntfsdrv_index_next(INDEX_ENTRY *ie,
		ntfsdrv_index_context *ictx);

extern int ntfsdrv_index_add_filename(ntfsdrv_inode *ni, FILE_NAME_ATTR *fn,
		MFT_REF mref);
extern int ntfsdrv_index_remove(ntfsdrv_inode *dir_ni, ntfsdrv_inode *ni,
		const void *key, const int keylen);

extern VCN ntfsdrv_ie_get_vcn(INDEX_ENTRY *ie);

extern void ntfsdrv_index_entry_mark_dirty(ntfsdrv_index_context *ictx);

extern char *ntfsdrv_ie_filename_get(INDEX_ENTRY *ie);

/* For security descriptor operations */
extern int ntfsdrv_ie_add(ntfsdrv_index_context *icx, INDEX_ENTRY *ie);
extern int ntfsdrv_index_rm(ntfsdrv_index_context *icx);

#endif /* NTFSDRV_NTFS_BTREE_INDEX_H */
