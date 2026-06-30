/*
* Entry point for pcalc.
*/
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "token.h"
#include "lexer.h"


/*
 * Returns the number of tokens in a string and -1 if the string is NULL.
 */
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

/* 
TODO: Usage and help function.
*/
void pcalc_help(void) {
    /* TODO */
    return;
}



int main(int argc, char** argv) {

    bool b_flag = false, o_flag = false, d_flag = false, x_flag = false, all_bases = true; 
    (void)all_bases; /* TO SILENCE WERROR. TODO: REMOVE */
    /*
    * To control the base of the output, use the following options:
    * -b: binary
    * -o: octal
    * -d: decimal
    * -x: hexadecimal
    * -h: help
    */

    opterr = 0;
    char c = 0;
    while ((c = getopt(argc, argv, "+bodxh")) != -1) {
        switch(c) {
            case 'b':
                b_flag = true;
                break;
            case 'o':
                o_flag = true;
                break;
            case 'd':
                d_flag = true;
                break;
            case 'x':
                x_flag = true;
                break;
            case 'h':
                pcalc_help();
                return EXIT_SUCCESS;
            case '?':
                fprintf(stderr, "Error: Unknown option '-%c' received.\n", optopt);
                return EXIT_FAILURE;
        }
    }

    if (b_flag + o_flag + d_flag + x_flag > 1) {
        fprintf(stderr, "Error: too many options received.\n");
        return EXIT_FAILURE;
    }
    else if (b_flag + o_flag + d_flag + x_flag == 1) {
        all_bases = false;
    }

    size_t token_count = 0;
    bool tokenize_argv = true;
    if (optind == argc) {
        fprintf(stderr, "Error: missing expression.\n");
        return EXIT_FAILURE;
    }
    else if (argc - optind >= 2) {
        token_count = argc - optind;
        /* tokenize_argv remains true in this case */
    }
    /* One non-option argument remaining is interpreted as an expression enclosed by quotes */
    else if (argc - optind == 1) {
        token_count = count_tokens(argv[optind]);
        tokenize_argv = false;
    }

    /* Produce array of tokens */
    token_t tokens[token_count];
    tokens_status tok_status;
    char *invalid = NULL;

    /* Valid ONLY when tokenizing from string instead of argv. Declared here to extend 'string' 
     * lifetime until print_token_error attempts to access 'invalid', which would point to 'string' */
    size_t string_len = strlen(argv[optind]); 
    char string_buf[string_len + 1]; 

    if (tokenize_argv) {
        argv += optind;
        tok_status = create_tokens_from_argv(argv, tokens, &invalid);
    }
    else {
        strncpy(string_buf, argv[optind], string_len + 1);
        string_buf[string_len] = '\0';
        tok_status = create_tokens_from_string(string_buf, tokens, &invalid);
    }

    if (tok_status != TOKENS_OK) {
        print_token_error(tok_status, invalid);
        return EXIT_FAILURE;
    }


    /* Step 1) complete! Token array ready! */ 
    
    /*
    * TODO:
    * 1) Take argv and produce a token array
    * 2) Build an AST with the token array
    * 3) Perform semantic analysis (i.e. does the math make sense, type checking and errors)
    * 4) Traverse the AST to evaluate
    * 5) Output the result
    */

    return EXIT_SUCCESS;
}
