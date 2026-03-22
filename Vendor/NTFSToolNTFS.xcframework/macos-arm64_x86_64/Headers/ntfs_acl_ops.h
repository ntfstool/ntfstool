/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */

#ifndef NTFSDRV_NTFS_ACL_OPS_H
#define NTFSDRV_NTFS_ACL_OPS_H

#include "ntfs_endian_utils.h"

/*
 *	Configuration mode for security and ACL operations
 *	should be moved to a configuration file
 */

#define BUFSZ 1024		/* buffer size for reading the mapping file */
#define MAPPINGFILE ".NTFSTOOL-4G/UserMapping" /* default mapping file */
#define LINESZ 120              /* maximum valid length of a mapping line */
#define CACHE_PERMISSIONS_BITS 6  /* log2 of the permission unit allocation */
#define CACHE_PERMISSIONS_SIZE 262144 /* maximum number of cacheable permissions */

/*
 *		NTFS Mapping of NTFS permissions to Linux permissions
 *	These constants are adapted for byte order
 *	all are set when setting
 *	any one present counts when checking
 */

          /* flags meaning execute, write or read */

#define FILE_READ (FILE_READ_DATA)
#define FILE_WRITE (FILE_WRITE_DATA | FILE_APPEND_DATA \
		| READ_CONTROL | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA)
#define FILE_EXEC (FILE_EXECUTE)
#define DIR_READ FILE_LIST_DIRECTORY
#define DIR_WRITE (FILE_ADD_FILE | FILE_ADD_SUBDIRECTORY | FILE_DELETE_CHILD \
	 	| READ_CONTROL | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA)
#define DIR_EXEC (FILE_TRAVERSE)

          /* flags to test for execute, write or read */
	  /* write test allows interpreting the sticky bit */

#define FILE_GREAD (FILE_READ_DATA | GENERIC_READ)
#define FILE_GWRITE (FILE_WRITE_DATA | FILE_APPEND_DATA | GENERIC_WRITE)
#define FILE_GEXEC (FILE_EXECUTE | GENERIC_EXECUTE)
#define DIR_GREAD (FILE_LIST_DIRECTORY | GENERIC_READ)
#define DIR_GWRITE (FILE_ADD_FILE | FILE_ADD_SUBDIRECTORY | GENERIC_WRITE)
#define DIR_GEXEC (FILE_TRAVERSE | GENERIC_EXECUTE)

	/* standard owner (and administrator) permissions */

#define OWNER_RIGHTS (DELETE | READ_CONTROL | WRITE_DAC | WRITE_OWNER \
			| SYNCHRONIZE \
			| FILE_READ_ATTRIBUTES | FILE_WRITE_ATTRIBUTES \
			| FILE_READ_EA | FILE_WRITE_EA)

	/* standard global permissions */

#define WORLD_RIGHTS (READ_CONTROL | FILE_READ_ATTRIBUTES | FILE_READ_EA \
			| SYNCHRONIZE)

          /* inheritance flags for files and directories */

#define FILE_INHERITANCE NO_PROPAGATE_INHERIT_ACE
#define DIR_INHERITANCE (OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE)

/*
 *		Marks identifying NTFS ACLs that represent Posix ACLs granted to root NTFS ACL
 *	They use permissions that are always granted to anybody, so they have
 *	Windows no effect on either Windows or Linux.
 */

#define ROOT_OWNER_UNMARK SYNCHRONIZE	/* ACL granted to root as owner ACL */
#define ROOT_GROUP_UNMARK FILE_READ_EA	/* ACL granted to root as group ACL */

/*
 *		Maximum SID size and a type large enough to hold it
 */

#define MAX_SID_SIZE (8 + SID_MAX_SUB_AUTHORITIES*4)
typedef char BIGSID[MAX_SID_SIZE];

/*
 *		Structure holding the input mapping file
 *	(private to this module)
 */

struct MAPLIST {
	struct MAPLIST *next;
	char *uidstr;		/* uid text from the same record */
	char *gidstr;		/* gid text from the same record */
	char *sidstr;		/* SID text from the same record */
	char maptext[LINESZ + 1];
};

typedef int (*FILEREADER)(void *fileid, char *buf, size_t size, off_t pos);

/*
 *		acls.c Constants defined in acls.c
 */

extern const SID *adminsid;
extern const SID *worldsid;

/*
 *		acls.c Functions defined in acls.c
 */

BOOL ntfsdrv_valid_descr(const char *securattr, unsigned int attrsz);
BOOL ntfsdrv_valid_pattern(const SID *sid);
BOOL ntfsdrv_valid_sid(const SID *sid);
BOOL ntfsdrv_same_sid(const SID *first, const SID *second);

BOOL ntfsdrv_is_user_sid(const SID *usid);

int ntfsdrv_sid_size(const SID * sid);
unsigned int ntfsdrv_attr_size(const char *attr);

const SID *ntfsdrv_find_usid(const struct MAPPING *usermapping,
			uid_t uid, SID *pdefsid);
const SID *ntfsdrv_find_gsid(const struct MAPPING *groupmapping,
			gid_t gid, SID *pdefsid);
uid_t ntfsdrv_find_user(const struct MAPPING *usermapping, const SID *usid);
gid_t ntfsdrv_find_group(const struct MAPPING *groupmapping, const SID * gsid);
const SID *ntfsdrv_acl_owner(const char *secattr);

#if POSIXACLS

BOOL ntfsdrv_valid_posix(const struct POSIX_SECURITY *pxdesc);
void ntfsdrv_sort_posix(struct POSIX_SECURITY *pxdesc);
int ntfsdrv_merge_mode_posix(struct POSIX_SECURITY *pxdesc, mode_t mode);
struct POSIX_SECURITY *ntfsdrv_build_inherited_posix(
		const struct POSIX_SECURITY *pxdesc, mode_t mode,
		mode_t umask, BOOL isdir);
struct POSIX_SECURITY *ntfsdrv_build_basic_posix(
		const struct POSIX_SECURITY *pxdesc, mode_t mode,
		mode_t umask, BOOL isdir);
struct POSIX_SECURITY *ntfsdrv_replace_acl(const struct POSIX_SECURITY *oldpxdesc,
		const struct POSIX_ACL *newacl, int count, BOOL deflt);
struct POSIX_SECURITY *ntfsdrv_build_permissions_posix(
			struct MAPPING* const mapping[],
			const char *securattr,
			const SID *usid, const SID *gsid, BOOL isdir);
struct POSIX_SECURITY *ntfsdrv_merge_descr_posix(const struct POSIX_SECURITY *first,
			const struct POSIX_SECURITY *second);
char *ntfsdrv_build_descr_posix(struct MAPPING* const mapping[],
			struct POSIX_SECURITY *pxdesc,
			int isdir, const SID *usid, const SID *gsid);

#endif /* POSIXACLS */

int ntfsdrv_inherit_acl(const ACL *oldacl, ACL *newacl,
			const SID *usid, const SID *gsid,
			BOOL fordir, le16 inherited);
int ntfsdrv_build_permissions(const char *securattr,
			const SID *usid, const SID *gsid, BOOL isdir);
char *ntfsdrv_build_descr(mode_t mode,
			int isdir, const SID * usid, const SID * gsid);
struct MAPLIST *ntfsdrv_read_mapping(FILEREADER reader, void *fileid);
struct MAPPING *ntfsdrv_do_user_mapping(struct MAPLIST *firstitem);
struct MAPPING *ntfsdrv_do_group_mapping(struct MAPLIST *firstitem);
void ntfsdrv_free_mapping(struct MAPPING *mapping[]);

#endif /* NTFSDRV_NTFS_ACL_OPS_H */
