/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_BLOCK_DEV_IO_H
#define NTFSDRV_NTFS_BLOCK_DEV_IO_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef NO_NTFS_DEVICE_DEFAULT_IO_OPS

/* Use standard Unix-style low-level device operations. */
#define ntfsdrv_device_default_io_ops ntfsdrv_device_unix_io_ops

/* Forward declarations. */
struct ntfsdrv_device_operations;

extern struct ntfsdrv_device_operations ntfsdrv_device_default_io_ops;

#endif /* NO_NTFS_DEVICE_DEFAULT_IO_OPS */

#endif /* defined NTFSDRV_NTFS_BLOCK_DEV_IO_H */

