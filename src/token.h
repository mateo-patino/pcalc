#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>
#include <stdbool.h>

/* Token types */
typedef enum {
  OPERAND,
  NUMBER,
  LPAREN,
  RPAREN,
  INVALID_TOKEN
} token_type;

/* Token struct; serves as a wrapper around number_t and operand_t */
typedef struct {
    token_type type;
    void *obj;
} token_t;


/*
* Frees the 'obj' member of a token_t but NOT the token_t itself.
* Returns true if 'obj' was not NULL and was freed and false otherwise.
*/
bool free_token_obj(token_t *tok);


/*
* Calls free_token_obj on an array of token_t structs until finding the first
* token_t with type = INVALID_TOKEN. 
* 
* It returns the number of tokens freed, excluding the INVALID_TOKEN.
*/
int free_tokens_invalid(token_t *tok);


/* 
* Prints a token_t's information in a readable format to stdout.
*/
void print_token(const token_t *tok, bool add_newline);


/* Supported operations */
typedef enum {
    ADD,
    SUB,
    MUL,
    DIV,
    NUM_OP
} operation_type;

#define SYNONYMS_PER_OP 3
extern const char* const operation_labels[NUM_OP][SYNONYMS_PER_OP];
extern const char op_arity[NUM_OP];
extern const char op_precedence[NUM_OP];

/* Operand data type */
typedef struct {
    char arity;
    char precedence;
    operation_type op;
} operand_t;

/*
* Constructor by type for operand_t objects. Returns a pointer a HEAP-allocated
* operand_t and NULL if malloc fails.
*/
operand_t *init_operand(operation_type type);


/* Number data type */
typedef uint64_t value_t;
typedef struct {
    value_t value;
    int base;
} number_t;


/* 
* Parametrized constructor for number_t objects. Returns a pointer to a HEAP-allocated 
* number_t and NULL if malloc fails.
*/
number_t *init_number(value_t value, int base);


#endif

