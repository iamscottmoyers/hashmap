#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

static void arbitrary( void )
{
	int res, val;
	hashmap_t *hashmap;

	hashmap = hashmap_create();
	assert( hashmap );

	res = hashmap_insert( hashmap, "test", 100 );
	assert( 0 == res );

	res = hashmap_insert( hashmap, "test", 101 );
	assert( 0 == res );

	res = hashmap_insert( hashmap, "test2", 3 );
	assert( 0 == res );

	res = hashmap_find( hashmap, "test2", NULL );
	assert( 1 == res );

	res = hashmap_find( hashmap, "test2", &val );
	assert( 1 == res );
	assert( 3 == val );

	res = hashmap_find( hashmap, "test", &val );
	assert( 1 == res );
	assert( 101 == val );

	hashmap_erase( hashmap, "test" );
	res = hashmap_find( hashmap, "test", NULL );
	assert( 0 == res );
	hashmap_erase( hashmap, "test" );
	res = hashmap_find( hashmap, "test", NULL );
	assert( 0 == res );

	hashmap_destroy( hashmap );
}

static void random_string( char str[], size_t len )
{
	unsigned int i;
	for( i = 0; i < len - 1; ++i )
	{
		str[i] = (rand() % ('~' - ' ')) - ' ';
	}

	str[len - 1] = '\0';
}

static void random_insertions( void )
{
	int res;
	hashmap_t hashmap;
	unsigned int i;

	res = hashmap_init_with_buckets( &hashmap, 8192 /* pow 2 */ );
	assert( 0 == res );

	for( i = 0; i < 100000; ++i )
	{
		char str[10];
		hashmap_value_t val = rand();
		hashmap_value_t retrieved;

		random_string( str, sizeof(str) );

		res = hashmap_insert( &hashmap, str, val );
		assert( 0 == res );

		res = hashmap_find( &hashmap, str, &retrieved );
		assert( 1 == res );
		assert( retrieved == val );
	}

	hashmap_term( &hashmap );
}

static void test_size( void )
{
	int res;
	hashmap_t hashmap;
	size_t size;
	unsigned int empty;

	res = hashmap_init( &hashmap );
	assert( 0 == res );

	/* Confirm initial size is 0. */
	size = hashmap_size( &hashmap );
	assert( 0 == size );
	empty = hashmap_empty( &hashmap );
	assert( 0 != empty );

	/* Confirm insertion increases by 1. */
	res = hashmap_insert( &hashmap, "test", 1 );
	assert( 0 == res );

	size = hashmap_size( &hashmap );
	assert( 1 == size );
	empty = hashmap_empty( &hashmap );
	assert( 0 == empty );

	/* Confirm duplicate insert doesn't change size. */
	res = hashmap_insert( &hashmap, "test", 100 );
	assert( 0 == res );

	size = hashmap_size( &hashmap );
	assert( 1 == size );
	empty = hashmap_empty( &hashmap );
	assert( 0 == empty );

	/* Confirm insertion increases by 1. */
	res = hashmap_insert( &hashmap, "test2", 4 );
	assert( 0 == res );

	size = hashmap_size( &hashmap );
	assert( 2 == size );
	empty = hashmap_empty( &hashmap );
	assert( 0 == empty );

	/* Confirm removal decreases by 1. */
	hashmap_erase( &hashmap, "test" );

	size = hashmap_size( &hashmap );
	assert( 1 == size );
	empty = hashmap_empty( &hashmap );
	assert( 0 == empty );

	/* Confirm removal of key that doesn't exist doesn't change the size. */
	hashmap_erase( &hashmap, "doesn't exist" );

	size = hashmap_size( &hashmap );
	assert( 1 == size );
	empty = hashmap_empty( &hashmap );
	assert( 0 == empty );

	/* Confirm removal of key that once existed but doesn't now doesn't change the size. */
	hashmap_erase( &hashmap, "test" );

	size = hashmap_size( &hashmap );
	assert( 1 == size );
	empty = hashmap_empty( &hashmap );
	assert( 0 == empty );

	/* Confirm removal decreases by 1. */
	hashmap_erase( &hashmap, "test2" );

	size = hashmap_size( &hashmap );
	assert( 0 == size );
	empty = hashmap_empty( &hashmap );
	assert( 0 != empty );

	hashmap_term( &hashmap );
}

static void iter_test( void )
{
	int res;
	hashmap_t *hashmap;
	hashmap_iter_t iter;
	unsigned int count = 0;

	hashmap = hashmap_create();
	assert( hashmap );

	for( hashmap_iter_begin( hashmap, &iter );
	     !hashmap_iter_end( hashmap, &iter );
	     hashmap_iter_next( hashmap, &iter ) )
	{
		assert( 0 );
	}
	assert( 0 == count );

	res = hashmap_insert( hashmap, "test", 100 );
	assert( 0 == res );

	count = 0;
	for( hashmap_iter_begin( hashmap, &iter );
	     !hashmap_iter_end( hashmap, &iter );
	     hashmap_iter_next( hashmap, &iter ) )
	{
		hashmap_key_t key = hashmap_iter_key( &iter );
		hashmap_value_t value = hashmap_iter_value( &iter );

		assert( 0 == strcmp( key, "test" ) );
		assert( value == 100 );
		++count;
	}

	assert( 1 == count );

	res = hashmap_insert( hashmap, "next", 2 );
	assert( 0 == res );
	res = hashmap_insert( hashmap, "another", 3 );
	assert( 0 == res );
	res = hashmap_insert( hashmap, "yet another", 4 );
	assert( 0 == res );
	res = hashmap_insert( hashmap, "penultimate", 5 );
	assert( 0 == res );
	res = hashmap_insert( hashmap, "final", 5 );
	assert( 0 == res );

	count = 0;
	for( hashmap_iter_begin( hashmap, &iter );
	     !hashmap_iter_end( hashmap, &iter );
	     hashmap_iter_next( hashmap, &iter ) )
	{
		++count;
	}

	assert( 6 == count );


	hashmap_erase( hashmap, "penultimate" );
	count = 0;
	for( hashmap_iter_begin( hashmap, &iter );
	     !hashmap_iter_end( hashmap, &iter );
	     hashmap_iter_next( hashmap, &iter ) )
	{
		++count;
	}

	assert( 5 == count );

	hashmap_destroy( hashmap );
}

int main( int argc, char *argv[] )
{
	(void) argc;
	(void) argv;

	arbitrary();
	random_insertions();
	test_size();
	iter_test();

	return 0;
}
