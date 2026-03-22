/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_DRIVER_PARAMS_H
#define NTFSDRV_NTFS_DRIVER_PARAMS_H

#define CACHE_INODE_SIZE 256	/* inode cache, 0 or >= 3 and not too large */
#define CACHE_NIDATA_SIZE 512	/* idata cache, 0 or >= 3 and not too large */
#define CACHE_LOOKUP_SIZE 512	/* lookup cache, 0 or >= 3 and not too large */
#define CACHE_SECURID_SIZE 128    /* securid cache, 0 or >= 3 and not too large */
#define CACHE_LEGACY_SIZE 32    /* legacy cache size, 0 or >= 3 and not too large */

#define FORCE_FORMAT_v1x 0	/* Insert security data the NTFS v1.x way */
#define OWNERFROMACL 1		/* Get owner from ACL (rather than the Windows owner) */

		/* Default security sub-permissions */
enum {
	DEFSECAUTH1 = -1153374643, /* 3141592653 */
	DEFSECAUTH2 = 589793238,
	DEFSECAUTH3 = 462843383,
	DEFSECBASE = 10000
};

/*
 *		Formatting parameters
 */

		/* Windows 10 and earlier, cluster size is limited to 64K 64K */
#define NTFS_MAX_CLUSTER_SIZE 2097152 /* Windows 10 Creators allows 2MB 2MB */

/*
 *		Compression parameters
 */

	/* Default compression options */
#define DEFAULT_COMPRESSION 1
	/* log2 of the number of clusters in a compression block of new files */
#define STANDARD_COMPRESSION_UNIT 4
	/* Maximum cluster size at which new files may be compressed */
#define MAX_COMPRESSION_CLUSTER_SIZE 4096

/*
 *		Default option parameters
 */

#define DEFAULT_DMTIME 60 /* delay_mtime default: 1 minute */

/*
 *		Use of large write buffers
 *
 *	For small volumes, the cluster allocator may fail to allocate enough clusters when the volume is nearly full.
 *	At most one run is allocated per bitmap block, so there is a risk when
 *	the number of blocks (capacity/(32768*clsiz)) is less than the number of
 *	clusters in the maximum write buffer (131072/clsiz).
 *	So the safe minimum capacity is 4GB.
 */

#define SAFE_CAPACITY_FOR_BIG_WRITES 0x100000000LL

/*
 *		runlist parameters
 */

	/* Only update the last extent of the runlist when appending data extent */
#define PARTIAL_RUNLIST_UPDATING 1

/*
 *		Uppercase table parameters
 */

	/* Create the uppercase table per the Windows 6.1 (Win7) definition */
#define UPCASE_MAJOR 6
#define UPCASE_MINOR 1

/*
 *		User and xattr mapping parameters
 */

#define XATTRMAPPINGFILE ".NTFSTOOL-4G/XattrMapping" /* Default mapping file */

/*
 *		Path normalization parameters
 */

#define MAPPERNAMELTH 256

/*
 *		Permission check modes of the high and low levels
 *
 *	The high and low level choices are independent and have no effect on the library
 *
 *	Use the recommended values unless you understand the impact on protection and performance.
 *	Using the cache helps performance but hurts security for internal fuse or
 *	external fuse below version 2.8
 *
 *	On Linux, caching is not recommended for the high level interface,
 *	to support hard links correctly. Hence, doing access control in the file system
 *	reduces requests to the file system and context switches
 *
 *	Whatever the mode, read-only mounts always use the cache
 *
 *	High level values:
 *		1 : no cache, kernel control (recommended)
 *		4 : no cache, file system control
 *		6 : kernel/fuse cache, file system control (OpenIndiana only)
 *		7 : no cache, kernel-controlled ACL ACL
 *
 *	Low level values:
 *		2 : no cache, kernel control
 *		3 : kernel/fuse cache, kernel control (recommended)
 *		5 : no cache, file system control
 *		6 : kernel/fuse cache, file system control (OpenIndiana only)
 *		8 : no cache, kernel-controlled ACL ACL
 *		9 : kernel/fuse cache, kernel-controlled ACL (target value)
 *
 *	Using options 7, 8 and 9 requires upgrading the fuse module
 *	When Posix ACL is selected in the configure options,
 *	the value 6 is added in the mount report.
 */

#define TIMEOUT_RO 600 /* Attribute timeout for read-only mounts */
#if defined(__sun) && defined(__SVR4)
/*
 *	OpenIndiana does not implement kernel access control,
 *	but it handles caching of hard-linked files.
 */
#define HPERMSCONFIG 6
#define LPERMSCONFIG 6
#else /* defined(__sun) && defined(__SVR4) */
/*
 *	Kernel caching on Linux is flawed when access control is done by the file system,
 *	as it is when using hard-linked files on the fuse high level interface.
 *	ACL checks in recent kernels are also not ideal.
 */
#define HPERMSCONFIG 1
#define LPERMSCONFIG 3
#endif /* defined(__sun) && defined(__SVR4) */

#endif /* defined NTFSDRV_NTFS_DRIVER_PARAMS_H */
