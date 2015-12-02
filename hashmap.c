#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "hashmap.h"

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

static void bucket_term( bucket_t *bucket )
{
	entry_t *iter;
	entry_t *next;
	for( iter = bucket->entries; NULL != iter; iter = next )
	{
		next = iter->next;
		free( iter );
	}
}

void hashmap_term( hashmap_t *hashmap )
{
	unsigned int i;

	assert( NULL != hashmap );

	for( i = 0; i < HASHMAP_BUCKETS; ++i )
	{
		bucket_term( &hashmap->buckets[i] );
	}
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

static size_t bucket_index( key_t key )
{
	return string_hash( key ) % HASHMAP_BUCKETS;
}

static int bucket_add( bucket_t *bucket, key_t key, value_t value )
{
	int err = -1;
	entry_t *iter;
	entry_t *new_entry;

	assert( NULL != bucket );

	/* See if the key already exists in this bucket. */
	for( iter = bucket->entries; NULL != iter; iter = iter->next )
	{
		if( 0 == strcmp( iter->key, key ) )
		{
			/* The key exists, replace the value and return. */
			iter->value = value;
			return 0;
		}
	}

	/* The key doesn't exist, allocate a new entry and prepend it to the entry list. */
	new_entry = malloc( sizeof(entry_t) );
	if( NULL != new_entry )
	{
		size_t key_len = strlen( key ) + 1;
		key_t key_copy = malloc( key_len );

		if( NULL != key_copy )
		{
			memcpy( key_copy, key, key_len );

			new_entry->key = key_copy;
			new_entry->value = value;

			new_entry->next = bucket->entries;
			bucket->entries = new_entry;

			++bucket->stats.num_entries;
			err = 0;
		}
		else
		{
			free( new_entry );
		}
	}

	return err;
}

static bucket_t *hashmap_bucket_get( hashmap_t *hashmap, key_t key )
{
	assert( NULL != hashmap );
	assert( NULL != key );

	return &hashmap->buckets[bucket_index( key )];
}

int hashmap_add( hashmap_t *hashmap, key_t key, value_t value )
{
	bucket_t *bucket = hashmap_bucket_get( hashmap, key );
	return bucket_add( bucket, key, value );
}

static unsigned int bucket_find( bucket_t *bucket, key_t key, value_t * const value )
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

unsigned int hashmap_find( hashmap_t *hashmap, key_t key, value_t * const value )
{
	bucket_t *bucket = hashmap_bucket_get( hashmap, key );
	return bucket_find( bucket, key, value );
}
