#ifndef CEX_H
#define CEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <setjmp.h>
#include <stdlib.h>

enum cex_type
{
	CEX_STRING,
	CEX_INT,
	CEX_VOID_PTR
};

typedef const char *CEX_STRING_TYPE;
typedef int CEX_INT_TYPE;
typedef void *CEX_VOID_PTR_TYPE;

union cex_payload
{
	CEX_STRING_TYPE   CEX_STRING_VAL;
	CEX_VOID_PTR_TYPE CEX_VOID_PTR_VAL;
	CEX_INT_TYPE      CEX_INT_VAL;
};

struct cex_exception
{
	union cex_payload payload;
	enum cex_type type;
	jmp_buf jbuf;
	unsigned char caught;
};

struct cex_stack
{
	size_t max_size;
	struct cex_exception *bp;
	struct cex_exception *sp;
};




// Stack creation/destruction
extern int cex_stack_init_alloc( struct cex_stack *st, size_t size );
extern void cex_stack_init_fixed( struct cex_stack *st, struct cex_exception *array, size_t size );
extern void cex_stack_delete( struct cex_stack *st );

// Exception stack access
extern void cex_push( struct cex_stack *st );
extern struct cex_exception *cex_top( struct cex_stack *st );
extern void cex_pop( struct cex_stack *st );

// Propagates uncaught exception down the stack. Calls terminate() eventually.
extern void cex_generic_throw( struct cex_stack *st, union cex_payload payload, enum cex_type type );
extern void cex_propagate( struct cex_stack *st );

// Macros for user
#define cex_try(st)     cex_push((st)); if ( !setjmp(cex_top((st))->jbuf) ) {
#define cex_catch_block(st) cex_pop((st)); } else for( ;; cex_propagate((st)) )
#define cex_throw(st, T, value) { union cex_payload p = {.T##_VAL = value}; cex_generic_throw( st, p, T ); }

#define cex_catch(st, T) for(T##_TYPE cex_ex; cex_top((st))->type==(T) && !cex_top((st))->caught && (cex_ex=cex_top((st))->payload. T##_VAL, cex_top((st))->caught=1, 1) ; )

#ifdef __cplusplus
}
#endif

#endif
