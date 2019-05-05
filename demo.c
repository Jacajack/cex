#include <stdio.h>
#include <stdlib.h>
#include "cex.h"

struct cex_status CEX;

// Macros to make life easier
#define GLOBAL_CEX      CEX
#define try             cex_try_block(GLOBAL_CEX)
#define catch           cex_catch_block(GLOBAL_CEX)
#define catch_case(T)   cex_catch_case(GLOBAL_CEX, T)
#define catch_all       cex_catch_all(GLOBAL_CEX)
#define throw(T, value) cex_throw(GLOBAL_CEX, T, (value)) 


int main( )
{
	// Initialize CEX
	if ( cex_init_alloc( &CEX, 256 ) )
		abort( );
	
	try
	{
		printf( "in try{}\n" );
		
		// Something thrown here
		throw( CEX_STRING, "'Exception description'" );
		throw( CEX_INT, 17 );
		
		// Never reached
		printf( "after throw\n" );
	}
	catch
	{	
		// Handles only strings
		catch_case( CEX_STRING )
		{
			// cex_what contains payload in catch cases
			printf( "Caught string: %s\n", cex_what );
		}
		
		// Handles all cases
		catch_all
		{
			printf( "Caught something\n" );
		}
	}

	cex_delete( &CEX );

	return 0;
}
