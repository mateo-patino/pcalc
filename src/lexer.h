#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include "token.h"

/* Tokenization status codes */
typedef enum {
    TOKENS_OK,
    TOKENS_INVALID_ARG,
    TOKENS_ULL_OVERFLOW,
    TOKENS_NULL_STR,
    TOKENS_UNMATCHED_LPARENS,
    TOKENS_UNMATCHED_RPARENS,
    TOKENS_EXPECTED_OPERAND,
    TOKENS_DIV_BY_ZERO,
    TOKENS_MALLOC_FAILURE
} tokens_status;


/*
* Produces an array of token_t structures from a NULL-terminated argument vector.
* The first token_t object is written to 'addr' and 'TOKENS_OK' is returned upon success.
* 
* If argv contains an invalid argument that cannot be tokenized, a value from 
* 'tokens_status' describing the error is returned, and 'invalid' is modified to point
* to this argument. Previously heap-allocated token_t members are freed, and the token slot
* where the invalid argument occurred is filled with a token_t of type INVALID_TOKEN.
*/
tokens_status create_tokens_from_argv(char **argv, token_t *addr, char **invalid);


/*
* Produces an array of token_t structures from a string.
* The first token_t object is written to 'addr' and 'TOKENS_OK' is returned upon success.
*
* If the string contains an invalid argument that cannot be tokenized, a value from 
* 'tokens_status' describing the error is returned, and 'invalid' is modified to point
* to this argument. Previously heap-allocated token_t members are freed, and the token slot
* where the invalid argument occurred is filled with a token_t of type INVALID_TOKEN.
*
* Note that this function tokenizes the string using delimiters defined by the macro
* SPACE_CHARS
*/
#define SPACE_CHARS " \t\n\v\f\r"
tokens_status create_tokens_from_string(char *str, token_t *addr, char **invalid);


/* 
* Creates a token_t object from a string 'str'. The token is written to 'addr'.
* It returns TOKENS_OK upon success and the respective tokens_status code upon
* error.
*
* Inside the function, 'str' is normalized to lowercase so that pcalc can accept
* operators in upper and lowercase.
*/
tokens_status create_token_from_str(const char *str, token_t *addr);


/* 
* Determines the base a number in string format has. The allowed formats are:
*
* No prefix: decimal
* Prefix "0b": binary
* Prefix "0": octal
* Prefix "0x": hexadecimal
*
* Ignoring all leading whitespace, if 'str' does not conform to one of the four allowed
* prefixes above, the function returns -1. If 'str' has a valid prefix, the function
* returns an integer representing the base of the number.
*
* This function GUARANTEES that the first two non-whitespace characters in 'str' comprise
* a valid prefix. It does NOT check any characters beyond the first two, so non-numerical 
* characters that make 'str' impossible to tokenize may exist.
*/
int get_base_from_str(const char *str);


/* 
* Calls strtoull on a string representing a number of base 'base'. If 'val' is not NULL, the 
* value of the number is written there upon success.
*
* Returns true if the string is a valid number and false otherwise. 
*
* Note that the function does not check for prefixes. Hence, get_base_from_str should be
* called before this function in most cases to obtain the prefix of the string and pass the
* corresponding base into this function. 
*/
bool get_number_from_str(const char *str, const int base, value_t *val);


/*
* Returns true if 'str' contains an en-dash, em-dash, or hyphen.
*/
bool has_dash(const char *str);


/* 
* Prints an error message to stderr given a tokens_status code. 
*/
void print_token_error(tokens_status status, const char *msg);


/*
* Returns true if 'str' represents a valid (64-bit unsigned) number and false otherwise.
* If false is returned, errno will be set to ERANGE when the string is numerical but too
* large to fit in an unsigned 64-bit integer. 

If 'base' is not NULL and the integer is valid, the base of the integer is stored in 
'base' and the value in 'val'. If 'str' was invalid, neither parameter is modified.
*/
bool is_number(const char *str, int *base, value_t *val);


/*
* Returns true if 'str' is a valid operation. It compares 'str' to all the entries in 
* operation_labels table.
*
* If 'type' is not NULL, the operation type is written to it.
*/
bool is_operation(const char *str, operation_type *type);


/*
* Verifies that the 'tokens' array makes mathematical sense and can be transformed into
* an abstract syntax tree by the AST module.
*
* It uses the 'validation_funcs' table in lexer.c to run a comprenhensive set of checks.
* Each validator function is declared below, and it is of type validator_func_t.
*/
tokens_status validate_tokens_semantic(const token_t *tokens, size_t token_count);

typedef tokens_status (*validator_func_t)(const token_t *tokens, size_t token_count);

/*
* Verify all parentheses are correctly opened and closed.
*/
tokens_status validate_parens(const token_t *tokens, size_t token_count);

/*
* Verify the user doesn't attempt to write " ... / 0 "
*/
tokens_status validate_div_by_zero(const token_t *tokens, size_t token_count);


#endif
