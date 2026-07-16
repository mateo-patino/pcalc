#ifndef TEST_AST_H
#define TEST_AST_H

#include "token.h"
#include "lexer.h"
#include "ast.h"

#include <stdbool.h>


/* 
* Runner for the AST tests. 
*/
int run_ast_tests(int *total_ran, int *crashes);


/*
* Tokenizes an expression and returns a pointer to the array of tokens. 
*
* If 'stauts' is not NULL, the tokens_status code is written there. If 'tc' is not NULL,
* the token count is written there.
*/
token_t *_tokenize_from_expression(const char *expr, tokens_status *status, size_t *tc);

/*
* Calls tokenizer and evaluator on a mathematical expression, simulating the entire pipeline
* of bitpeek.
*
* Returns the result of evaluating the expression and -1 if an error occurs.
*/
value_t _evaluate_expression(const char *expr, ast_status *status);


/*
* Integration test: tests the tokenization, semantic validation, and AST building and evaluation
* pipeline on algebraic expressions.
*
* See _evaluate_expression to see the pipeline tested.
*/
bool test_evaluation_simple(void);
bool test_evaluation_medium(void);
bool test_evaluation_hard(void);
bool test_evaluation_harder(void);


/*
* Test parenthesis handling
*/
bool test_evaluation_parens_madness(void);


/*
* Test AST structure on various expressions.
*/
bool test_ast_structure_easy(void);
bool test_ast_structure_medium(void);
bool test_ast_structure_hard(void);
bool test_ast_structure_harder(void);
bool test_ast_structure_edge_cases(void);

/*
* Test AST error catching (overflow, underflow, etc.)
*/
bool test_ast_division_by_zero(void);

#endif
