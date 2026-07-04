#include "lexer.h"
#include "token.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


tokens_status create_tokens_from_string(char *str, token_t *addr, char **invalid) {
    tokens_status status;
    size_t i = 0;
    const char *sep = SPACE_CHARS;
    char *tok = strtok(str, sep);

    while (tok) {
        status = create_token_from_str(tok, addr + i);
        if (status != TOKENS_OK) {
            if (invalid) {
                *invalid = tok;
            }
            addr[i].type = INVALID_TOKEN;
            addr[i].obj = NULL;
            for (size_t t = 0; t < i; t++) {
                free_token_obj(addr + t);
            }
            return status;
        }
        i++;
        tok = strtok(NULL, sep);
    }

    return TOKENS_OK;
}


tokens_status create_tokens_from_argv(char **argv, token_t *addr, char **invalid) {
    char **ptr = argv;
    tokens_status status;
    size_t i = 0;

    while (*ptr) {
        status = create_token_from_str(*ptr, addr + i);
        if (status != TOKENS_OK) {
            if (invalid) {
                *invalid = *ptr;
            }
            addr[i].type = INVALID_TOKEN;
            addr[i].obj = NULL;
            for (size_t t = 0; t < i; t++) {
                free_token_obj(addr + t);
            }
            return status;
        }
        i++;
        ptr++;
    } 

    return TOKENS_OK;
}


tokens_status create_token_from_str(const char *str, token_t *addr) {
    if (str == NULL || strlen(str) == 0) {
        return TOKENS_NULL_STR;
    }
    size_t len = strlen(str);
    char s[len + 1];
    for (size_t i = 0; i <= len; i++) {
        s[i] = tolower((unsigned char)str[i]);
    }

    /* Check if 'str' is a valid number */
    int base;
    value_t val;
    if (is_number(s, &base, &val)) {
        number_t *number = init_number(val, base);
        if (!number) {
            return TOKENS_MALLOC_FAILURE;
        }
        addr->type = NUMBER;
        addr->obj = number;

        return TOKENS_OK;
    }
    else {
        /* errno could also be EINVAL ('str' is not a number) but that doesn't mean the string 
         * is invalid. The string could be a non-numeric token, hence we continue checking. */
        if (errno == ERANGE) {
            return TOKENS_ULL_OVERFLOW;
        }
    }

    /* Check if 'str' is a valid operand */
    operation_type type;
    if (is_operation(s, &type)) {
        operand_t *op = init_operand(type);
        if (!op) {
            return TOKENS_MALLOC_FAILURE;
        }
        addr->type = OPERAND;
        addr->obj = op;

        return TOKENS_OK;
    }
    
    /* Check if 'str' is a parenthesis */
    if (strcmp(s, "(") == 0) {
        addr->type = LPAREN;
        addr->obj = NULL;
        return TOKENS_OK;
    }
    else if (strcmp(s, ")") == 0) {
        addr->type = RPAREN;
        addr->obj = NULL;
        return TOKENS_OK;
    }
    
    /* At this point, 'str' is neither a number, operand, or parenthesis */
    return TOKENS_INVALID_ARG;
}


bool has_dash(const char *str) {
    if (!str || strlen(str) == 0) {
        return false;
    }
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '-' || strcmp(str, "–") == 0 || strcmp(str, "—") == 0) {
            return true;
        }
    }
    return false;
}


void print_token_error(tokens_status status, const char *msg) {
    if (!msg) {
        msg = "";   
    }
    switch (status) {
        case TOKENS_OK:
            fprintf(stdout, "Valid tokens. %s\n", msg);
            break;
        case TOKENS_INVALID_ARG:
            fprintf(stderr, "Invalid argument. %s\n", msg);
            break;
        case TOKENS_ULL_OVERFLOW:
            fprintf(stderr, "Number is too large. %s\n", msg);
            break;
        case TOKENS_NULL_STR:
            fprintf(stderr, "NULL string was unexpectedly passed. %s\n", msg);
            break;
        case TOKENS_MALLOC_FAILURE:
            fprintf(stderr, "malloc() failed. %s\n", msg);
            break;
        default:
            fprintf(stderr, "%i is not a registered status code.\n", status);
    }
}


bool is_number(const char *str, int *base, value_t *val) {
    if (!str || strlen(str) == 0) {
        return false;
    }

    char *endptr = NULL;
    unsigned long long result;

    errno = 0;
    result = strtoull(str, &endptr, 10);
    if (endptr == str) {
        /* All 3 non-decimal bases are prefixed by a 0*, so endptr != str in such cases */ 
        return false;
    }
    else if (*endptr == '\0') {
        /* A leading 0 signals an octal (not inferred by strtoull by default, which skips the leading zero) */
        if (*str == '0') {
            errno = 0;
            result = strtoull(str, &endptr, 8);
            if (errno == 0) {
                if (base) { *base = 8; }
                if (val) { *val = (value_t)result; }
                return true;
            }
            return false;
        }
        /* Decimal */
        if (errno == 0) {
            if (base) { *base = 10; }
            if (val) { *val = (value_t)result; }
            return true;
        }
        return false;    
    }
    else if ((int)(endptr - str) == 1) {
        /* Check for bases prefixed by 0b and 0x */
        errno = 0;
        if (*endptr == 'b') {
            result = strtoull(str + 2, &endptr, 2);

            if (errno == 0 && *endptr == '\0') {
                if (base) { *base = 2; }
                if (val) { *val = (value_t)result; }
                return true;
            }
            return false;
        }
        else if (*endptr == 'x') {
            result = strtoull(str + 2, &endptr, 16);

            if (errno == 0 && *endptr == '\0') {
                if (base) { *base = 16; }
                if (val) { *val = (value_t)result; }
                return true;
            }
            return false;
        }
    }
    return false;
}


bool is_operation(const char *str, operation_type *type) {
    if (!str || strlen(str) == 0) {
        return false;
    } 
    for (int i = 0; i < NUM_OP; i++) {
       for (int j = 0; j < SYNONYMS_PER_OP; j++) {
           if (strcmp(str, operation_labels[i][j]) == 0) {
               if (type) { *type = (operation_type)i; }
               return true;
           }
       }
    }
    return false;
}

