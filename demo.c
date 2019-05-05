#include <stdio.h>
#include <stdlib.h>
#include "cex.h"

#define try cex_try(&st)
#define catch cex_catch_block(&st)
#define catch_case(T) cex_catch(&st, T)

int main( )
{
	struct cex_stack st;
	if ( cex_stack_init_alloc( &st, 256 ) )
		abort( ); 
		
	try
	{
		printf( "Before throw!\n" );
		
		
		cex_throw( &st, CEX_STRING, "asd" );
		//union cex_payload payload = {.str="error!"};
		// cex_throw( &st, payload, CEX_STRING );
		
		
		printf( "After throw!\n" );
	}
	catch
	{
		printf( "In catch block\n" );
		
		catch_case( CEX_STRING )
		{
			printf( "Caught string %s\n", cex_ex );
		}
		
		printf( "Stack size: %d\n", (int) cex_stack_size( &st ) );
	}
	
	printf( "After all...\n" );
	return 0;
}
