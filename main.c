#include <assert.h>

#include "hashmap.h"

int main( int argc, char *argv[] )
{
	int res, val;
	hashmap_t *hashmap;

	(void) argc;
	(void) argv;

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

/*	hashmap_stats_fprintf( stdout, hashmap );*/
	hashmap_destroy( hashmap );

	return 0;
}
