/* Miscellaneous helpers used in unit and integration tests */
#include <stdio.h>
#include <inttypes.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#include "test_assert.h"
#include "token.h"
#include "ast.h"


int run_forked_tests(const test_case_t *tests, size_t test_count, int *signaled) {
   int pass = 0;
   int crashes = 0;
   pid_t test_pid;

   for (size_t i = 0; i < test_count; i++) {

        if ((test_pid = fork()) == -1) {
            fprintf(stderr, "fork() failed. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        /* Child process running the test */
        else if (test_pid == 0) {
           if (tests[i].func()) {
               _exit(EXIT_SUCCESS);
           }
           _exit(EXIT_FAILURE);
        }
        /* Runner process (parent) */
        int status;
        pid_t terminated_pid = waitpid(test_pid, &status, 0);
        while (terminated_pid == -1 && errno == EINTR) {
            terminated_pid = waitpid(test_pid, &status, 0);
        }

        if (terminated_pid == -1) {
            fprintf(stderr, "waitpid() failed. %s\n", strerror(errno));
            fprintf(stderr, "Could not read result of %s\n", tests[i].name);
            continue;
        }

        /* Check for normal exit */
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code == EXIT_SUCCESS) {
                fprintf(stdout, BOLD "%s " ANSI_RESET ANSI_GREEN "PASS" ANSI_RESET "\n", tests[i].name);
                fflush(stdout);
                pass++;
            }
            /* If the test fails, the assert statements call a function that handles the printing of the error message.
            * No printing is needed here in the runner. */
            continue;
        }
        /* Check for termination by a signal */
        else if (WIFSIGNALED(status)) {
            /* TODO: implement a printer for the signal number */
            fprintf(stderr, BOLD "%s " ANSI_RED "CRASHED (signal %i)" ANSI_RESET "\n", tests[i].name, WTERMSIG(status)); 
            crashes++;
        }
   }

   if (signaled) { *signaled = crashes; }

   return pass;
}


token_t *_tokenize_from_expression(const char *expr, tokens_status *status, size_t *tc) {
    if (!expr) { return NULL; }

    size_t len = strlen(expr);
    char str[len + 1];
    strncpy(str, expr, len + 1);
    str[len] = '\0';

    size_t token_count = count_tokens(expr);
    token_t *tokens = malloc(token_count * sizeof(token_t));
    tokens_status tok_status;
    if ((tok_status = create_tokens_from_string(str, tokens, NULL)) != TOKENS_OK) {
        if (status) { *status = tok_status; }
        free_tokens_invalid(tokens);
        return NULL;
    }

    if ((tok_status = validate_tokens_semantic(tokens, token_count)) != TOKENS_OK) {
        if (status) { *status = tok_status; }
        free_tokens_count(tokens, token_count);
        return NULL;
    }

    if (tc) { *tc = token_count; }

    return tokens;
}


value_t _evaluate_expression(const char *expr) { 
    size_t token_count = 0;
    tokens_status tok_status;
    /* Note these tokens are malloc'd */
    token_t *tokens = _tokenize_from_expression(expr, &tok_status, &token_count);
    
    /* Parse the token array to build an Abstract Syntax Tree (AST) */
    ast_status eval_status;
    AST ast;
    ast.root = create_ast_from_tokens(tokens, token_count, NULL); 
    value_t out = evaluate_ast(&ast, &eval_status);

    free_tokens_count(tokens, token_count);
    free(tokens);
    free_ast(&ast);
    return out;
}


void assert_true_failed(const char *file_name, int line, const char *func, const char *expr) {
    fprintf(stderr, BOLD "%s " ANSI_RED "FAILED" ANSI_RESET "\n", func);
    fprintf(stderr, "   Assertion failed: %s\n", expr);
    fprintf(stderr, "   at %s:%i\n", file_name, line);
    fprintf(stderr, "   in %s\n", func);
}


void assert_eq_int_failed(int exp, int rec, const char *file_name, int line, const char* func) {
    fprintf(stderr, BOLD "%s " ANSI_RED "FAILED" ANSI_RESET "\n", func);
    fprintf(stderr, "   Expected: %i Received: " BOLD "%i" ANSI_RESET "\n",  exp, rec);
    fprintf(stderr, "   at %s:%i\n", file_name, line);
    fprintf(stderr, "   in %s\n", func);
}


void assert_eq_number_t_failed(const number_t *exp, const number_t *rec, const char *file_name, int line, const char *func) {
    fprintf(stderr, BOLD "%s " ANSI_RED "FAILED" ANSI_RESET "\n", func);
    fprintf(stderr, "   Expected: number_t(val=%" PRIu64 ", base=%i)\n", exp->value, exp->base);
    fprintf(stderr, "   Received:" BOLD "number_t(val=%" PRIu64 ", base=%i)" ANSI_RESET "\n", rec->value, rec->base);
    fprintf(stderr, "   at %s:%i\n", file_name, line);
    fprintf(stderr, "   in %s\n", func);
}


void assert_eq_value_t_failed(value_t exp, value_t rec, const char *file_name, int line, const char *func) {
    fprintf(stderr, BOLD "%s " ANSI_RED "FAILED" ANSI_RESET "\n", func);
    fprintf(stderr, "   Expected: %" PRIu64 " Received: " BOLD "%" PRIu64 ANSI_RESET "\n",  exp, rec);
    fprintf(stderr, "   at %s:%i\n", file_name, line);
    fprintf(stderr, "   in %s\n", func);
}


void assert_expr_failed(const char *expr, value_t out, value_t result, const char *file_name, int line, const char *func){
    fprintf(stderr, BOLD "%s " ANSI_RED "FAILED" ANSI_RESET "\n", func);
    fprintf(stderr, "   %s\n", expr);
    fprintf(stderr, "   Expected: %" PRIu64 " Received: " BOLD "%" PRIu64 ANSI_RESET "\n",  result, out);
    fprintf(stderr, "   at %s:%i\n", file_name, line);
    fprintf(stderr, "   in %s\n", func);
}


void tokens_status_to_str(tokens_status status, bool add_newline) {
    switch (status) {
        case TOKENS_OK:
            fprintf(stderr, "TOKENS_OK");
            break;
        case TOKENS_INVALID_ARG:
            fprintf(stderr, "TOKENS_INVALID_ARG");
            break;
        case TOKENS_ULL_OVERFLOW:
            fprintf(stderr, "TOKENS_ULL_OVERFLOW");
            break;
        case TOKENS_NULL_STR:
            fprintf(stderr, "TOKENS_NULL_STR");
            break;
        case TOKENS_UNMATCHED_LPARENS:
            fprintf(stderr, "TOKENS_UNMATCHED_LPARENS");
            break; 
        case TOKENS_UNMATCHED_RPARENS:
            fprintf(stderr, "TOKENS_UNMATCHED_RPARENS");
            break;
        case TOKENS_EXPECTED_OPERAND:
            fprintf(stderr, "TOKENS_EXPECTED_OPERAND");
            break;
        case TOKENS_DIV_BY_ZERO:
            fprintf(stderr, "TOKENS_DIV_BY_ZERO");
            break;
        case TOKENS_MALLOC_FAILURE:
            fprintf(stderr, "TOKENS_MALLOC_FAILURE");
            break;
        default:
            fprintf(stderr, "unknown tokens status");
            break;
    }
    if (add_newline) { 
        fprintf(stderr, "\n"); 
    }
}


void assert_tok_status_failed(tokens_status expect, tokens_status recv, const char *expr, const char *file_name, int line, const char *func) {
    fprintf(stderr, BOLD "%s " ANSI_RED "FAILED" ANSI_RESET "\n", func);
    fprintf(stderr, "   %s\n", expr);
    fprintf(stderr, "   Expected: "); 
    tokens_status_to_str(expect, false);
    fprintf(stderr, " Received: ");
    tokens_status_to_str(recv, true);
    fprintf(stderr, "   at %s:%i\n", file_name, line);
    fprintf(stderr, "   in %s\n", func);
}
