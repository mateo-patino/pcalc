#include "token.h"
#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

const char* const operation_labels[NUM_OP][SYNONYMS_PER_OP] = {
    [OR] = {"or", "|", "bitor"},
    [XOR] = {"xor", "^", "bitxor"},
    [AND] = {"and", "&", "bitand"},
    [ADD] = {"add", "plus", "+"},
    [SUB] = {"sub", "minus", "-"},
    [MUL] = {"mul", "times", "*"},
    [DIV] = {"div", "over", "/"},
};


const char op_arity[NUM_OP] = {
    [OR] = 2,
    [XOR] = 2,
    [AND] = 2,
    [ADD] = 2,
    [SUB] = 2,
    [MUL] = 2,
    [DIV] = 2,
};

/* 
* A higher the number means the operator binds tighter than those with lower numbers. 
* The precedence levels here are identical to those specified by the C standard whenever applicable.
*/
const char op_precedence[NUM_OP] = {
    [OR] = 5,
    [XOR] = 6,
    [AND] = 7,
    [ADD] = 11,
    [SUB] = 11,
    [MUL] = 12,
    [DIV] = 12,
};


const associativity op_associativity[NUM_OP] = {
    [OR] = ASSOC_LEFT,
    [XOR] = ASSOC_LEFT,
    [AND] = ASSOC_LEFT,
    [ADD] = ASSOC_LEFT,
    [SUB] = ASSOC_LEFT,
    [MUL] = ASSOC_LEFT,
    [DIV] = ASSOC_LEFT
};


int count_tokens(const char *str) {
    if (!str) { 
        return -1;
    }
    size_t len = strlen(str);
    char s[len + 1];
    strncpy(s, str, len + 1);
    s[len] = '\0';

    const char *sep = SPACE_CHARS;
    char *tok = strtok(s, sep);
    if (!tok) {
        return 0;
    }

    int tok_count = 1;
    while ((tok = strtok(NULL, sep)) != NULL) {
        tok_count++;
    }
    return tok_count;
}


number_t *init_number(value_t value, int base) {
    number_t *obj = malloc(sizeof(number_t));
    if (!obj) {
        return NULL;
    } 
    obj->value = value;
    obj->base = base;
    
    return obj;
}


operator_t *init_operator(operation_type type) {
    operator_t *op = malloc(sizeof(operator_t));
    if (!op) {
        return NULL;
    }
    op->op = type;
    op->arity = op_arity[type];
    op->precedence = op_precedence[type];

    return op;
}


bool free_token_obj(token_t *tok) {
    if (!tok) {
        return false;
    }
    if (tok->obj) {
        free(tok->obj);
        tok->obj = NULL;
        return true;
    }
    return false;
}


int free_tokens_invalid(token_t *tok) {
    if (!tok) {
        return 0;
    }
    int count = 0;
    token_t *ptr = tok;
    while (ptr->type != INVALID_TOKEN) {
        free_token_obj(ptr);
        count++;
        ptr++;
    }
    return count;
}


void free_tokens_count(token_t *tok, size_t count) {
    if (!tok) {
        return;
    }
    for (size_t i = 0; i < count; i++) {
        free_token_obj(tok + i);
    }
}


void print_token(const token_t *tok, bool add_newline) {
    if (!tok) {
        return;
    }

    /* Operand print */
    if (tok->type == OPERATOR) {
        operator_t *oper = (operator_t *)tok->obj;
        operation_type op = oper->op;
        switch(op) {
            case OR:
                fprintf(stdout, "OR");
                break;
            case XOR:
                fprintf(stdout, "XOR");
                break;
            case AND:
                fprintf(stdout, "AND");
                break;
            case ADD:
                fprintf(stdout, "ADD");
                break;
            case SUB:
                fprintf(stdout, "SUB");
                break;
            case MUL:
                fprintf(stdout, "MUL");
                break;
            case DIV:
                fprintf(stdout, "DIV");
                break;
            case NUM_OP:
                break;
        }
    }
    /* Number print */
    else if (tok->type == NUMBER) {
        number_t *number = tok->obj;
        value_t val = number->value;
        int base = number->base;
        fprintf(stdout, "NUM(%" PRIu64 ", %i)", val, base); /* PRIu64 is a macro for the identifier of uint64 */
    }
    /* Parentheses print */
    else if (tok->type == LPAREN) {
        fprintf(stdout, "LP");
    }
    else if(tok->type == RPAREN) {
        fprintf(stdout, "RP");
    }
    /* Invalid token print */
    else if (tok->type == INVALID_TOKEN) {
        fprintf(stdout, "INVAL");
    }

    if (add_newline) {
        fprintf(stdout, "\n");
    }
}
