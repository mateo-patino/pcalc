#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include "token.h"

/* Tokenization status codes */
typedef enum {
    TOKENS_OK,
    TOKENS_UNKNOWN_ARG,
    TOKENS_ULL_OVERFLOW,
    TOKENS_NULL_STR
} tokens_status;


/*
* Produces an array of token_t structures from a NULL-terminated argument vector.
* The first token_t object is written to 'addr' and 'TOKENS_OK' is returned upon success.
* 
* If argv contains an invalid argument that cannot be tokenized, a value from 
* 'tokens_status' describing the error is returned, and 'invalid' is modified to point
* to this argument.
*/
tokens_status create_tokens_from_argv(char **argv, token_t *addr, char **invalid);


/* Creates a token_t object from a string 'str'. The token is written to 'addr'.
* It returns TOKENS_OK upon success and 
*/
tokens_status create_token_from_str(const char *str, token_t *addr);


/* 
* Prints an error message to stderr given a tokens_status code.
*/
void print_token_error(tokens_status status);


/*
* Returns true if 'str' represents a valid (64-bit unsigned) number and false otherwise.
* If false is returned, errno will be set to ERANGE when the string is numerical but too
* large to fit in an unsigned 64-bit integer. 

If 'base' is not NULL and the integer is valid, the base of the integer is stored in 
'base'. If 'str' was invalid, 'base' is not modified.
*/
bool is_number(const char *str, int *base);

#endif
