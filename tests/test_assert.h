#ifndef TEST_ASSERT_H
#define TEST_ASSERT_H

#include "token.h"
#include "lexer.h"

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))


/* Structure of a test case */
typedef bool (*test_func_t)(void);
typedef struct {
    const char *name;
    test_func_t func;
} test_case_t;


/*
* Takes an array of 'test_count' test cases and runs each test case in an isolated process.
* It calls fork() for each test case and counts how many test cases produce crashes, if any.
*
* If 'signaled' is not NULL, it writes the number of crashed test cases there. 
*
* Returns the number of successful tests.
*/
int run_forked_tests(const test_case_t *tests, size_t test_count, int *signaled);

/* Brace initialize a test_case_t */
#define TEST(name) {#name, name}

/* Colors */
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_RESET "\x1b[0m"
#define BOLD "\x1b[1m"

/* 
* Prints information about the test that failed a true assertion.
*/
void assert_true_failed(const char *file_name, int line, const char *func, const char *expr); 

/* Check if an expression is true */
#define ASSERT_TRUE(x) \
    do { \
        if (!(x)) { \
            assert_true_failed(__FILE__, __LINE__, __func__, #x);  \
            return false; \
        } \
    } while (0) 


/*
* Prints information about the test that failed an equals int assertion.
*/
void assert_eq_int_failed(int exp, int rec, const char *file_name, int line, const char *func);

/* Check if two integer variables are equal to each other */
#define ASSERT_EQ_INT(exp, rec) \
    do { \
        if ((exp) != (rec)) { \
            assert_eq_int_failed(exp, rec, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while (0)

/*
* Prints information about the test that failed an equals value_t assertion.
*/
void assert_eq_value_t_failed(value_t exp, value_t rec, const char *file_name, int line, const char *func);


#define ASSERT_EQ_VALUE_T(exp, rec) \
    do { \
        value_t _exp = (value_t)(exp); \
        value_t _rec = (value_t)(rec); \
        if (_exp != _rec) { \
            assert_eq_value_t_failed(exp, rec, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while (0) \

/* 
* Prints information about the test that failed.
*/
void assert_eq_number_t_failed(const number_t *exp, const number_t *rec, const char *file_name, int line, const char *func);

/* Check if two number_t objects have the same value and base. */
#define ASSERT_EQ_NUMBER_T(exp, rec) \
    do { \
        const number_t _exp = (exp); \
        const number_t _rec = (rec);  \
        if (_exp.val != _rec.val || _exp.base != _rec.base) { \
            assert_eq_value_t_failed(&exp, &rec, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while (0)


void assert_expr_failed(const char *expr, value_t out, value_t result, const char *file_name, int line, const char *func);
#define ASSERT_EXPR(expr, result) \
    do { \
        value_t out = _evaluate_expression(expr); \
        if (out != result) { \
            assert_expr_failed(expr, out, result, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while (0)


void assert_tok_status_failed(tokens_status expect, tokens_status recv, const char *expr, const char *file_name, int line, const char *func);
void tokens_status_to_str(tokens_status status, bool add_newline);
#define ASSERT_TOK_STATUS(expect, recv, expr) \
    do { \
        tokens_status _expect = (tokens_status)expect; \
        tokens_status _recv = (tokens_status)recv; \
        if (_recv != _expect) { \
            assert_tok_status_failed(_expect, _recv, expr, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while (0)

#endif
