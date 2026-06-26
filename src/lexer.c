#include "lexer.h"
#include "token.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



tokens_status create_tokens_from_argv(char **argv, token_t *addr, char **invalid) {
    char **ptr = argv;
    tokens_status status;
    size_t i = 0;

    while (*ptr != NULL) {
        status = create_token_from_str(*ptr, addr + i);
        /* TODO */

    } 
}


tokens_status create_token_from_str(const char *str, token_t *addr) {
    if (str == NULL || strlen(str) == 0) {
        return TOKENS_NULL_STR;
    }

    /* Check if 'str' is a valid number */
    int base;
    if (is_number(str, &base)) {
        /* TODO */
    }
    else {
        /* errno could also be EINVAL ('str' is not a number) but that doesn't mean the string 
         * is invalid. The string could be a non-numeric token, hence we continue checking.
         */
        if (errno == ERANGE) {
            return TOKENS_ULL_OVERFLOW;
        }
    }

  /* TODO */
    return TOKENS_OK;

}


bool is_number(const char *str, int *base) {
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
                if (base) {
                    *base = 8;
                }
                return true;
            }
            return false;
        }
        /* Decimal */
        if (errno == 0) {
            if (base) {
                *base = 10;
            }
            return true;
        }
        return false;    
    }
    else if ((int)(endptr - str) == 1) {
        /* Check for bases prefixed by 0b and 0x */
        errno = 0;
        if (*endptr == 'b') {
            result = strtoull(str + 2, &endptr, 2);
        }
        else if (*endptr == 'x') {
            result = strtoull(str + 2, &endptr, 16);
        }
        return true ? errno != ERANGE && *endptr == '\0' : false; 
    }
    return false;
}
