/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_SEC_DESCRIPTOR_H
#define NTFSDRV_NTFS_SEC_DESCRIPTOR_H

#include "ntfs_core_types.h"
#include "ntfs_fs_layout.h"
#include "ntfs_vnode_core.h"
#include "ntfs_dir_catalog.h"
#include "ntfs_endian_utils.h"

#ifndef POSIXACLS
#define POSIXACLS 0
#endif

/*
 *          Entry in the mapping list
 */

struct MAPPING {
	struct MAPPING *next;
	int xid;		/* Linux ID: uid or gid */
	SID *sid;		/* Windows ID: user SID or group SID */
	int grcnt;		/* group count (users only) */
	gid_t *groups;		/* list of groups the user belongs to */
};

/*
 *		Entry in the permissions cache
 *	Note: this cache is not organized like a generic cache
 */

struct CACHED_PERMISSIONS {
	uid_t uid;
	gid_t gid;
	le32 inh_fileid;
	le32 inh_dirid;
#if POSIXACLS
	struct POSIX_SECURITY *pxdesc;
	unsigned int pxdescsize:16;
#endif
	unsigned int mode:12;
	unsigned int valid:1;
} ;

/*
 *	Entry in the permissions cache for a directory without security_id
 */

struct CACHED_PERMISSIONS_LEGACY {
	struct CACHED_PERMISSIONS_LEGACY *next;
	struct CACHED_PERMISSIONS_LEGACY *previous;
	void *variable;
	size_t varsize;
	union ALIGNMENT payload[0];
		/* the fields above must match "struct CACHED_GENERIC" */
	u64 mft_no;
	struct CACHED_PERMISSIONS perm;
} ;

/*
 *	Entry in the security identifier cache
 */

struct CACHED_SECURID {
	struct CACHED_SECURID *next;
	struct CACHED_SECURID *previous;
	void *variable;
	size_t varsize;
	union ALIGNMENT payload[0];
		/* the fields above must match "struct CACHED_GENERIC" */
	uid_t uid;
	gid_t gid;
	unsigned int dmode;
	le32 securid;
} ;

/*
 *	Header of the security cache
 *	(it has no cache structure of its own)
 */

struct CACHED_PERMISSIONS_HEADER {
	unsigned int last;
			/* permission statistics */
	unsigned long p_writes;
	unsigned long p_reads;
	unsigned long p_hits;
} ;

/*
 *	The complete permissions cache
 */

struct PERMISSIONS_CACHE {
	struct CACHED_PERMISSIONS_HEADER head;
	struct CACHED_PERMISSIONS *cachetable[1]; /* variable-size array */
} ;

/*
 *	Security flag values
 */

enum {
	SECURITY_DEFAULT,	/* rely on fuse for permission checks */
	SECURITY_RAW,		/* force the same owner/permissions on all files */
	SECURITY_ACL,		/* enable Posix ACLs (when enabled at compile time) */
	SECURITY_ADDSECURIDS,	/* upgrade old security descriptors */
	SECURITY_STATICGRPS,	/* use static groups for access control */
	SECURITY_WANTED		/* security-related options are present */
} ;

/*
 *	Security context, needed by most security functions
 */

enum { MAPUSERS, MAPGROUPS, MAPCOUNT } ;

struct SECURITY_CONTEXT {
	ntfsdrv_volume *vol;
	struct MAPPING *mapping[MAPCOUNT];
	struct PERMISSIONS_CACHE **pseccache;
	uid_t uid; /* uid of the requesting user (not the mounter) */
	gid_t gid; /* gid of the requesting user (not the mounter) */
	pid_t tid; /* thread ID of the requesting thread */
	mode_t umask; /* umask of the requesting thread */
	} ;

#if POSIXACLS

/*
 *		       Posix ACL structure
 */

struct POSIX_ACE {
	u16 tag;
	u16 perms;
	s32 id;
} __attribute__((__packed__));

struct POSIX_ACL {
	u8 version;
	u8 flags;
	u16 filler;
	struct POSIX_ACE ace[0];
} __attribute__((__packed__));

struct POSIX_SECURITY {
	mode_t mode;
	int acccnt;
	int defcnt;
	int firstdef;
	u16 tagsset;
	u16 filler;
	struct POSIX_ACL acl;
} ;

/*
 *		Posix tag, CPU byte order 16 bits
 */

enum {
	POSIX_ACL_USER_OBJ =	1,
	POSIX_ACL_USER =	2,
	POSIX_ACL_GROUP_OBJ =	4,
	POSIX_ACL_GROUP =	8,
	POSIX_ACL_MASK =	16,
	POSIX_ACL_OTHER =	32,
	POSIX_ACL_SPECIAL =	64  /* internal use only */
} ;

#define POSIX_ACL_EXTENSIONS (POSIX_ACL_USER | POSIX_ACL_GROUP | POSIX_ACL_MASK)

/*
 *		Posix permissions, CPU byte order 16 bits
 */

enum {
	POSIX_PERM_X =		1,
	POSIX_PERM_W =		2,
	POSIX_PERM_R =		4,
	POSIX_PERM_DENIAL =	64 /* internal use only */
} ;

#define POSIX_VERSION 2

#endif

extern void ntfsdrv_generate_guid(GUID *guid);
extern int ntfsdrv_sd_add_everyone(ntfsdrv_inode *ni);

extern le32 ntfsdrv_security_hash(const SECURITY_DESCRIPTOR_RELATIVE *sd,
			       const u32 len);

int ntfsdrv_build_mapping(struct SECURITY_CONTEXT *scx, const char *usermap_path,
		BOOL allowdef);
int ntfsdrv_get_owner_mode(struct SECURITY_CONTEXT *scx,
		ntfsdrv_inode *ni, struct stat*);
int ntfsdrv_set_mode(struct SECURITY_CONTEXT *scx, ntfsdrv_inode *ni, mode_t mode);
BOOL ntfsdrv_allowed_as_owner(struct SECURITY_CONTEXT *scx, ntfsdrv_inode *ni);
int ntfsdrv_allowed_access(struct SECURITY_CONTEXT *scx,
		ntfsdrv_inode *ni, int accesstype);
int ntfsdrv_allowed_create(struct SECURITY_CONTEXT *scx,
		ntfsdrv_inode *ni, gid_t *pgid, mode_t *pdsetgid);
BOOL old_ntfs_allowed_dir_access(struct SECURITY_CONTEXT *scx,
		const char *path, int accesstype);

#if POSIXACLS
le32 ntfsdrv_alloc_securid(struct SECURITY_CONTEXT *scx,
		uid_t uid, gid_t gid, ntfsdrv_inode *dir_ni,
		mode_t mode, BOOL isdir);
#else
le32 ntfsdrv_alloc_securid(struct SECURITY_CONTEXT *scx,
		uid_t uid, gid_t gid, mode_t mode, BOOL isdir);
#endif
int ntfsdrv_set_owner(struct SECURITY_CONTEXT *scx, ntfsdrv_inode *ni,
		uid_t uid, gid_t gid);
#if POSIXACLS
int ntfsdrv_set_owner_mode(struct SECURITY_CONTEXT *scx,
		ntfsdrv_inode *ni, uid_t uid, gid_t gid,
		mode_t mode, struct POSIX_SECURITY *pxdesc);
#else
int ntfsdrv_set_owner_mode(struct SECURITY_CONTEXT *scx,
		ntfsdrv_inode *ni, uid_t uid, gid_t gid, mode_t mode);
#endif
le32 ntfsdrv_inherited_id(struct SECURITY_CONTEXT *scx,
		ntfsdrv_inode *dir_ni, BOOL fordir);
int ntfsdrv_open_secure(ntfsdrv_volume *vol);
int ntfsdrv_close_secure(ntfsdrv_volume *vol);

void ntfsdrv_destroy_security_context(struct SECURITY_CONTEXT *scx);

#if POSIXACLS

int ntfsdrv_set_inherited_posix(struct SECURITY_CONTEXT *scx,
		ntfsdrv_inode *ni, uid_t uid, gid_t gid,
		ntfsdrv_inode *dir_ni, mode_t mode);
int ntfsdrv_get_posix_acl(struct SECURITY_CONTEXT *scx, ntfsdrv_inode *ni,
			const char *name, char *value, size_t size);
int ntfsdrv_set_posix_acl(struct SECURITY_CONTEXT *scx, ntfsdrv_inode *ni,
			const char *name, const char *value, size_t size,
			int flags);
int ntfsdrv_remove_posix_acl(struct SECURITY_CONTEXT *scx, ntfsdrv_inode *ni,
			const char *name);
#endif

int ntfsdrv_get_ntfs_acl(struct SECURITY_CONTEXT *scx, ntfsdrv_inode *ni,
			char *value, size_t size);
int ntfsdrv_set_ntfs_acl(struct SECURITY_CONTEXT *scx, ntfsdrv_inode *ni,
			const char *value, size_t size, int flags);

int ntfsdrv_get_ntfs_attrib(ntfsdrv_inode *ni, char *value, size_t size);
int ntfsdrv_set_ntfs_attrib(ntfsdrv_inode *ni,
			const char *value, size_t size,	int flags);

#endif /* defined NTFSDRV_NTFS_SEC_DESCRIPTOR_H */
