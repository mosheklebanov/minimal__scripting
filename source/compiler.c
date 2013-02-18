#include "compiler.h"

/*
Token definition and typedef
Tokens will be gathered for each statement, then evaluated in descending order of depth
*/
#define TOKEN_QUANTITY_PER_STMT 24
typedef struct token_t /* by reference */
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

/*
SPECIAL MACROS
Reponsible for parsing input - inlined for speed and easy access to variables in local scope
Simplifies access to the parsing mechanism

The following variables MUST be avaliable in the local scope of the calling function:
	int (*cb)(void*, unsigned);
	void (*err)(unsigned, unsigned, const char*);
	unsigned chr;
	unsigned line;
	unsigned traversal_depth;
	unsigned token_quantity;
	TOKEN tokens[n];
	
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
		if (tokens[i].depth<tokens[i+1].depth) \
		{ \
			t = tokens[i]; \
			tokens[i] = tokens[i+1]; \
			tokens[i+1] = t; \
		} \
	} \
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
Responsible for generating bytecode from a buffer
Will call the CALLBACK function with necessary bytes to be written to file, 
executed directly, etc..
Requires a bidirectional access to the input
*/

/*
Note: this function may be called several times during the parsing procedure,
as long as (entire) evaluates to false
*/
void gen_bytecode(const char* src, unsigned src_len, int entire, CALLBACK cb, ERROR err)
{
	unsigned chr = 0;
	unsigned line = 1;
	unsigned traversal_depth = 0;
	unsigned token_quantity = 0;
	TOKEN tokens[TOKEN_QUANTITY_PER_STMT];
	while (1)
	{
		MOVE_NEXT_LINE();
		GATHER_TOKENS();
		SORT_TOKENS();
	}
}