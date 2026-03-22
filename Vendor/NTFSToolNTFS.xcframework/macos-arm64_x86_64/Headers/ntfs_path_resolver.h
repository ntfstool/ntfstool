/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
/*
 * ntfs_path_resolver.h - device mapper aware realpath()
 */

#ifndef NTFSDRV_NTFS_PATH_RESOLVER_H
#define NTFSDRV_NTFS_PATH_RESOLVER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_REALPATH
#define ntfsdrv_realpath realpath
#else
extern char *ntfsdrv_realpath(const char *path, char *resolved_path);
#endif

#ifdef linux
extern char *ntfsdrv_realpath_canonicalize(const char *path, char *resolved_path);
#else
#define ntfsdrv_realpath_canonicalize ntfsdrv_realpath
#endif

#endif /* NTFSDRV_NTFS_PATH_RESOLVER_H */
