/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_TIME_UTILS_H
#define NTFSDRV_NTFS_TIME_UTILS_H

#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif

#include "ntfs_core_types.h"

/*
 * If st_mtime is not defined, assume "struct timespec" is not defined
 */
#if !defined(st_mtime) & !defined(__timespec_defined)
struct timespec {
	time_t tv_sec;
	long tv_nsec;
} ;
#endif

/*
 * There are four times as many conversions from the internal representation to NTFS as other conversions,
 * so the most efficient internal representation is the NTFS one (little-endian byte order)
 */
typedef sle64 ntfsdrv_time;

#define NTFS_TIME_OFFSET ((s64)(369 * 365 + 89) * 24 * 3600 * 10000000)

/**
 * ntfs2timespec - convert NTFS time to Unix time
 * @ntfsdrv_time:  NTFS time, in 100ns units since 1601
 *
 * NTFS stores time as the number of
 * 100ns intervals since January 1, 1601 00:00 UTC. This system has no Y2K problem until about the year 57000 AD.
 *
 * Returns: Unix time (seconds and nanoseconds since 1970)
 */
static __inline__ struct timespec ntfs2timespec(ntfsdrv_time ntfstime)
{
	struct timespec spec;
	s64 cputime;

	cputime = sle64_to_cpu(ntfstime);
	spec.tv_sec = (cputime - (NTFS_TIME_OFFSET)) / 10000000;
	spec.tv_nsec = (cputime - (NTFS_TIME_OFFSET)
			- (s64)spec.tv_sec*10000000)*100;
		/* Force nanoseconds to zero on date overflow */
	if ((spec.tv_nsec < 0) || (spec.tv_nsec > 999999999))
		spec.tv_nsec = 0;
	return (spec);
}

/**
 * timespec2ntfs - convert Unix time to NTFS time
 * @utc_time:  Unix time to convert to NTFS time
 *
 * Convert Unix time @utc_time to the corresponding NTFS time.
 *
 * Unix currently stores time in a long, counted in 1-second intervals since January 1, 1970
 * 00:00:00 UTC (the epoch),
 * nanoseconds are stored separately and are non-negative
 *
 * NTFS uses the Microsoft standard time format, stored in an sle64,
 * measured in intervals of
 * 100 nanoseconds since January 1, 1601 00:00:00 UTC.
 *
 * Returns: NTFS time (100ns units since January 1601)
 */
static __inline__ ntfsdrv_time timespec2ntfs(struct timespec spec)
{
	s64 units;

	units = (s64)spec.tv_sec * 10000000
				+ NTFS_TIME_OFFSET + spec.tv_nsec/100;
	return (cpu_to_sle64(units));
}

/*
 *		returns the current time in NTFS format
 */

static __inline__ ntfsdrv_time ntfsdrv_current_time(void)
{
	struct timespec now;

#if defined(HAVE_CLOCK_GETTIME) || defined(HAVE_SYS_CLOCK_GETTIME)
	clock_gettime(CLOCK_REALTIME, &now);
#elif defined(HAVE_GETTIMEOFDAY)
	struct timeval microseconds;

	gettimeofday(&microseconds, (struct timezone*)NULL);
	now.tv_sec = microseconds.tv_sec;
	now.tv_nsec = microseconds.tv_usec*1000;
#else
	now.tv_sec = time((time_t*)NULL);
	now.tv_nsec = 0;
#endif
	return (timespec2ntfs(now));
}

#endif /* NTFSDRV_NTFS_TIME_UTILS_H */
