#include <stdio.h>
#include <stdlib.h>
#include "cex.h"

int main( )
{
	struct cex_stack st;
	if ( cex_stack_init_alloc( &st, 256 ) )
		abort( ); 
		
	cex_try(&st)
	{
		printf( "Before throw!\n" );
		
		
		cex_throw( &st, CEX_STRING, "asd" );
		//union cex_payload payload = {.str="error!"};
		// cex_throw( &st, payload, CEX_STRING );
		
		
		printf( "After throw!\n" );
	}
	cex_catch_block(&st)
	{
		printf( "In catch block\n" );
		
		cex_catch( &st, CEX_STRING )
		{
			printf( "Caught string %s\n", cex_ex );
		}
	}
	
	printf( "After all...\n" );
	return 0;
}
