#include <assert.h>
#include <stdlib.h>

#include "hashmap.h"

static void arbitrary( void )
{
	int res, val;
	hashmap_t *hashmap;

	hashmap = hashmap_create();
	assert( hashmap );

	res = hashmap_add( hashmap, "test", 100 );
	assert( 0 == res );

	res = hashmap_add( hashmap, "test", 101 );
	assert( 0 == res );

	res = hashmap_add( hashmap, "test2", 3 );
	assert( 0 == res );

	res = hashmap_find( hashmap, "test2", NULL );
	assert( 1 == res );

	res = hashmap_find( hashmap, "test2", &val );
	assert( 1 == res );
	assert( 3 == val );

	res = hashmap_find( hashmap, "test", &val );
	assert( 1 == res );
	assert( 101 == val );

	hashmap_remove( hashmap, "test" );
	res = hashmap_find( hashmap, "test", NULL );
	assert( 0 == res );
	hashmap_remove( hashmap, "test" );
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

		res = hashmap_add( &hashmap, str, val );
		assert( 0 == res );

		res = hashmap_find( &hashmap, str, &retrieved );
		assert( 1 == res );
		assert( retrieved == val );
	}

	hashmap_term( &hashmap );
}

int main( int argc, char *argv[] )
{
	(void) argc;
	(void) argv;

	arbitrary();
	random_insertions();

	return 0;
}
