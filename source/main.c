#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"

int callback(void* data, unsigned len)
{
	printf("Callback!! %d bytes", len);
}
void error(unsigned chr, unsigned line, const char* error_msg)
{
	fprintf(stderr,"On line %d, char %d:\n\t%s", line, chr, error_msg);
}

int main(int argc, char** argv)
{
	const char* code = "x=1\nx=2\n";
	gen_bytecode(code, strlen(code), 1, callback, error);
	return 0;
}