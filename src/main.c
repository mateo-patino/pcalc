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
#include <inttypes.h>

#include "token.h"
#include "lexer.h"
#include "ast.h"
#include "printer.h"


/* 
TODO: Usage and help function.
*/
void pcalc_help(void) {
    /* TODO */
    return;
}

/* 
* Prints 'n' token_t structs from an array. Used for debugging and development only.
*/
void show_token_array(const token_t *tok, int count) {
    fprintf(stdout, "[");
    for (int i = 0; i < count - 1; i++) {
        print_token(tok + i, false);
        fprintf(stdout, ", ");
    }
    print_token(tok + count - 1, false);
    fprintf(stdout, "]\n");
}


int main(int argc, char** argv) {

    bool b_flag = false, o_flag = false, d_flag = false, x_flag = false, all_bases = true; 
    bool print_caps = false; 

    /*
    * To print the output in machine-friendly format, use the following options:
    *
    * -b: binary
    * -o: octal
    * -d: decimal
    * -x: hexadecimal
    * -h: help
    *
    * If you do not pass any flags, a human-friendly format and all bases are printed.
    *
    * To print hexadecimal digits in uppercase, pass the -c flag.
    */

    opterr = 0;
    int c = 0;
    while ((c = getopt(argc, argv, "+bodxch")) != -1) {
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
            case 'c':
                print_caps = true;
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
        fprintf(stderr, "Error: too many options received. Remove all flags for full report.\n");
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

    /* Valid ONLY when tokenizing from string instead of argv. Declared here to extend 'string_buf' 
     * lifetime until print_token_error attempts to access 'invalid', which would point to 'string_buf' */
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
        free_tokens_invalid(tokens); 
        return EXIT_FAILURE;
    }


    /* Ensure the tokens form a valid mathematical expression that can be parsed by the AST module */
    if ((tok_status = validate_tokens_semantic(tokens, token_count)) != TOKENS_OK) {
        /* TOKENS_EXPECTED_OPERAND error message is printed inside validators to inform user of which operator
        * is missing an operand, hence we skip printing out here if that error is returned */
        if (tok_status != TOKENS_EXPECTED_OPERAND) {
            print_token_error(tok_status, NULL);
        }
        free_tokens_count(tokens, token_count);
        return EXIT_FAILURE;
    }

    /* Exit status for main */
    int retval = EXIT_SUCCESS;

    /* Parse the token array to build an Abstract Syntax Tree (AST) */
    ast_status ast_create_status = AST_OK;
    AST ast;
    ast.root = create_ast_from_tokens(tokens, token_count, &ast_create_status); 
    if (!ast.root || ast_create_status != AST_OK) {
        print_ast_error(ast_create_status, NULL);
        retval = EXIT_FAILURE;
        goto FREE_AND_EXIT;
    }

    /* Evaluate the AST */
    ast_status ast_eval_status = AST_OK;
    value_t out = evaluate_ast(&ast, &ast_eval_status);
    if (ast_eval_status != AST_OK) {
        print_ast_error(ast_eval_status, NULL);
        retval = EXIT_FAILURE;
        goto FREE_AND_EXIT;
    }

    /* Pretty print according to flags */
    if (all_bases) {
        pretty_print_all_bases(stdout, out, print_caps);
    }
    else if (b_flag) {
        raw_print_binary(stdout, out, true); 
    }
    else if (o_flag) {
        raw_print_octal(stdout, out, true);
    }
    else if (d_flag) {
        raw_print_decimal(stdout, out, true);
    }
    else if (x_flag) {
        raw_print_hexadecimal(stdout, out, print_caps, true);
    }
    
FREE_AND_EXIT:
    free_ast(&ast);
    free_tokens_count(tokens, token_count);
    return retval;
}
