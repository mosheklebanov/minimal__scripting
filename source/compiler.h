#ifndef _COMPILER
#define _COMPILER
#include <stdint.h>

typedef struct memory_allocation_t
{
	unsigned	writeable;
	uint8_t*	cnsts_ptr;
	unsigned	cnsts_ptr_size;
	unsigned	cnsts_ptr_allocd;
	uint32_t*	cllbls_ptr;
	unsigned	cllbls_ptr_size;
	unsigned	cllbls_ptr_allocd;
	uint32_t*	code_ptr;
	unsigned	code_ptr_size;
	unsigned	code_ptr_allocd;
} MEMORY_ALLOCATION;

void free_memory_allocation_internal(MEMORY_ALLOCATION* m);

typedef void (*ERROR_CALLBACK) (unsigned chr, unsigned line, const char* error_msg);
void gen_bytecode(const char* src, unsigned src_len, MEMORY_ALLOCATION* memory, ERROR_CALLBACK err);

#endif