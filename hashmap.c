#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "hashmap.h"

typedef struct entry_t
{
	hashmap_key_t key;
	hashmap_value_t value;
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

static entry_t *entry_create( hashmap_key_t key, hashmap_value_t value )
{
	entry_t *entry = malloc( sizeof(entry_t) );

	if( NULL != entry )
	{
		size_t key_len = strlen( key ) + 1;
		hashmap_key_t key_copy = malloc( key_len );

		if( NULL != key_copy )
		{
			memcpy( key_copy, key, key_len );

			entry->key = key_copy;
			entry->value = value;
			entry->next = NULL;
		}
		else
		{
			free( entry );
			entry = NULL;
		}
	}

	return entry;
}

static void entry_destroy( entry_t *entry )
{
	assert( NULL != entry );

	free( entry->key );
	free( entry );
}

static void bucket_term( bucket_t *bucket )
{
	entry_t *iter;
	entry_t *next;

	for( iter = bucket->entries; NULL != iter; iter = next )
	{
		next = iter->next;
		entry_destroy( iter );
	}
}

static unsigned int bucket_find( bucket_t *bucket, hashmap_key_t key, hashmap_value_t * const value )
{
	entry_t *iter;
	for( iter = bucket->entries; NULL != iter; iter = iter->next )
	{
		if( 0 == strcmp( key, iter->key ) )
		{
			if( NULL != value )
			{
				*value = iter->value;
			}

			return 1;
		}
	}

	return 0;
}

static int bucket_add( bucket_t *bucket, const hashmap_key_t key, const hashmap_value_t value )
{
	int err = -1;

	assert( NULL != bucket );

	if( 0 == bucket_find( bucket, key, NULL ) )
	{
		entry_t *new_entry = entry_create( key, value );

		if( NULL != new_entry )
		{
			new_entry->next = bucket->entries;
			bucket->entries = new_entry;

			++bucket->stats.num_entries;
			err = 0;
		}
	}

	return err;
}

int hashmap_init( hashmap_t *hashmap )
{
	assert( NULL != hashmap );

	return hashmap_init_with_buckets( hashmap, HASHMAP_DEFAULT_BUCKETS );
}

int hashmap_init_with_buckets( hashmap_t *hashmap, size_t num_buckets )
{
	int err = -1;

	assert( NULL != hashmap );
	assert( num_buckets > 0 );

	hashmap->num_buckets = num_buckets;
	hashmap->buckets = calloc( hashmap->num_buckets, sizeof(bucket_t) );

	if( NULL != hashmap->buckets )
	{
		err = 0;
	}

	return err;
}

void hashmap_term( hashmap_t *hashmap )
{
	unsigned int i;

	assert( NULL != hashmap );

	for( i = 0; i < hashmap->num_buckets; ++i )
	{
		bucket_term( &hashmap->buckets[i] );
	}

	free( hashmap->buckets );
}

hashmap_t *hashmap_create( void )
{
	return hashmap_create_with_buckets( HASHMAP_DEFAULT_BUCKETS );
}

hashmap_t *hashmap_create_with_buckets( size_t num_buckets )
{
	hashmap_t *hashmap;

	assert( num_buckets > 0 );

	hashmap = malloc( sizeof(hashmap_t) );

	if( NULL != hashmap )
	{
		int err = hashmap_init( hashmap );

		if( 0 != err )
		{
			free( hashmap );
			hashmap = NULL;
		}
	}

	return hashmap;
}

void hashmap_destroy( hashmap_t *hashmap )
{
	assert( NULL != hashmap );

	hashmap_term( hashmap );

	free( hashmap );
}

/* Reference for hashing functions:
   http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
*/
#define generate_hash oat_hash

__attribute__((unused)) static size_t djb_hash( void *key, size_t len )
{
	unsigned char *p = key;
	size_t hash = 5381;
	size_t i;

	for( i = 0; i < len; ++i )
	{
		hash = hash * 33 + p[i];
	}

	return hash;
}

static size_t oat_hash( void *key, size_t len )
{
	unsigned char *p = key;
	unsigned int hash = 0;
	unsigned int i;

	for( i = 0; i < len; ++i )
	{
		hash += p[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

static size_t hashmap_bucket_index( const hashmap_t *hashmap, hashmap_key_t key )
{
	assert( NULL != hashmap );
	assert( NULL != key );

	return generate_hash( key, strlen( key ) ) % hashmap->num_buckets;
}

static bucket_t *hashmap_bucket_get( hashmap_t *hashmap, hashmap_key_t key )
{
	size_t index;

	assert( NULL != hashmap );
	assert( NULL != key );

	index = hashmap_bucket_index( hashmap, key );
	return &hashmap->buckets[index];
}

int hashmap_add( hashmap_t *hashmap, hashmap_key_t key, hashmap_value_t value )
{
	bucket_t *bucket = hashmap_bucket_get( hashmap, key );
	return bucket_add( bucket, key, value );
}

void hashmap_stats_fprintf( FILE *fp, const hashmap_t *hashmap )
{
	unsigned int i;

	assert( NULL != fp );
	assert( NULL != hashmap );

	for( i = 0; i < hashmap->num_buckets; ++i )
	{
		fprintf( fp, "%u\n", hashmap->buckets[i].stats.num_entries );
	}
}

unsigned int hashmap_find( hashmap_t *hashmap, hashmap_key_t key, hashmap_value_t * const value )
{
	bucket_t *bucket = hashmap_bucket_get( hashmap, key );
	return bucket_find( bucket, key, value );
}
