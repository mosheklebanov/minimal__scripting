#ifndef _FILEIO
#define _FILEIO
#include "compiler.h"

int load_from_file(const char* filename, MEMORY_ALLOCATION* memory);
int save_to_file(const char* filename, MEMORY_ALLOCATION* memory);

#endif
