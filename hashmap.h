#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>

typedef char * key_t;
typedef int value_t;

typedef struct entry_t
{
	key_t key;
	value_t value;
	struct entry_t *next;
} entry_t;

typedef struct bucket_stats_t
{
	unsigned int num_entries;
} bucket_stats_t;

typedef struct bucket_t
{
	entry_t *entries;
	bucket_stats_t stats;
} bucket_t;

typedef struct hashmap_t
{
	bucket_t *buckets;
} hashmap_t;

int hashmap_init( hashmap_t *hashmap );
void hashmap_term( hashmap_t *hashmap );
hashmap_t *hashmap_create( void );
void hashmap_destroy( hashmap_t *hashmap );
int hashmap_add( hashmap_t *hashmap, key_t key, const value_t value );
unsigned int hashmap_find( hashmap_t *hashmap, const key_t key, value_t * const value );
void hashmap_stats_fprintf( FILE *fp, const hashmap_t *hashmap );

#endif
