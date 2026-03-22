/**
 * ntfstool-driver - NTFS Engine for macOS
 *
 * Copyright (c) 2019-2026 NTFSTool. Released under the MIT License.
 * Contact: service@ntfstool.com
 */
#ifndef NTFSDRV_NTFS_CACHE_ENGINE_H
#define NTFSDRV_NTFS_CACHE_ENGINE_H

#include "ntfs_vol_context.h"

struct CACHED_GENERIC {
	struct CACHED_GENERIC *next;
	struct CACHED_GENERIC *previous;
	void *variable;
	size_t varsize;
	union ALIGNMENT payload[0];
} ;

struct CACHED_INODE {
	struct CACHED_INODE *next;
	struct CACHED_INODE *previous;
	const char *pathname;
	size_t varsize;
	union ALIGNMENT payload[0];
		/* The above fields must match "struct CACHED_GENERIC" */
	u64 inum;
} ;

struct CACHED_NIDATA {
	struct CACHED_NIDATA *next;
	struct CACHED_NIDATA *previous;
	const char *pathname;	/* unused */
	size_t varsize;		/* unused */
	union ALIGNMENT payload[0];
		/* The above fields must match "struct CACHED_GENERIC" */
	u64 inum;
	ntfsdrv_inode *ni;
} ;

struct CACHED_LOOKUP {
	struct CACHED_LOOKUP *next;
	struct CACHED_LOOKUP *previous;
	const char *name;
	size_t namesize;
	union ALIGNMENT payload[0];
		/* The above fields must match "struct CACHED_GENERIC" */
	u64 parent;
	u64 inum;
} ;

enum {
	CACHE_FREE = 1,
	CACHE_NOHASH = 2
} ;

typedef int (*cache_compare)(const struct CACHED_GENERIC *cached,
				const struct CACHED_GENERIC *item);
typedef void (*cache_free)(const struct CACHED_GENERIC *cached);
typedef int (*cache_hash)(const struct CACHED_GENERIC *cached);

struct HASH_ENTRY {
	struct HASH_ENTRY *next;
	struct CACHED_GENERIC *entry;
} ;

struct CACHE_HEADER {
	const char *name;
	struct CACHED_GENERIC *most_recent_entry;
	struct CACHED_GENERIC *oldest_entry;
	struct CACHED_GENERIC *free_entry;
	struct HASH_ENTRY *free_hash;
	struct HASH_ENTRY **first_hash;
	cache_free dofree;
	cache_hash dohash;
	unsigned long reads;
	unsigned long writes;
	unsigned long hits;
	int fixed_size;
	int max_hash;
	struct CACHED_GENERIC entry[0];
} ;

	/* Cast to the generic type to avoid gcc warnings */
#define GENERIC(pstr) ((const struct CACHED_GENERIC*)(const void*)(pstr))

struct CACHED_GENERIC *ntfsdrv_fetch_cache(struct CACHE_HEADER *cache,
			const struct CACHED_GENERIC *wanted,
			cache_compare compare);
struct CACHED_GENERIC *ntfsdrv_enter_cache(struct CACHE_HEADER *cache,
			const struct CACHED_GENERIC *item,
			cache_compare compare);
int ntfsdrv_invalidate_cache(struct CACHE_HEADER *cache,
			const struct CACHED_GENERIC *item,
			cache_compare compare, int flags);
int ntfsdrv_remove_cache(struct CACHE_HEADER *cache,
			struct CACHED_GENERIC *item, int flags);

void ntfsdrv_create_lru_caches(ntfsdrv_volume *vol);
void ntfsdrv_free_lru_caches(ntfsdrv_volume *vol);

#endif /* NTFSDRV_NTFS_CACHE_ENGINE_H */
