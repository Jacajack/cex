#include "cex.h"
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

/**
	\brief Calls error handling function provided by the user or abort() if there's no such. 
*/
static void cex_abort( struct cex_status *st, enum cex_error err )
{
	if ( st->error_callback != NULL )
		st->error_callback( st, err );
	else
		abort( );
}

/**
	\brief Initialize CEX status and jump location stack in memory chunck allocated with malloc().
	
	Stack memory shall be later freed with cex_stack_destroy()
*/
enum cex_error cex_init_alloc( struct cex_status *st, size_t size )
{
	st->stack_allocated = 1;
	st->max_stack_size = size;
	st->bp = malloc( sizeof( struct cex_jmp_buf_wrapper ) * size );
	st->sp = st->bp;
	
	st->error_callback = NULL;
	return st->bp == NULL ? CEX_ALLOC_ERROR : CEX_NO_ERROR;
}

/**
	\brief Initialize CEX status and jump location stack based on fixed-size array
*/
enum cex_error cex_init_fixed( struct cex_status *st, struct cex_jmp_buf_wrapper *arr, size_t size )
{
	st->stack_allocated = 0;
	st->max_stack_size = size;
	st->bp = arr;
	st->sp = st->bp;
	
	st->error_callback = NULL;
	return CEX_NO_ERROR;
}

/**
	\brief Cleans up a status, frees memory if needed
*/
void cex_delete( struct cex_status *st )
{
	if ( st->stack_allocated ) free( st->bp );
	st->bp = st->sp = NULL;
}

/**
	\brief Adds new jump location to the stack
	
	The jump buffer has to be initialized with setjmp()	

	Calls cex_abort() with CEX_STACK_OVERFLOW if stack is full
*/
void cex_push( struct cex_status *st )
{	
	if ( ++st->sp - st->bp == st->max_stack_size )
		cex_abort( st, CEX_STACK_OVERFLOW );
}

/**
	\brief Returns pointer to the top jump location
	
	Calls cex_abort() with CEX_STACK_UNDERFLOW if stack is empty
*/
struct cex_jmp_buf_wrapper *cex_top( struct cex_status *st )
{
	if ( st->sp == st->bp )
		cex_abort( st, CEX_STACK_UNDERFLOW );
	return st->sp;
}

/**
	\brief Returns stack size
*/
size_t cex_stack_size( struct cex_status *st )
{
	return st->sp - st->bp;
}

/**
	\brief Pops one jump location from the stack and returns it
	
	Calls cex_abort() with CEX_STACK_UNDERFLOW if stack is empty
*/
struct cex_jmp_buf_wrapper cex_pop( struct cex_status *st )
{
	if ( st->sp == st->bp )
		cex_abort( st, CEX_STACK_UNDERFLOW );
	return *st->sp--;
}

/**
	\brief Generic throw function
	
	Throws an exception with certain type and payload.
*/
void cex_generic_throw( struct cex_status *st, union cex_payload payload, enum cex_type type )
{
	struct cex_jmp_buf_wrapper *jbufw = cex_top( st );
	
	st->ex_payload = payload;
	st->ex_type = type;
	st->ex_caught = 0;
	longjmp( jbufw->jbuf, 1 );
}

/**
	\brief Propagates exception.
	
	If the jump location stack is empty calls cex_abort() with CEX_UNCAUGHT
	
	Has effect only if the current exception is uncaught
*/
void cex_propagate( struct cex_status *st )
{
	if ( !st->ex_caught )
	{
		if ( cex_stack_size( st ) != 0 )
		{	
			struct cex_jmp_buf_wrapper *jbufw = cex_top( st );
			longjmp( jbufw->jbuf, 1 );
		}
		else
			cex_abort( st, CEX_UNCAUGHT );
	}
}

