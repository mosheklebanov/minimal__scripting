#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#include "fileio.h"

void error(unsigned chr, unsigned line, const char* error_msg)
{
	fprintf(stderr,"On line %d, char %d:\n\t%s", line, chr, error_msg);
}

int main(int argc, char** argv)
{
	//const char* code = "(((2+3)*7)-2)/3\n";
	const char* code = "1+2\n";
	MEMORY_ALLOCATION m;
	memset(&m, 0, sizeof(MEMORY_ALLOCATION));
	gen_bytecode(code, strlen(code), &m, error);
	
	printf("cnsts_ptr_size %d\n",m.cnsts_ptr_size);
	printf("cllbls_ptr_size %d\n",m.cllbls_ptr_size);
	printf("code_ptr_size %d\n",m.code_ptr_size);
	
	
	printf("Save to file: %d", save_to_file("prog1.bin", &m));
	return 0;
}