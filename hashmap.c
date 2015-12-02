#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "hashmap.h"

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

static int bucket_add( bucket_t *bucket, hashmap_key_t key, hashmap_value_t value )
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

#define HASHMAP_BUCKETS (2048)

int hashmap_init( hashmap_t *hashmap )
{
	int err = -1;

	assert( NULL != hashmap );

	hashmap->buckets = calloc( HASHMAP_BUCKETS, sizeof(bucket_t) );

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

	for( i = 0; i < HASHMAP_BUCKETS; ++i )
	{
		bucket_term( &hashmap->buckets[i] );
	}

	free( hashmap->buckets );
}

hashmap_t *hashmap_create( void )
{
	hashmap_t *hashmap = malloc( sizeof(hashmap_t) );

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

/* djb2 string hash function, see http://www.cse.yorku.ca/~oz/hash.html */
static unsigned long string_hash( const char *string )
{
	unsigned long hash = 5381;
	int c;

	while( (c = *string++) )
	{
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}

static size_t hashmap_bucket_index( hashmap_key_t key )
{
	return string_hash( key ) % HASHMAP_BUCKETS;
}

static bucket_t *hashmap_bucket_get( hashmap_t *hashmap, hashmap_key_t key )
{
	size_t index;

	assert( NULL != hashmap );
	assert( NULL != key );

	index = hashmap_bucket_index( key );
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

	for( i = 0; i < HASHMAP_BUCKETS; ++i )
	{
		fprintf( fp, "%u\n", hashmap->buckets[i].stats.num_entries );
	}
}

unsigned int hashmap_find( hashmap_t *hashmap, hashmap_key_t key, hashmap_value_t * const value )
{
	bucket_t *bucket = hashmap_bucket_get( hashmap, key );
	return bucket_find( bucket, key, value );
}
