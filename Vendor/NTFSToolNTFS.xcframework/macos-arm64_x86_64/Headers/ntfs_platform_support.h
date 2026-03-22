/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_PLATFORM_SUPPORT_H
#define NTFSDRV_NTFS_PLATFORM_SUPPORT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

/*
 * mailing list. Use this macro to avoid mistyping the mailbox address.
 */
#define NTFS_DEV_LIST	"service@ntfstool.com"

/*
 * Generic macro: convert a pointer to a value for comparison.
 */
#ifndef p2n
#define p2n(p)		((ptrdiff_t)((ptrdiff_t*)(p)))
#endif

/*
 * The classic min and max macros.
 */
#ifndef min
#define min(a,b)	((a) <= (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)	((a) >= (b) ? (a) : (b))
#endif

/*
 * Macro to get the offset of a struct member.
 */
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/*
 * Simple bit operation macros. Note: these are not atomic.
 */
#define test_bit(bit, var)	      ((var) & (1 << (bit)))
#define set_bit(bit, var)	      (var) |= 1 << (bit)
#define clear_bit(bit, var)	      (var) &= ~(1 << (bit))

#define test_and_set_bit(bit, var)			\
({							\
	const BOOL old_state = test_bit(bit, var);	\
	set_bit(bit, var);				\
	old_state;					\
})

#define test_and_clear_bit(bit, var)			\
({							\
	const BOOL old_state = test_bit(bit, var);	\
	clear_bit(bit, var);				\
	old_state;					\
})

#endif /* defined NTFSDRV_NTFS_PLATFORM_SUPPORT_H */
