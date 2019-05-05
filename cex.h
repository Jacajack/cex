#ifndef CEX_H
#define CEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <setjmp.h>
#include <stdlib.h>

/*
	Payload types for macros
*/
typedef const char *CEX_STRING_TYPE;
typedef int         CEX_INT_TYPE;
typedef void       *CEX_VOID_PTR_TYPE;

/**
	\brief Stores one of many possible exception payloads
*/
union cex_payload
{
	CEX_STRING_TYPE   CEX_STRING_VAL;
	CEX_VOID_PTR_TYPE CEX_VOID_PTR_VAL;
	CEX_INT_TYPE      CEX_INT_VAL;
};

/**
	\brief Exception payload types
*/
enum cex_type
{
	CEX_STRING,
	CEX_INT,
	CEX_VOID_PTR
};

/**
	\brief A wrapper for jmp_buf so it's more implementation
	independent and easier to use.
*/
struct cex_jmp_buf_wrapper
{
	jmp_buf jbuf;
};

/**
	\brief Possible error values handled by the 
*/
enum cex_error
{
	CEX_NO_ERROR = 0,
	CEX_STACK_UNDERFLOW,
	CEX_STACK_OVERFLOW,
	CEX_UNCAUGHT
};

/**
	\brief The status struct.
	
	This struct contains a stack of try-catch block jump locations and stores
	currently processed exception.
*/
struct cex_status
{
	// The jump location stack
	unsigned char stack_allocated;     //!< Was the stack allocated with malloc
	size_t max_stack_size;             //!< Max stack size 
	struct cex_jmp_buf_wrapper *bp;    //!< Stack base pointer
	struct cex_jmp_buf_wrapper *sp;    //!< Stack pointer
	
	// The exception
	union cex_payload ex_payload;
	enum cex_type     ex_type;
	unsigned char     ex_caught;
	
	//! Error handler function
	void ( *error_callback )( struct cex_status *st, enum cex_error err );
};


// CEX status management
extern int cex_init_alloc( struct cex_status *st, size_t size );
extern void cex_init_fixed( struct cex_status *st, struct cex_jmp_buf_wrapper *array, size_t size );
extern void cex_delete( struct cex_status *st );

// Jump location stack access
extern size_t cex_stack_size( struct cex_status *st );
extern void cex_push( struct cex_status *st );
extern struct cex_jmp_buf_wrapper *cex_top( struct cex_status *st );
extern struct cex_jmp_buf_wrapper cex_pop( struct cex_status *st );

// Exception throwing and propagation
extern void cex_generic_throw( struct cex_status *st, union cex_payload payload, enum cex_type type );
extern void cex_propagate( struct cex_status *st );

// Throw
#define cex_throw(st, T, value) { union cex_payload p = {.T##_VAL = value}; cex_generic_throw( &(st), p, (T) ); }

// Try and catch blocks
#define cex_try_block(st)       cex_push( &(st) ); if ( !setjmp( cex_top( &(st) )->jbuf ) ) {
#define cex_catch_block(st)     cex_pop( &(st) ); } else for ( cex_pop( &(st) ); !(st).ex_caught; cex_propagate( &(st) ) )

// Catchers
#define cex_catch_case(st, T)   for ( T##_TYPE cex_what; !(st).ex_caught && (st).ex_type == (T) && ( cex_what = (st).ex_payload. T##_VAL, (st).ex_caught = 1, 1 ); )
#define cex_catch_all(st)       if ( !(st).ex_caught && ( ( (st).ex_caught = 1 ), 1 ) )

#ifdef __cplusplus
}
#endif

#endif
