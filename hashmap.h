#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>

#define HASHMAP_DEFAULT_BUCKETS (8263) /* Prime */

typedef char * hashmap_key_t;
typedef int hashmap_value_t;

typedef struct hashmap_t
{
	size_t size;
	size_t num_buckets;
	struct bucket_t *buckets;
} hashmap_t;

int hashmap_init( hashmap_t *hashmap );
int hashmap_init_with_buckets( hashmap_t *hashmap, size_t num_buckets );
void hashmap_term( hashmap_t *hashmap );

hashmap_t *hashmap_create( void );
hashmap_t *hashmap_create_with_buckets( size_t num_buckets );
void hashmap_destroy( hashmap_t *hashmap );

int hashmap_insert( hashmap_t *hashmap, const hashmap_key_t key, const hashmap_value_t value );
int hashmap_insert_existed( hashmap_t *hashmap, const hashmap_key_t key, const hashmap_value_t value, unsigned int * const existed );
void hashmap_remove( hashmap_t *hashmap, const hashmap_key_t key );
void hashmap_remove_existed( hashmap_t *hashmap, const hashmap_key_t key, unsigned int * const existed );
size_t hashmap_size( const hashmap_t *hashmap );
unsigned int hashmap_find( hashmap_t *hashmap, const hashmap_key_t key, hashmap_value_t * const value );
void hashmap_stats_fprintf( FILE *fp, const hashmap_t *hashmap );

#endif
