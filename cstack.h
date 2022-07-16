#ifndef __CSTACK_H__
#define __CSTACK_H__

/* Necessary C library */
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Structures */
typedef struct {
	size_t capacity, item;
	size_t top;
	void *data, *ref;
} cstack ;

/* Functions Defination */
/* CREATE DESTROY */
cstack cstack_create ( size_t item );
void cstack_destroy ( cstack* cst );

/* Element access */
void* cstack_top ( cstack* cst );
void* cstack_data ( cstack* cst );

/* Capacity */
bool cstack_empty ( cstack* cst );
size_t cstack_size ( cstack* cst );
//TODO: Reserve function is not provided, maybe not need so...
size_t cstack_capacity ( cstack* cst );

/* MODIFIERS */
void cstack_push ( cstack* cst, void* elem );
void cstack_pop ( cstack* cst );
void cstack_make_empty ( cstack* cst );

/* Printer for debugger */
void cstack_print ( cstack* cst, void (*print) ( void* ) );

/* Functions Implementation */
cstack cstack_create ( size_t item )
{
	cstack cst = {
		.capacity = 0,
		.item = item,
		.top = 0,
		.data = NULL,
		.ref = NULL
	};
	return cst;
}

void cstack_destroy ( cstack* cst )
{
	cst -> capacity = 0;
	cst -> item = 0;
	cst -> top = 0;
	free ( cst -> data );
	cst -> data = NULL;
	cst -> ref = NULL;
}

void* cstack_top ( cstack* cst )
{
	assert ( cst->top > (size_t)0 );
	return cst->ref = cst->data + ((cst->top - 1) * cst->item);
}

void* cstack_data ( cstack* cst )
{
	return cst->ref = cst->data;
}

bool cstack_empty ( cstack* cst )
{
	return ( cst->top == (size_t)0 );
}

size_t cstack_size ( cstack* cst )
{
	return cst->top;
}

size_t cstack_capacity ( cstack* cst )
{
	return cst -> capacity;
}

void cstack_push ( cstack* cst, void* elem )
{
	if ( cst->top == cst->capacity ) {
		cst->capacity == 0 ? (cst->capacity = 1) : (cst->capacity *= 2);
		cst->data = realloc ( cst->data, cst->item * cst->capacity );
		assert ( cst->data != NULL );
	}
	memcpy ( cst->data + (cst->top * cst->item), elem, cst->item );
	cst->top += 1;
}

void cstack_pop ( cstack* cst )
{
	assert ( cst->top != (size_t)0 );
	cst->top -= (size_t)1;
}

void cstack_make_empty ( cstack* cst )
{
	cst->top = 0;
}

void cstack_print ( cstack* cst, void (*print) ( void* ) )
{
	printf ( "Printing Stack:\n" );
	printf ( "Capacity : %zu\n", cst->capacity );
	printf ( "Item : %zu\n", cst->item );
	printf ( "Top : %zu\n", cst->top );
	printf ( "Printing the data.\n" );
	for ( size_t i = 0 ; i < cst->top ; ++i ) {
		cst->ref = (cst->data + (cst->item * i));
		print ( cst->ref );
	}
	puts ( "" );
}

#endif
