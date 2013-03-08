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
	if (argc<3)
	{
		printf("-----Invalid number of arguments-----\n\nUsage:\n\t%s compile <filename> - compiles a script to bytecode\n\t%s exec(ute) <filename> - executes the script directly\n\t%s hash <string>  - uses the native hashing function and returns the result",argv[0],argv[0],argv[0]);
	}
	else if (strcmp(argv[1],"compile")==0)
	{
		MEMORY_ALLOCATION m;
		FILE* f = fopen(argv[2],"r");
		if (f)
			gen_bytecode(f, &m, error);
		else
			error(0,0,"File cannot be opened");
	}
	else if (strcmp(argv[1],"exec")==0 || strcmp(argv[1],"execute")==0)
	{
		// executes the file
	}
	else if (strcmp(argv[1],"hash")==0)
	{
		printf("\n\"%s\" => %x\n", argv[2], hash(argv[2], 0, strlen(argv[2])));
	}
	return 0;
}