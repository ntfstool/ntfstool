/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_BLOCK_DEV_H
#define NTFSDRV_NTFS_BLOCK_DEV_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ntfs_block_dev_io.h"
#include "ntfs_core_types.h"
#include "ntfs_platform_support.h"
#include "ntfs_vol_context.h"

/**
 * enum ntfsdrv_device_state_bits -
 *
 * ntfsdrv_device Bit definitions of the state field in the ntfsdrv_device structure.
 */
typedef enum {
	ND_Open,	/* 1: device is open.*/
	ND_ReadOnly,	/* 1: device is read-only.*/
	ND_Dirty,	/* 1: device has been modified and needs syncing.*/
	ND_Block,	/* 1: device is a block device.*/
	ND_Sync,	/* 1: device is mounted with "-o sync" */
} ntfsdrv_device_state_bits;

#define  test_ndev_flag(nd, flag)	   test_bit(ND_##flag, (nd)->d_state)
#define   set_ndev_flag(nd, flag)	    set_bit(ND_##flag, (nd)->d_state)
#define clear_ndev_flag(nd, flag)	  clear_bit(ND_##flag, (nd)->d_state)

#define NDevOpen(nd)		 test_ndev_flag(nd, Open)
#define NDevSetOpen(nd)		  set_ndev_flag(nd, Open)
#define NDevClearOpen(nd)	clear_ndev_flag(nd, Open)

#define NDevReadOnly(nd)	 test_ndev_flag(nd, ReadOnly)
#define NDevSetReadOnly(nd)	  set_ndev_flag(nd, ReadOnly)
#define NDevClearReadOnly(nd)	clear_ndev_flag(nd, ReadOnly)

#define NDevDirty(nd)		 test_ndev_flag(nd, Dirty)
#define NDevSetDirty(nd)	  set_ndev_flag(nd, Dirty)
#define NDevClearDirty(nd)	clear_ndev_flag(nd, Dirty)

#define NDevBlock(nd)		 test_ndev_flag(nd, Block)
#define NDevSetBlock(nd)	  set_ndev_flag(nd, Block)
#define NDevClearBlock(nd)	clear_ndev_flag(nd, Block)

#define NDevSync(nd)		 test_ndev_flag(nd, Sync)
#define NDevSetSync(nd)		  set_ndev_flag(nd, Sync)
#define NDevClearSync(nd)	clear_ndev_flag(nd, Sync)

/**
 * struct ntfsdrv_device -
 *
 * NTFS device structure defining all operations needed to access the underlying device of an NTFS volume.
 *
 * Note that d_heads and d_sectors_per_track are only set after calling
 * ntfsdrv_device_heads_get() or ntfsdrv_device_sectors_per_track_get()
 * (both calls set both fields,
 * and they stay -1 if the query fails).
 */
struct ntfsdrv_device {
	struct ntfsdrv_device_operations *d_ops;	/* Device operations.*/
	unsigned long d_state;			/* Device state.*/
	char *d_name;				/* Device name.*/
	void *d_private;			/* Private data used by
						   the device operations.*/
	int d_heads;				/* Disk geometry:
						   number of heads or -1.*/
	int d_sectors_per_track;		/* Disk geometry:
						   sectors per track or -1.*/
};

struct stat;

/**
 * struct ntfsdrv_device_operations -
 *
 * NTFS device operations definition, containing all operations that can be performed
 * on the underlying device described by the NTFS device structure.
 */
struct ntfsdrv_device_operations {
	int (*open)(struct ntfsdrv_device *dev, int flags);
	int (*close)(struct ntfsdrv_device *dev);
	s64 (*seek)(struct ntfsdrv_device *dev, s64 offset, int whence);
	s64 (*read)(struct ntfsdrv_device *dev, void *buf, s64 count);
	s64 (*write)(struct ntfsdrv_device *dev, const void *buf, s64 count);
	s64 (*pread)(struct ntfsdrv_device *dev, void *buf, s64 count, s64 offset);
	s64 (*pwrite)(struct ntfsdrv_device *dev, const void *buf, s64 count,
			s64 offset);
	int (*sync)(struct ntfsdrv_device *dev);
	int (*stat)(struct ntfsdrv_device *dev, struct stat *buf);
	int (*ioctl)(struct ntfsdrv_device *dev, unsigned long request,
			void *argp);
};

/* Device operations override used at mount time: if non-NULL, replaces the default io operations (set by the upper layer before mounting, e.g. the BitLocker device layer). */
extern struct ntfsdrv_device_operations *ntfsdrv_device_ops_override;

extern struct ntfsdrv_device *ntfsdrv_device_alloc(const char *name, const long state,
		struct ntfsdrv_device_operations *dops, void *priv_data);
extern int ntfsdrv_device_free(struct ntfsdrv_device *dev);
extern int ntfsdrv_device_sync(struct ntfsdrv_device *dev);

extern s64 ntfsdrv_pread(struct ntfsdrv_device *dev, const s64 pos, s64 count,
		void *b);
extern s64 ntfsdrv_pwrite(struct ntfsdrv_device *dev, const s64 pos, s64 count,
		const void *b);

extern s64 ntfsdrv_mst_pread(struct ntfsdrv_device *dev, const s64 pos, s64 count,
		const u32 bksize, void *b);
extern s64 ntfsdrv_mst_pwrite(struct ntfsdrv_device *dev, const s64 pos, s64 count,
		const u32 bksize, void *b);

extern s64 ntfsdrv_device_size_get(struct ntfsdrv_device *dev, int block_size);
extern s64 ntfsdrv_device_partition_start_sector_get(struct ntfsdrv_device *dev);
extern int ntfsdrv_device_heads_get(struct ntfsdrv_device *dev);
extern int ntfsdrv_device_sectors_per_track_get(struct ntfsdrv_device *dev);
extern int ntfsdrv_device_sector_size_get(struct ntfsdrv_device *dev);
extern int ntfsdrv_device_block_size_set(struct ntfsdrv_device *dev, int block_size);

#endif /* defined NTFSDRV_NTFS_BLOCK_DEV_H */
