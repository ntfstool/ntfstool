/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */

#ifndef NTFSDRV_NTFS_IOCTL_HANDLER_H
#define NTFSDRV_NTFS_IOCTL_HANDLER_H

/*
 * uses "unsigned long cmd" internally, similar to Linux <sys/ioctl.h>,
 * but note that fuse truncates arg to 32 bits, and some commands
 * (such as FITRIM) do not fit in a signed 32-bit field.
 */
int ntfsdrv_ioctl(ntfsdrv_inode *ni, unsigned long cmd, void *arg,
                        unsigned int flags, void *data);

#endif /* NTFSDRV_NTFS_IOCTL_HANDLER_H */
