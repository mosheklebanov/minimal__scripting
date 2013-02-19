#include <stdlib.h>
#include <stdio.h> //debug only
#include "compiler.h"
#include "bytecode_constants.h"
#include "uthash/uthash.h"

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
} while(0)

#define GET_NEXT_OBJECT_FROM_END() do { \
	current_obj_end = chr; \
	for(current_obj_start = chr; IS_VALID_NAME_CHAR(src[current_obj_start]); current_obj_start--) {} \
} while(0)
	

//credits: http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash(char* s, unsigned len)
{
	unsigned long hashval = 0;
	unsigned c;
	for (c=0;c<len;c++)
		hashval = s[c] + (hashval<<6) + (hashval<<16) - hashval;
	return hashval;
}

/*
The main entry to the compiler
Responsible for generating bytecode from a char buffer
The out pointer will be initialized to an internal memory buffer; this will be accessible to the caller after
the function returns; must be free()-d later on to prevent memory leaks
Any compilation requires a bidirectional access to the input
*/

/*
Note: this function may be called several times during the parsing procedure,
as long as (entire) evaluates to false
*/

/*
Note: ERROR_CALLBACK will be called when an error results from the compiling procudure.
Is it guaranteed that the error_msg will be non-NULL - the text in error_msg does not need to be freed
*/
void gen_bytecode(const char* src, unsigned src_len, int entire, void** out_ptr, unsigned* out_len, ERROR_CALLBACK err)
{
	void* out_buffer = *out_ptr;
	out_buffer = malloc(ALLOC_BLOCK_SIZE);
	
	*out_len = 0;
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
			printf("Token\nchr_pos %d\ndepth %d\nletter %c\n",tokens[i].chr_pos,tokens[i].depth, src[tokens[i].chr_pos]);
			chr = tokens[i].chr_pos + 1;
			GET_NEXT_OBJECT_FROM_BEG();
			printf("Follows: %d\n\n",current_obj_end - current_obj_start);
			chr = tokens[i].chr_pos - 1;
			GET_NEXT_OBJECT_FROM_END();
			printf("Precedes: %d\n\n",current_obj_end - current_obj_start);
		}
		
	//}
}