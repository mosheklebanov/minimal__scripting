#ifndef _COMPILER
#define _COMPILER

typedef struct memory_allocation_t
{
	void* ptr;
	unsigned size;
} MEMORY_ALLOCATION;

typedef void (*ERROR_CALLBACK) (unsigned chr, unsigned line, const char* error_msg);
void gen_bytecode(const char* src, unsigned src_len, MEMORY_ALLOCATION* memory, ERROR_CALLBACK err);

#endif