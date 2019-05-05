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

// User-defined error callback function
void error_callback( struct cex_status *st, enum cex_error err )
{
	const char *mesg;
	switch ( err )
	{
		case CEX_NO_ERROR:
			mesg = "CEX_NO_ERROR";
			break;
			
		case CEX_ALLOC_ERROR:
			mesg = "CEX_ALLOC_ERROR";
			break;
	
		case CEX_STACK_OVERFLOW:
			mesg = "CEX_STACK_OVERFLOW";
			break;
			
		case CEX_STACK_UNDERFLOW:
			mesg = "CEX_STACK_UNDERFLOW";
			break;
			
		case CEX_UNCAUGHT:
			mesg = "CEX_UNCAUGHT";
			break;
		
		default:
			mesg = "???";
			break;
	}
	
	fprintf( stderr, "CEX error: %s\n", mesg );
	abort( );
}

void foo( )
{
	throw( CEX_STRING, "throw from foo()" );
}

int main( )
{
	// Initialize CEX
	if ( cex_init_alloc( &CEX, 256 ) )
		abort( );
	
	// Initialize error callback
	CEX.error_callback = error_callback;
	
	try
	{
		printf( "in try{}\n" );
		
		try
		{
			printf( "in try{try{}}\n" );
			
			// Throw things here
			// throw( CEX_STRING, "'Exception description'" );
			throw( CEX_INT, 17 );
			// throw( CEX_VOID_PTR, NULL );
			
			printf( "never reached\n" );
		}
		catch
		{	
			printf( "in try{catch{}}\n" );
			
			// Handles only strings
			catch_case( CEX_STRING )
			{
				// cex_what contains payload in catch cases
				printf( "caught string: %s\n", cex_what );
			}

		}
	}
	catch
	{
		printf( "in catch{}\n" );
		
		catch_case( CEX_INT )
		{
			printf( "caught int: %d\n", cex_what );
		}
	}
	
	try
	{
		foo( );
	}
	catch catch_case( CEX_STRING )
	{
		printf( "caught string down here: %s\n", cex_what );
	}
	
	// Cleanup
	cex_delete( &CEX );
	return 0;
}
