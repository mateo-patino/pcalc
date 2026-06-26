#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

/* Token types */
typedef enum {
  OPERAND,
  NUMBER,
  LPAREN,
  RPAREN
} token_type;

/* Supported operations */
typedef enum {
    ADD,
    SUB,
    MUL,
    DIV
} operation_type;


/* Token struct; serves as a wrapper around number_t and operand_t */
typedef struct {
    token_type type;
    void *obj;
} token_t;

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

/* Operand data type */
typedef struct {
    char arity;
    char precedence;
    operation_type op;
} operand_t;

#endif

