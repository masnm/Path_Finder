#ifndef __CQUEUE_H__
#define __CQUEUE_H__

/* Necessary C library */
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Structures */
typedef struct {
	size_t capacity, item;
	size_t start, end;
	void *data, *ref;
} cqueue ;

/* Functions Defination */
/* CREATE DESTROY */
cqueue cqueue_create ( size_t item );
void cqueue_destroy ( cqueue* vec );

/* Element access */
void* cqueue_front ( cqueue* vec );
void* cqueue_back ( cqueue* vec );
void* cqueue_data ( cqueue* vec );

/* Capacity */
bool cqueue_empty ( cqueue* que );
size_t cqueue_size ( cqueue* que );
void cqueue_reserve ( cqueue* que, size_t capacity );
size_t cqueue_capacity ( cqueue* que );

/* MODIFIERS */
void cqueue_push ( cqueue* vec, void* elem );
void cqueue_pop ( cqueue* vec );

/* Printer for debugger */
void cqueue_print ( cqueue* que, void (*print) ( void* ) );

/* Functions Implementation */
cqueue cqueue_create ( size_t item )
{
	cqueue que = {
		.capacity = 0,
		.item = item,
		.start = 0,
		.end = 0,
		.data = NULL,
		.ref = NULL
	};
	return que;
}

void cqueue_destroy ( cqueue* que )
{
	que -> capacity = 0;
	que -> item = 0;
	que -> start = 0;
	que -> end = 0;
	free ( que -> data );
	que -> data = NULL;
	que -> ref = NULL;
}

void* cqueue_front ( cqueue* que )
{
	assert ( que->start < que->end );
	return que->ref = que->data + (que->start * que->item);
}

void* cqueue_back ( cqueue* que )
{
	assert ( que->start < que->end );
	return que->ref = que->data + ((que->end - 1) * que->item);
}

void* cqueue_data ( cqueue* que )
{
	return que->ref = que->data;
}

bool cqueue_empty ( cqueue* que )
{
	return que->start >= que->end;
}

size_t cqueue_size ( cqueue* que )
{
	return que->end - que->start;
}

void cqueue_reserve ( cqueue* que, size_t capacity )
{
	assert ( capacity <= ((size_t)1 << ((sizeof(size_t)*8)-1)) );
	size_t count = 0;
	for ( size_t i = 1 ; i <= capacity ; i <<= 1 ) {
		if ( capacity & i ) ++count;
	}
	assert ( count == 1 && "capacity must be power of 2" );
	que -> capacity = capacity;
	que -> data = realloc ( que->data, que->item * que->capacity );
	assert ( que -> data != NULL );
}

size_t cqueue_capacity ( cqueue* que )
{
	return que -> capacity;
}

void cqueue_push ( cqueue* que, void* elem )
{
	if ( que->end == que->capacity ) {
		que->capacity == 0 ? (que->capacity = 1) : (que->capacity *= 2);
		que->data = realloc ( que->data, que->item * que->capacity );
		assert ( que->data != NULL );
	}
	memcpy ( que->data + (que->end * que->item), elem, que->item );
	que->end += 1;
}

void cqueue_pop ( cqueue* que )
{
	assert ( que->start < que->end );
	que->start += 1;
	const size_t re_set = 4;
	if ( que->start >= re_set ) {
		void* ndata = malloc ( que->capacity * que->item );
		memcpy ( ndata, que->data+ (que->start * que->item), que->item * (que->end - que->start) );
		free ( que->data );
		que->data = ndata;
		que->start -= re_set;
		que->end -= re_set;
	}
}

void cqueue_print ( cqueue* que, void (*print) ( void* ) )
{
	printf ( "Printing Queue:\n" );
	// size_t capacity, item;
	// size_t start, end;
	// void *data, *ref;
	printf ( "Capacity : %zu\n", que->capacity );
	printf ( "Item : %zu\n", que->item );
	printf ( "start : %zu\n", que->start );
	printf ( "End : %zu\n", que->end );
	printf ( "Printing the data.\n" );
	for ( size_t i = que->start ; i < que->end ; ++i ) {
		que->ref = (que->data + (que->item * i));
		print ( que->ref );
	}
	puts ( "" );
}

#endif
