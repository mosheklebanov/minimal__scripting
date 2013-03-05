#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h> //debug only
#include "compiler.h"
#include "bytecode_constants.h"



/*
Constant ALLOC_BLOCK_SIZE
The bytecode buffer will repeatedly malloc() and realloc() data in 'blocks' at a time
Increasing this constant will result is fewer allocations and potential copies, but possibly more wasted memory
*/
#define ALLOC_BLOCK_SIZE 256

/*
Token definition and typedef
Tokens will be gathered for each statement, then evaluated in descending order of depth
*/
#define TOKEN_QUANTITY_PER_STMT 24
typedef struct token_t /* by value */
{
	unsigned chr_pos;
	unsigned depth;
} TOKEN;

#define NEWL '\n'

/*
Error messages
*/
#define IN_BLOCK_NO_MORE_CODE "Source terminates while still in block"
#define TOO_MANY_TOKENS "Too many tokens. Try rewriting your code to decrease code complexity per line."

/*
OPERATOR MACROS
*/
#define IS_ARITHMETIC_OPERATOR(n) ((n)=='+'||(n)=='-'||(n)=='*'||(n)=='/')
#define IS_OPERATOR(n) (IS_ARITHMETIC_OPERATOR(n))
#define OPERATOR_ORDER(n) ((((n)=='*')||((n)=='/'))?2:((((n)=='+')||((n)=='-'))?1:0))

#define ARITHMETIC_OPERATOR_BC_ROOT(n) ((n)=='+'?ADD:((n)=='-'?SUB:((n)=='*'?MUL:((n)=='/'?DIV:0))))

/*
Denotes whether a character is valid for a variable OR number
*/
#define IS_VALID_NAME_CHAR(n) (!IS_OPERATOR(n) && (n)!=NEWL && (n)!='(' && (n)!=')')

/*
Manages depth ordering, order of operations, etc...
`chr* src` is necessary here also - likely that this should be inherited
 from the bytecode generating function
*/
#define TOKEN_SORT_DETERMINANT(tkn) ((((tkn).depth)<<3) + OPERATOR_ORDER(src[tkn.chr_pos]))

/*
SPECIAL MACROS
Reponsible for parsing input - inlined for speed and easy access to variables in local scope
Simplifies access to the parsing mechanism

The following variables MUST be avaliable in the local scope of the calling function:
	const char* src;
	void (*err)(unsigned, unsigned, const char*);
	unsigned chr;
	unsigned line;
	unsigned traversal_depth;
	unsigned token_quantity;
	TOKEN tokens[n];
	unsigned current_obj_start;
	unsigned current_obj_end;
	
*/
#define MOVE_NEXT_LINE() do { \
	if (chr>=src_len && traversal_depth>0) err(chr, line, IN_BLOCK_NO_MORE_CODE); \
	if (chr>=src_len) return; /*stop parsing*/ \
	for (;chr<src_len && src[chr]!=NEWL; chr++) {} /*otherwise, move to the next line and put the cursor there*/ \
} while(0)

#define GATHER_TOKENS() do { \
	unsigned chr2; \
	unsigned token_d=0; \
	for (chr2=chr; chr2<src_len && src[chr2]!=NEWL; chr2++) \
	{ \
		if (IS_OPERATOR(src[chr2])) \
		{ \
			tokens[token_quantity].chr_pos = chr2; \
			tokens[token_quantity].depth = token_d; \
			token_quantity++; \
			if (token_quantity>=TOKEN_QUANTITY_PER_STMT) \
			{ \
				err(chr2, line, TOO_MANY_TOKENS); \
				break; \
			} \
		} \
		else if (src[chr2]=='(') token_d++; \
		else if (src[chr2]==')') token_d--; \
	} \
} while(0)

#define SORT_TOKENS() do { \
	TOKEN t; \
	unsigned i=0; \
	for (;i<(token_quantity-1); i++) \
	{ \
		if (TOKEN_SORT_DETERMINANT(tokens[i])<TOKEN_SORT_DETERMINANT(tokens[i+1])) \
		{ \
			t = tokens[i]; \
			tokens[i] = tokens[i+1]; \
			tokens[i+1] = t; \
		} \
	} \
} while(0)

#define GET_NEXT_OBJECT_FROM_BEG() do { \
	current_obj_start = chr; \
	for(current_obj_end = chr; IS_VALID_NAME_CHAR(src[current_obj_end]); current_obj_end++) {} \
	current_obj_end--; \
} while(0)

#define GET_NEXT_OBJECT_FROM_END() do { \
	current_obj_end = chr; \
	for(current_obj_start = chr; IS_VALID_NAME_CHAR(src[current_obj_start]); current_obj_start--) {} \
	current_obj_start++; \
} while(0)

#define IS_NUMERIC() (atof(src+current_obj_start)!=0 || (memcmp(src+current_obj_start,"0",1)==0) || (memcmp(src+current_obj_start,"0.0",3)==0))
#define IS_STRING_LITERAL() ((src[current_obj_start]==34 && src[current_obj_end]==34) || (src[current_obj_start]==39 && src[current_obj_end]==39))

//credits: http://www.cse.yorku.ca/~oz/hash.html
uint32_t hash(const char* s, unsigned start, unsigned end)
{
	uint32_t hashval = 0;
	unsigned c;
	for (c=start;c<=end;c++)
		hashval = s[c] + (hashval<<6) + (hashval<<16) - hashval;
	return hashval;
}

/*
Special PREPARE_* macros
Ensures there is enough space to write onto the given MEMORY_ALLOCATION
*/

//PREPARE_WRITE_INIT - called to ensure that a MEMORY_ALLOCATION is valid and ready to write to
#define PREPARE_WRITE_INIT(memory) do { \
	if (!(memory->writeable)) \
	{ \
		memory->cnsts_ptr = malloc(ALLOC_BLOCK_SIZE); \
		memory->cnsts_ptr_allocd = ALLOC_BLOCK_SIZE; \
		memory->cllbls_ptr = malloc(ALLOC_BLOCK_SIZE); \
		memory->cllbls_ptr_allocd = ALLOC_BLOCK_SIZE; \
		memory->code_ptr = malloc(ALLOC_BLOCK_SIZE); \
		memory->code_ptr_allocd = ALLOC_BLOCK_SIZE; \
		memory->writeable = 1;\
	}\
} while(0)

#define PREPARE_WRITE_CODE(memory, bytestoadd) do { \
	if (memory->code_ptr_allocd <= (bytestoadd + memory->code_ptr_size)) \
		memory->code_ptr = realloc(memory->code_ptr, memory->code_ptr_allocd+ALLOC_BLOCK_SIZE); \
} while(0)

#define PREPARE_WRITE_CNSTS(memory, bytestoadd) do { \
	if (memory->cnsts_ptr_allocd <= (bytestoadd + memory->cnsts_ptr_size)) \
		memory->code_ptr = realloc(memory->cnsts_ptr, memory->cnsts_ptr_allocd+ALLOC_BLOCK_SIZE); \
} while(0)

#define PREPARE_WRITE_CALLABLE(memory, bytestoadd) do { \
	if (memory->cllbls_ptr_allocd <= (bytestoadd + memory->cllbls_ptr_size)) \
		memory->cllbls_ptr = realloc(memory->cllbls_ptr, memory->cllbls_ptr_allocd+ALLOC_BLOCK_SIZE); \
} while(0)

/*
The main entry to the compiler
Responsible for generating bytecode from a char buffer
The out pointer will be initialized to an internal memory buffer; this will be accessible to the caller after
the function returns; must be free()-d later on to prevent memory leaks
Any compilation requires a bidirectional access to the input
*/

/*
Note: ERROR_CALLBACK will be called when an error results from the compiling procudure.
Is it guaranteed that the error_msg will be non-NULL - the text in error_msg does not need to be freed
*/
void gen_bytecode(const char* src, unsigned src_len, MEMORY_ALLOCATION* memory, ERROR_CALLBACK err)
{
	PREPARE_WRITE_INIT(memory);
	
	unsigned chr = 0;
	unsigned line = 1;
	unsigned traversal_depth = 0;
	unsigned token_quantity = 0;
	TOKEN tokens[TOKEN_QUANTITY_PER_STMT];
	
	unsigned current_obj_start;
	unsigned current_obj_end;
	//while (1)
	//{
		printf("Source length %d\n", src_len);
		GATHER_TOKENS();
		printf("Token quantity %d\n", token_quantity);
		SORT_TOKENS();
		int i;
		for (i=0;i<token_quantity;i++)
		{
			int32_t command = ARITHMETIC_OPERATOR_BC_ROOT(src[tokens[i].chr_pos]);
			uint32_t local_obj[2];
			
			chr = tokens[i].chr_pos - 1;
			GET_NEXT_OBJECT_FROM_END();
			if (IS_NUMERIC())
			{
				command |= 2;
				PREPARE_WRITE_CNSTS(memory, sizeof(double));
				*((double*)(memory->cnsts_ptr)+memory->cnsts_ptr_size) = atof(src+current_obj_start);
				local_obj[0] = memory->cnsts_ptr_size;
				memory->cnsts_ptr_size +=sizeof(double);
			}
			else
			{
				local_obj[0] = hash(src, current_obj_start, current_obj_end);
			}
			
			chr = tokens[i].chr_pos + 1;
			GET_NEXT_OBJECT_FROM_BEG();
			if (IS_NUMERIC())
			{
				command |= 1;
				PREPARE_WRITE_CNSTS(memory, sizeof(double));
				*((double*)(memory->cnsts_ptr)+memory->cnsts_ptr_size) = atof(src+current_obj_start);
				local_obj[0] = memory->cnsts_ptr_size;
				memory->cnsts_ptr_size +=sizeof(double);
			}
			else
			{
				local_obj[1] = hash(src, current_obj_start, current_obj_end);
			}
			
			PREPARE_WRITE_CODE(memory, 12);
			*(memory->code_ptr+memory->code_ptr_size) = command; //left padded
			memory->code_ptr_size+=4;
			*(memory->code_ptr+memory->code_ptr_size) = local_obj[0];
			memory->code_ptr_size+=4;
			*(memory->code_ptr+memory->code_ptr_size) = local_obj[1];
			memory->code_ptr_size+=4;
			
		}
		
	//}
}

void free_memory_allocation_internal(MEMORY_ALLOCATION* m)
{
	free(m->cnsts_ptr);
	free(m->cllbls_ptr);
	free(m->code_ptr);
}