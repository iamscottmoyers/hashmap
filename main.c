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

	res = hashmap_find( hashmap, "test", &val );
	assert( 1 == res );
	assert( 100 == val );

/*	hashmap_stats_fprintf( stdout, hashmap );*/
	hashmap_destroy( hashmap );

	return 0;
}
