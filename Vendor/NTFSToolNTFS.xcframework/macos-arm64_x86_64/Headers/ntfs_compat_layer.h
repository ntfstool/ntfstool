/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_COMPAT_LAYER_H
#define NTFSDRV_NTFS_COMPAT_LAYER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include <errno.h>	/* ENODATA */

#ifndef ENODATA
#define ENODATA ENOENT
#endif

#ifndef ELIBBAD
#define ELIBBAD ENOEXEC
#endif

#ifndef ELIBACC
#define ELIBACC ENOENT
#endif

/* macOS uses an xattr API that differs from Linux; when the attribute is not found it
 * expects the special error code ENOATTR to be returned. So NTFS_NOXATTR_ERRNO is NTFS_NOXATTR_ERRNO
 * defined as the "xattr not found" errno value for the current platform.*/
#if defined(__APPLE__) || defined(__DARWIN__)
#define NTFS_NOXATTR_ERRNO ENOATTR
#else
#define NTFS_NOXATTR_ERRNO ENODATA
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef HAVE_FFS
extern int ffs(int i);
#endif /* HAVE_FFS */

#ifndef HAVE_DAEMON
extern int daemon(int nochdir, int noclose);
#endif /* HAVE_DAEMON */

#ifndef HAVE_STRSEP
extern char *strsep(char **stringp, const char *delim);
#endif /* HAVE_STRSEP */

#ifdef WINDOWS

#define HAVE_STDIO_H		/* mimic config.h */
#define HAVE_STDARG_H

#define atoll			_atoi64
#define fdatasync		commit
#define __inline__		inline
#define __attribute__(X)	/*nothing*/

#else /* !defined WINDOWS */

#ifndef O_BINARY
#define O_BINARY		0		/* unix is binary mode by default */
#endif

#endif /* defined WINDOWS */

#endif /* defined NTFSDRV_NTFS_COMPAT_LAYER_H */

