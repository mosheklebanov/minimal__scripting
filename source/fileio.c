#include <stdlib.h>
#include <stdio.h>
#include "fileio.h"
#include "compiler.h"

int load_from_file(const char* filename, MEMORY_ALLOCATION* memory)
{
	FILE* f = fopen(filename, "r");
	if (f==NULL)
		return -2;
	if (fread(&(memory->cnsts_ptr_size), 4, 1, f)<4)
		return -1;
	memory->cnsts_ptr = malloc(memory->cnsts_ptr_size);
	if (fread(memory->cnsts_ptr, memory->cnsts_ptr_size, 1, f) < memory->cnsts_ptr_size)
		return -1;
	
	if (fread(&(memory->cllbls_ptr_size), 4, 1, f)<4)
		return -1;
	memory->cllbls_ptr = malloc(memory->cllbls_ptr_size);
	if (fread(memory->cllbls_ptr, memory->cllbls_ptr_size, 1, f) < memory->cllbls_ptr_size)
		return -1;
	
	if (fread(&(memory->code_ptr_size), 4, 1, f)<4)
		return -1;
	memory->code_ptr = malloc(memory->code_ptr_size);
	if (fread(memory->code_ptr, memory->code_ptr_size, 1, f) < memory->code_ptr_size)
		return -1;
	
	fclose(f);
	return 0;
	
	
}
int save_to_file(const char* filename, MEMORY_ALLOCATION* memory)
{
	FILE* f = fopen(filename, "w");
	if (f==NULL)
		return -2;
	if (fwrite(&(memory->cnsts_ptr_size), 4, 1, f)<4)
		return -1;
	if (fwrite(memory->cnsts_ptr, memory->cnsts_ptr_size, 1, f) < memory->cnsts_ptr_size)
		return -1;
	
	if (fwrite(&(memory->cllbls_ptr_size), 4, 1, f)<4)
		return -1;
	if (fwrite(memory->cllbls_ptr, memory->cllbls_ptr_size, 1, f) < memory->cllbls_ptr_size)
		return -1;
	
	if (fwrite(&(memory->code_ptr_size), 4, 1, f)<4)
		return -1;
	if (fwrite(memory->code_ptr, memory->code_ptr_size, 1, f) < memory->code_ptr_size)
		return -1;
	
	fclose(f);
	return 0;
}

