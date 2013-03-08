#ifndef _COMPILER
#define _COMPILER
#include <stdint.h>
#include <stdio.h>

typedef struct memory_allocation_t
{
	unsigned	writeable;
	uint8_t*	cnsts_ptr;
	uint32_t	cnsts_ptr_size;
	uint32_t	cnsts_ptr_allocd;
	uint32_t*	cllbls_ptr;
	uint32_t	cllbls_ptr_size;
	uint32_t	cllbls_ptr_allocd;
	uint32_t*	code_ptr;
	uint32_t	code_ptr_size;
	uint32_t	code_ptr_allocd;
} MEMORY_ALLOCATION;

void free_memory_allocation_internal(MEMORY_ALLOCATION* m);

uint32_t hash(const char* s, unsigned start, unsigned end);
typedef void (*ERROR_CALLBACK) (unsigned chr, unsigned line, const char* error_msg);
void gen_bytecode(FILE* in, MEMORY_ALLOCATION* memory, ERROR_CALLBACK err);

#endif