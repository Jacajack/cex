#include "cex.h"
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

static void cex_abort( )
{
	abort( );
}

/**
	\brief Initialize exception stack in memory chunck allocated with malloc().
	
	Stack memory shall be later freed with cex_stack_destroy()
*/
int cex_stack_init_alloc( struct cex_stack *st, size_t size )
{
	st->max_size = size;
	st->bp = malloc( sizeof( struct cex_exception ) * size );
	st->sp = st->bp;
	
	return st->bp == NULL;
}

/**
	\brief Initialize exception stack based on fixed-size array
	
	After initializing with this function cex_stack_destroy() shall *not* be called
*/
void cex_stack_init_fixed( struct cex_stack *st, struct cex_exception *arr, size_t size )
{
	st->max_size = size;
	st->bp = arr;
	st->sp = st->bp;
}

void cex_stack_delete( struct cex_stack *st )
{
	free( st->bp );
	st->bp = st->bp = NULL;
}



void cex_push( struct cex_stack *st )
{	
	if ( ++st->sp - st->bp == st->max_size )
		cex_abort( );
}

struct cex_exception *cex_top( struct cex_stack *st )
{
	if ( st->sp == st->bp )
		cex_abort( );
	return st->sp;
}

void cex_pop( struct cex_stack *st )
{
	if ( st->sp-- == st->bp )
		cex_abort( );
}

void cex_generic_throw( struct cex_stack *st, union cex_payload payload, enum cex_type type )
{
	struct cex_exception *ex = cex_top( st );
	ex->payload = payload;
	ex->type = type;
	ex->caught = 0;
	longjmp( ex->jbuf, 1 );
}

void cex_propagate( struct cex_stack *st )
{
	struct cex_exception bak = *cex_top( st );
	cex_pop( st );

	if ( !bak.caught )
	{	
		struct cex_exception *ex = cex_top( st );
		cex_generic_throw( st, ex->payload, ex->type );
	}
}

