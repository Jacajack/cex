# cex
CEX is a simple and lightweight exceptions library based on `setjmp.h`.
It provides a set of preprocessor macros imitating C++ `try`, `throw` and `catch` functionality.

### Usage
A global variable of `struct cex_status` shall be defined by user. On program startup, it must be initialized either with `cex_init_alloc()` or `cex_init_fixed()` depending on whether the user wants `malloc()` to be used in his application. The status structure has to be cleaned up with `cex_delete()` at the end of runtime.

Example use:
```c
// These can handle up to 256 nested try blocks
struct cex_status CEX;
cex_init_alloc( &CEX, 256 );
// or
struct cex_jmp_buf_wrapper jmp_stack[256];
cex_init_fixed( &CEX, jmp_stack, sizeof( jmp_stack ) / sizeof( jmp_stack[0] ) );

cex_try_block( CEX )
{
	foo( );
	cex_throw( CEX, CEX_STRING, "error message!" ); 
	bar( );
}
cex_catch_block( CEX )
{
	// Code here will be executed once upon
	// throwing an exception in the try block.
	// ! Exceptions cannot be thrown directly in this block !
	
	// Exceptions can be thrown in cex_catch_case/cex_catch_all blocks
	cex_catch_case( CEX, CEX_STRING )
	{
		// Each cex_catch_case block defines local variable named
		// cex_what, containing exception payload.
	
		fprintf( stderr, "%s\n", cex_what );
	}
	cex_catch_case( CEX, CEX_INT )
	{
		fprintf( stderr, "%d\n", cex_what );
	}
	cex_catch_all( CEX )
	{
		// cex_catch_all must come last in cex_catch_block
		// cex_catch_all does not define cex_what!
	
		fprintf( stderr, "some exception caught!\n" );
	}
}

// Cleanup
cex_delete( &CEX );
```

This set of macros can make syntax much readable:
```c
#define GLOBAL_CEX      CEX
#define try             cex_try_block(GLOBAL_CEX)
#define catch           cex_catch_block(GLOBAL_CEX)
#define catch_case(T)   cex_catch_case(GLOBAL_CEX, T)
#define catch_all       cex_catch_all(GLOBAL_CEX)
#define throw(T, value) cex_throw(GLOBAL_CEX, T, (value)) 

try
{
	throw( CEX_INT, 78 );
}
catch
{
	catch_case( CEX_INT )
	{
	}
	catch_all
	{
	}
}
```

### Adding custom exception types

Assume you wanted to add a new exception type `struct foo` as `EX_FOO`. There are three changes you need to make in `cex.h`.
 - Define a new type for the exception. The name *must* be your exception name with `_TYPE` suffix. In this case, you have to add `typedef struct foo EX_FOO_TYPE;`.
 - Add your exception identifier to `enum cex_type` - in this case: `EX_FOO`.
 - Add proper payload declaration to `union cex_payload`. Use the `CEX_DECLARE_PAYLOAD(x)` macro. In this case, the declaration should look like `CEX_DECLARE_PAYLOAD( EX_FOO );`
 

