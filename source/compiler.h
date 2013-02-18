#ifndef _COMPILER
#define _COMPILER

typedef int (*CALLBACK) (void* data, unsigned len);
typedef void (*ERROR) (unsigned chr, unsigned line, const char* error_msg);
void gen_bytecode(const char* src, unsigned src_len, int entire, CALLBACK cb, ERROR err);

#endif