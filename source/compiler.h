#ifndef _COMPILER
#define _COMPILER

typedef void (*ERROR_CALLBACK) (unsigned chr, unsigned line, const char* error_msg);
void gen_bytecode(const char* src, unsigned src_len, void** out_ptr, unsigned* out_len, ERROR_CALLBACK err);

#endif