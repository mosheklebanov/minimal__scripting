#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

void error(unsigned chr, unsigned line, const char* error_msg)
{
	fprintf(stderr,"On line %d, char %d:\n\t%s", line, chr, error_msg);
}

int main(int argc, char** argv)
{
	void* bc;
	unsigned bc_len;
	
	const char* code = "2+(1+1)*3\n";
	gen_bytecode(code, strlen(code), 1, &bc, &bc_len, error);
	return 0;
}