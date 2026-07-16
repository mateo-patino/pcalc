#include "test_ast.h"
#include "test_assert.h"
#include "token.h"
#include "ast.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


static const test_case_t ast_tests[] = {
    TEST(test_evaluation_simple),
    TEST(test_evaluation_medium),
    TEST(test_evaluation_hard),
    TEST(test_evaluation_harder),
    TEST(test_evaluation_parens_madness),
    TEST(test_ast_structure_easy),
    TEST(test_ast_structure_medium),
    TEST(test_ast_structure_hard),
    TEST(test_ast_structure_harder),
    TEST(test_ast_structure_edge_cases),
    TEST(test_ast_division_by_zero),
    TEST(test_ast_overflow),
    TEST(test_ast_underflow)
};


int run_ast_tests(int *total_ran, int *crashes) {
    size_t test_count = ARRAY_LEN(ast_tests);
    int pass = run_forked_tests(ast_tests, test_count, crashes);
    if (total_ran) { *total_ran = test_count; }
    return pass;
}


bool test_evaluation_simple(void) {
    ASSERT_EXPR("1 + 1", (value_t)2);
    ASSERT_EXPR("10 - 1 ", (value_t)9);
    ASSERT_EXPR("100 / 20", (value_t)5);
    ASSERT_EXPR("2 * 2", (value_t)4);
    ASSERT_EXPR("0xff", (value_t)255);
    ASSERT_EXPR("0b10", (value_t)2);
    ASSERT_EXPR("010", (value_t)8);
    ASSERT_EXPR("00", (value_t)0);
    ASSERT_EXPR("123 + 0", (value_t)123);
    ASSERT_EXPR("123 - 0", (value_t)123);
    ASSERT_EXPR("123 * 1", (value_t)123);
    ASSERT_EXPR("123 / 1", (value_t)123);

    return true;
}


bool test_evaluation_medium(void) {
    ASSERT_EXPR("0x10 * 2 - 2", (value_t)30);
    ASSERT_EXPR("0b1010 - ( 0x1 + 01 ) ", (value_t)8);
    ASSERT_EXPR("( 0x64 - 0b110010 ) / 10", (value_t)5);
    ASSERT_EXPR("2 * 0b1000 - ( 0xff - 05 ) / 0b11001", (value_t)6);
    ASSERT_EXPR("0x20 + 0b11 * ( 010 + 2 )", (value_t)62);
    ASSERT_EXPR("( 077 - 0x1f ) * ( 0b101 + 3 )", (value_t)256);
    ASSERT_EXPR("( ( 0x40 / 0b100 ) + 012 ) * 3", (value_t)78);
    ASSERT_EXPR("0xff - ( 0b10000 + 020 ) * 5", (value_t)95);
    ASSERT_EXPR("18446744073709551615 - 1", (value_t)18446744073709551614ULL);
    ASSERT_EXPR("   2    +     3   ", (value_t)5);
    ASSERT_EXPR("  (   2 +   ( 3 * 4 ) ) ", (value_t)14);

    return true;
}


bool test_evaluation_hard(void) {
    ASSERT_EXPR("( ( 0x20 + 010 ) * ( 0b101 + 5 ) - 18 ) / 2", (value_t)191);
    ASSERT_EXPR("( 100 + ( 0x40 - 0b1000 ) * 07 ) / 4", (value_t)123);
    ASSERT_EXPR("( ( 0xff / 03 ) + ( 0b1100 * 5 ) - 15 ) / 2", (value_t)65);
    ASSERT_EXPR("( 0x80 - ( 0b1001 + 011 ) * 4 ) / 2", (value_t)28);
    ASSERT_EXPR("( ( 50 - 0x0a ) * ( 0b111 + 01 ) ) / 8", (value_t)40);
    ASSERT_EXPR("( ( 0x90 / ( 0b100 + 2 ) ) + 020 ) * 3 - 6", (value_t)114);
    ASSERT_EXPR("( ( 0b100000 + 0x10 ) / 06 ) * ( 9 - 05 )", (value_t)32);
    ASSERT_EXPR("( ( 0x7f - 077 ) + ( 0b1010 * 4 ) ) / 5", (value_t)20);
    ASSERT_EXPR("1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 + 12 + 13 + 14 + 15 + 16 + 17 + 18 + 19 + 20 + 21 + 22 + 23 + 24 + 25 + 26 + 27 + 28 + 29 + 30 + 31 + 32 + 33 + 34 + 35 + 36 + 37 + 38 + 39 + 40 + 41 + 42 + 43 + 44 + 45 + 46 + 47 + 48 + 49 + 50", (value_t)1275);

    return true;
}


bool test_evaluation_harder(void) {
    ASSERT_EXPR("100 - 50 - 25 - 10", (value_t)15);
    ASSERT_EXPR("256 / 4 / 2 / 2", (value_t)16);
    ASSERT_EXPR("100 * 10 / 5 * 2 / 4", (value_t)100);
    ASSERT_EXPR("1 + 2 * 3 - 4 / 2 + ( 5 + 6 ) * ( 7 - 8 / 4 )", (value_t)60);
    ASSERT_EXPR("( ( ( ( ( ( 1 + 2 ) * 3 ) - 4 ) / 5 ) + 6 ) * 7 )", (value_t)49);
    ASSERT_EXPR("  (  (  (  123  )  )  )  ", (value_t)123);
    ASSERT_EXPR("18446744073709551615 / 0xffffffffffffffff", (value_t)1);
    ASSERT_EXPR("0xffffffffffffffff - ( 0b1010 * 0xA ) + ( 0100 / 0x4 ) * 0", (value_t)18446744073709551515ULL);
    ASSERT_EXPR("0xff - 0b10 * 010 + ( 0x10 / 0b10 ) * 020", (value_t)367);
    ASSERT_EXPR(" 0b10101010 + 0x55 - ( 0252 + 0xAA ) / 2 ", (value_t)85);
    ASSERT_EXPR(" ( ( 0b11111111 * 0377 ) - 0xffff ) * 0 ", (value_t)0);

    return true;
}


bool test_evaluation_parens_madness(void) {
    ASSERT_EXPR("( ( ( ( ( ( 10 ) ) ) ) ) )", (value_t)10);
    ASSERT_EXPR("( ( ( ( 0x10 ) ) ) ) + ( ( ( ( ( 02 ) ) ) ) )", (value_t)18);
    ASSERT_EXPR("( ( ( ( ( 0b1010 ) ) ) ) ) * ( ( ( ( ( 03 ) ) ) ) )", (value_t)30);
    ASSERT_EXPR("( ( ( ( ( ( 100 ) ) ) ) ) ) / ( ( ( ( ( 0x04 ) ) ) ) )", (value_t)25);
    ASSERT_EXPR("( ( ( ( ( 0x20 ) ) ) ) ) + ( ( ( ( ( 0b1000 ) ) ) ) * ( ( ( ( ( 03 ) ) ) ) ) )", (value_t)56);
    ASSERT_EXPR("( ( ( ( ( ( 10 + ( ( ( ( ( 0x10 ) ) ) ) ) ) ) ) ) ) * ( ( ( ( ( 0b11 + ( ( ( ( ( 01 ) ) ) ) ) ) ) ) ) ) )", (value_t)104);
    ASSERT_EXPR("( ( ( ( ( ( ( ( 0x40 ) ) ) ) ) ) + ( ( ( ( ( 010 ) ) ) ) ) ) ) * ( ( ( ( ( ( 0b100 ) ) ) ) - ( ( ( ( ( 01 ) ) ) ) ) ) ) / ( ( ( ( ( 03 ) ) ) ) )", (value_t)72);
    ASSERT_EXPR("( ( ( ( ( ( 0x40 ) ) ) - ( ( ( ( ( ( 010 ) ) ) + ( ( ( 0b100 ) ) ) ) ) ) ) * ( ( ( ( ( ( 02 ) ) ) ) ) ) ) )", (value_t)104);
    
    return true;
}


bool test_ast_structure_easy(void) {
    ASTNode *root;

    root = _initialize_ast_from_expression("1 + 2 * 3");
    ASSERT_NODE_OP(root, ADD);
    ASSERT_NODE_NUMBER(root->left, (value_t)1);
    ASSERT_NODE_OP(root->right, MUL);
    ASSERT_NODE_NUMBER(root->right->left, (value_t)2);
    ASSERT_NODE_NUMBER(root->right->right, (value_t)3);
    /* TODO: implement full AST freer (frees token objects too) */

    root = _initialize_ast_from_expression("( 1 + 2 ) * 3");
    ASSERT_NODE_OP(root, MUL);
    ASSERT_NODE_OP(root->left, ADD);
    ASSERT_NODE_NUMBER(root->right, (value_t)3);
    ASSERT_NODE_NUMBER(root->left->left, (value_t)1);
    ASSERT_NODE_NUMBER(root->left->right, (value_t)2);

    root = _initialize_ast_from_expression("1 + 2 + 3");
    ASSERT_NODE_OP(root, ADD);
    ASSERT_NODE_OP(root->left, ADD);
    ASSERT_NODE_NUMBER(root->right, (value_t)3);
    ASSERT_NODE_NUMBER(root->left->left, (value_t)1);
    ASSERT_NODE_NUMBER(root->left->right, (value_t)2);
    
    root = _initialize_ast_from_expression("1 * 2 + 3");
    ASSERT_NODE_OP(root, ADD);
    ASSERT_NODE_OP(root->left, MUL);
    ASSERT_NODE_NUMBER(root->right, (value_t)3);
    ASSERT_NODE_NUMBER(root->left->left, (value_t)1);
    ASSERT_NODE_NUMBER(root->left->right, (value_t)2);

    root = _initialize_ast_from_expression("1 * 2 * 3");
    ASSERT_NODE_OP(root, MUL);
    ASSERT_NODE_OP(root->left, MUL);
    ASSERT_NODE_NUMBER(root->right, (value_t)3);
    ASSERT_NODE_NUMBER(root->left->left, (value_t)1);
    ASSERT_NODE_NUMBER(root->left->right, (value_t)2);

    return true;
}


bool test_ast_structure_medium(void) {
    ASTNode *root;

    root = _initialize_ast_from_expression("( 01 + 0x2 ) * 3");
    ASSERT_NODE_OP(root, MUL);
    ASSERT_NODE_NUMBER(root->right, (value_t)3);
    ASSERT_NODE_OP(root->left, ADD);
    ASSERT_NODE_NUMBER(root->left->left, (value_t)1);
    ASSERT_NODE_NUMBER(root->left->right, (value_t)2);

    root = _initialize_ast_from_expression("10 / ( 07 - 0x2 ) - 2");
    ASSERT_NODE_OP(root, SUB);
    ASSERT_NODE_NUMBER(root->right, (value_t)2);
    ASSERT_NODE_OP(root->left, DIV);
    ASSERT_NODE_NUMBER(root->left->left, (value_t)10);
    ASSERT_NODE_OP(root->left->right, SUB);
    ASSERT_NODE_NUMBER(root->left->right->left, (value_t)7);
    ASSERT_NODE_NUMBER(root->left->right->right, (value_t)2);    

    root = _initialize_ast_from_expression("( 0b101 + 6 ) / ( 0x8 - 03 )");
    ASSERT_NODE_OP(root, DIV);
    ASSERT_NODE_OP(root->left, ADD);
    ASSERT_NODE_OP(root->right, SUB);
    ASSERT_NODE_NUMBER(root->left->left, (value_t)5);
    ASSERT_NODE_NUMBER(root->left->right, (value_t)6);
    ASSERT_NODE_NUMBER(root->right->left, (value_t)8);
    ASSERT_NODE_NUMBER(root->right->right, (value_t)3);

    root = _initialize_ast_from_expression("0x10 - ( 0b100 * ( 05 + 1 ) )");
    ASSERT_NODE_OP(root, SUB);
    ASSERT_NODE_NUMBER(root->left, (value_t)16);
    ASSERT_NODE_OP(root->right, MUL);
    ASSERT_NODE_NUMBER(root->right->left, (value_t)4);
    ASSERT_NODE_OP(root->right->right, ADD);
    ASSERT_NODE_NUMBER(root->right->right->left, (value_t)5);
    ASSERT_NODE_NUMBER(root->right->right->right, (value_t)1);

    root = _initialize_ast_from_expression("( ( 10 - 0x4 ) + 0b11 ) * 02");
    ASSERT_NODE_OP(root, MUL);
    ASSERT_NODE_OP(root->left, ADD);
    ASSERT_NODE_NUMBER(root->right, (value_t)2);
    ASSERT_NODE_OP(root->left->left, SUB);
    ASSERT_NODE_NUMBER(root->left->right, (value_t)3);
    ASSERT_NODE_NUMBER(root->left->left->left, (value_t)10);
    ASSERT_NODE_NUMBER(root->left->left->right, (value_t)4);

    return true;
}


bool test_ast_structure_hard(void) {
    ASTNode *root;

    root = _initialize_ast_from_expression("( ( 10 + 2 ) * ( 0x8 - 0b11 ) ) / ( 07 - ( 1 + 1 ) )");
    ASSERT_NODE_OP(root, DIV);
    ASSERT_NODE_OP(root->left, MUL);
    ASSERT_NODE_OP(root->right, SUB);
    ASSERT_NODE_OP(root->left->left, ADD);
    ASSERT_NODE_OP(root->left->right, SUB);
    ASSERT_NODE_NUMBER(root->right->left, (value_t)7);
    ASSERT_NODE_OP(root->right->right, ADD);
    ASSERT_NODE_NUMBER(root->left->left->left, (value_t)10);
    ASSERT_NODE_NUMBER(root->left->left->right, (value_t)2);
    ASSERT_NODE_NUMBER(root->left->right->left, (value_t)8);
    ASSERT_NODE_NUMBER(root->left->right->right, (value_t)3);
    ASSERT_NODE_NUMBER(root->right->right->left, (value_t)1);
    ASSERT_NODE_NUMBER(root->right->right->right, (value_t)1);

    root = _initialize_ast_from_expression("( 20 - ( 0x4 / 02 ) ) + ( ( 0b11 + 5 ) * 2 )");
    ASSERT_NODE_OP(root, ADD);
    ASSERT_NODE_OP(root->left, SUB);
    ASSERT_NODE_OP(root->right, MUL);
    ASSERT_NODE_NUMBER(root->left->left, (value_t)20);
    ASSERT_NODE_OP(root->left->right, DIV);
    ASSERT_NODE_OP(root->right->left, ADD);
    ASSERT_NODE_NUMBER(root->right->right, (value_t)2);
    ASSERT_NODE_NUMBER(root->left->right->left, (value_t)4);
    ASSERT_NODE_NUMBER(root->left->right->right, (value_t)2);
    ASSERT_NODE_NUMBER(root->right->left->left, (value_t)3);
    ASSERT_NODE_NUMBER(root->right->left->right, (value_t)5);

    root = _initialize_ast_from_expression("( ( 0x10 / ( 2 + 2 ) ) - 1 ) * ( 0b101 + ( 010 - 3 ) )");
    ASSERT_NODE_OP(root, MUL);
    ASSERT_NODE_OP(root->left, SUB);
    ASSERT_NODE_OP(root->right, ADD);
    ASSERT_NODE_OP(root->left->left, DIV);
    ASSERT_NODE_NUMBER(root->left->right, (value_t)1);
    ASSERT_NODE_NUMBER(root->right->left, (value_t)5);
    ASSERT_NODE_OP(root->right->right, SUB);
    ASSERT_NODE_NUMBER(root->left->left->left, (value_t)16);
    ASSERT_NODE_OP(root->left->left->right, ADD);
    ASSERT_NODE_NUMBER(root->left->left->right->left, (value_t)2);
    ASSERT_NODE_NUMBER(root->left->left->right->right, (value_t)2);
    ASSERT_NODE_NUMBER(root->right->right->left, (value_t)8);
    ASSERT_NODE_NUMBER(root->right->right->right, (value_t)3);

    root = _initialize_ast_from_expression("( ( 30 / 0x5 ) + ( 0b100 * 2 ) ) - ( ( 011 - 1 ) / 3 )");
    ASSERT_NODE_OP(root, SUB);
    ASSERT_NODE_OP(root->left, ADD);
    ASSERT_NODE_OP(root->right, DIV);
    ASSERT_NODE_OP(root->left->left, DIV);
    ASSERT_NODE_OP(root->left->right, MUL);
    ASSERT_NODE_OP(root->right->left, SUB);
    ASSERT_NODE_NUMBER(root->right->right, (value_t)3);
    ASSERT_NODE_NUMBER(root->left->left->left, (value_t)30);
    ASSERT_NODE_NUMBER(root->left->left->right, (value_t)5);
    ASSERT_NODE_NUMBER(root->left->right->left, (value_t)4);
    ASSERT_NODE_NUMBER(root->left->right->right, (value_t)2);
    ASSERT_NODE_NUMBER(root->right->left->left, (value_t)9);
    ASSERT_NODE_NUMBER(root->right->left->right, (value_t)1);

    root = _initialize_ast_from_expression("( ( 100 - ( 0x20 + 0b100 ) ) / 2 ) + ( 03 * ( 6 - 1 ) )");
    ASSERT_NODE_OP(root, ADD);
    ASSERT_NODE_OP(root->left, DIV);
    ASSERT_NODE_OP(root->right, MUL);
    ASSERT_NODE_OP(root->left->left, SUB);
    ASSERT_NODE_NUMBER(root->left->right, (value_t)2);
    ASSERT_NODE_NUMBER(root->right->left, (value_t)3);
    ASSERT_NODE_OP(root->right->right, SUB);
    ASSERT_NODE_NUMBER(root->left->left->left, (value_t)100);
    ASSERT_NODE_OP(root->left->left->right, ADD);
    ASSERT_NODE_NUMBER(root->left->left->right->left, (value_t)32);
    ASSERT_NODE_NUMBER(root->left->left->right->right, (value_t)4);
    ASSERT_NODE_NUMBER(root->right->right->left, (value_t)6);
    ASSERT_NODE_NUMBER(root->right->right->right, (value_t)1);

    return true;
}


bool test_ast_structure_harder(void) {
    ASTNode *root;

    root = _initialize_ast_from_expression("( ( ( 1 + 2 ) * ( 3 - 4 ) ) / ( ( 5 + 6 ) - ( 7 * 8 ) ) ) + 9");
    ASSERT_NODE_OP(root, ADD);
    ASSERT_NODE_OP(root->left, DIV);
    ASSERT_NODE_NUMBER(root->right, (value_t)9);
    ASSERT_NODE_OP(root->left->left, MUL);
    ASSERT_NODE_OP(root->left->right, SUB);
    ASSERT_NODE_OP(root->left->left->left, ADD);
    ASSERT_NODE_OP(root->left->left->right, SUB);
    ASSERT_NODE_OP(root->left->right->left, ADD);
    ASSERT_NODE_OP(root->left->right->right, MUL);

    root = _initialize_ast_from_expression("1 + ( ( 2 * ( 3 + ( 4 * ( 5 - 6 ) ) ) ) / 7 )");
    ASSERT_NODE_OP(root, ADD);
    ASSERT_NODE_NUMBER(root->left, (value_t)1);
    ASSERT_NODE_OP(root->right, DIV);
    ASSERT_NODE_OP(root->right->left, MUL);
    ASSERT_NODE_NUMBER(root->right->right, (value_t)7);
    ASSERT_NODE_OP(root->right->left->right, ADD);
    ASSERT_NODE_OP(root->right->left->right->right, MUL);
    ASSERT_NODE_OP(root->right->left->right->right->right, SUB);    

    root = _initialize_ast_from_expression("( ( 0x10 - ( 0b10 + 01 ) ) * ( ( 20 / 4 ) + ( 3 * 2 ) ) ) - ( 8 / 2 )");
    ASSERT_NODE_OP(root, SUB);
    ASSERT_NODE_OP(root->left, MUL);
    ASSERT_NODE_OP(root->right, DIV);
    ASSERT_NODE_OP(root->left->left, SUB);
    ASSERT_NODE_OP(root->left->right, ADD);
    ASSERT_NODE_OP(root->left->left->right, ADD);
    ASSERT_NODE_OP(root->left->right->left, DIV);
    ASSERT_NODE_OP(root->left->right->right, MUL);

    root = _initialize_ast_from_expression("( ( ( 100 ) ) + ( ( ( 50 ) ) ) ) / ( ( ( 10 - 5 ) ) * ( ( 3 + 2 ) ) )");
    ASSERT_NODE_OP(root, DIV);
    ASSERT_NODE_OP(root->left, ADD);
    ASSERT_NODE_OP(root->right, MUL);
    ASSERT_NODE_NUMBER(root->left->left, (value_t)100);
    ASSERT_NODE_NUMBER(root->left->right, (value_t)50);
    ASSERT_NODE_OP(root->right->left, SUB);
    ASSERT_NODE_OP(root->right->right, ADD);

    root = _initialize_ast_from_expression("( ( 1 + ( 2 * 3 ) ) - ( ( 4 + 5 ) / ( 6 - 7 ) ) ) * ( ( 8 + 9 ) / ( 10 - 11 ) )");
    ASSERT_NODE_OP(root, MUL);
    ASSERT_NODE_OP(root->left, SUB);
    ASSERT_NODE_OP(root->right, DIV);
    ASSERT_NODE_OP(root->left->left, ADD);
    ASSERT_NODE_OP(root->left->right, DIV);
    ASSERT_NODE_OP(root->left->left->right, MUL);
    ASSERT_NODE_OP(root->left->right->left, ADD);
    ASSERT_NODE_OP(root->left->right->right, SUB);
    ASSERT_NODE_OP(root->right->left, ADD);
    ASSERT_NODE_OP(root->right->right, SUB);

    return true;
}


bool test_ast_structure_edge_cases(void) {
    ASTNode *root;

    root = _initialize_ast_from_expression("1");
    ASSERT_NODE_NUMBER(root, (value_t)1);
    ASSERT_TRUE(root->left == NULL);
    ASSERT_TRUE(root->right == NULL);

    root = _initialize_ast_from_expression("( 1 )");
    ASSERT_NODE_NUMBER(root, (value_t)1);
    ASSERT_TRUE(root->left == NULL);
    ASSERT_TRUE(root->right == NULL);

    root = _initialize_ast_from_expression("1 2");
    ASSERT_TRUE(root == NULL);

    root = _initialize_ast_from_expression("( ( ( 42 ) ) )");
    ASSERT_NODE_NUMBER(root, (value_t)42);
    ASSERT_TRUE(root->left == NULL);
    ASSERT_TRUE(root->right == NULL);

    root = _initialize_ast_from_expression("1 - ( 2 - 3 ) - 4");
    ASSERT_NODE_OP(root, SUB);
    ASSERT_NODE_OP(root->left, SUB);
    ASSERT_NODE_NUMBER(root->right, (value_t)4);
    ASSERT_NODE_NUMBER(root->left->left, (value_t)1);
    ASSERT_NODE_OP(root->left->right, SUB);
    ASSERT_NODE_NUMBER(root->left->right->left, (value_t)2);
    ASSERT_NODE_NUMBER(root->left->right->right, (value_t)3);

    root = _initialize_ast_from_expression("( 1 + 2 ) * ( 3 + 4 )");
    ASSERT_NODE_OP(root, MUL);
    ASSERT_NODE_OP(root->left, ADD);
    ASSERT_NODE_OP(root->right, ADD);
    ASSERT_NODE_NUMBER(root->left->left, (value_t)1);
    ASSERT_NODE_NUMBER(root->left->right, (value_t)2);
    ASSERT_NODE_NUMBER(root->right->left, (value_t)3);
    ASSERT_NODE_NUMBER(root->right->right, (value_t)4);

    return true;
}


bool test_ast_division_by_zero(void) {
    ast_status status;

    _evaluate_expression("1 / ( 1 - 1 )", &status);
    ASSERT_TRUE(status == AST_DIV_BY_ZERO);

    _evaluate_expression(" 1 / ( ( 0xff ) - ( 255 ) )", &status);
    ASSERT_TRUE(status == AST_DIV_BY_ZERO);

    _evaluate_expression("100 / ( ( 10 - ( 2 * 3 ) ) - 4 )", &status);
    ASSERT_TRUE(status == AST_DIV_BY_ZERO);

    _evaluate_expression("0x40 / ( ( 0b101 + 03 ) * 2 - 020 )", &status);
    ASSERT_TRUE(status == AST_DIV_BY_ZERO);

    _evaluate_expression("123 / ( ( 2 + 3 ) * ( 7 - 5 ) - ( 04 + 06 ) )", &status);
    ASSERT_TRUE(status == AST_DIV_BY_ZERO);

    _evaluate_expression("999 / ( ( 3 * ( 4 + 2 ) ) - ( 0x10 + 02 ) )", &status);
    ASSERT_TRUE(status == AST_DIV_BY_ZERO);

    _evaluate_expression("7 / ( ( 0b1000 / 2 ) - ( 03 + 01 ) )", &status);
    ASSERT_TRUE(status == AST_DIV_BY_ZERO);

    _evaluate_expression("42 / ( ( ( 2 + 3 ) * ( 4 + 1 ) ) - ( 0x10 + 0b1001 ) )", &status);
    ASSERT_TRUE(status == AST_DIV_BY_ZERO);

    _evaluate_expression("88 / ( ( 0x30 - 0b10000 ) / 04 - ( 02 * 04 ) )", &status);
    ASSERT_TRUE(status == AST_DIV_BY_ZERO);

    _evaluate_expression("123 / ( ( 010 + ( 0x10 - 0b100 ) ) - ( 0b101 * 4 ) )", &status);
    ASSERT_TRUE(status == AST_DIV_BY_ZERO);

    return true;
}


bool test_ast_overflow(void) {
    ast_status status;

    _evaluate_expression("18446744073709551615 + 1", &status);
    ASSERT_TRUE(status == AST_INTEGER_OVERFLOW);

    _evaluate_expression("0xffffffffffffffff + 0b1", &status);
    ASSERT_TRUE(status == AST_INTEGER_OVERFLOW);

    _evaluate_expression("( ( 0xffffffffffffffff - 0x10 ) + 0x11 ) + 1", &status);
    ASSERT_TRUE(status == AST_INTEGER_OVERFLOW);

    _evaluate_expression("2 * ( 0x7fffffffffffffff + 0x1 )", &status);
    ASSERT_TRUE(status == AST_INTEGER_OVERFLOW);

    _evaluate_expression("( ( 0x4000000000000000 + 0x4000000000000000 ) * ( 0b10 + 0b0 ) )", &status);
    ASSERT_TRUE(status == AST_INTEGER_OVERFLOW);

    _evaluate_expression("( ( 0xffffffffffffffff / 0x10 ) * 0x10 ) + 0x10", &status);
    ASSERT_TRUE(status == AST_INTEGER_OVERFLOW);

    _evaluate_expression("( ( 0x7fffffffffffffff * 0b10 ) + 0b1 ) + ( 0x10 - 0x0f )", &status);
    ASSERT_TRUE(status == AST_INTEGER_OVERFLOW);

    _evaluate_expression("0xffffffffffffffff + ( ( 03 * 05 ) - 0x0e )", &status);
    ASSERT_TRUE(status == AST_INTEGER_OVERFLOW);

    return true;
}


bool test_ast_underflow(void) {
    ast_status status;

    _evaluate_expression("1 - 2", &status);
    ASSERT_TRUE(status == AST_INTEGER_UNDERFLOW);

    _evaluate_expression("0x10 - 0b10001", &status);
    ASSERT_TRUE(status == AST_INTEGER_UNDERFLOW);

    _evaluate_expression("( ( 0x20 - 0x10 ) - ( ( 03 * 04 ) + 05 ) )", &status);
    ASSERT_TRUE(status == AST_INTEGER_UNDERFLOW);

    _evaluate_expression("( ( 02 + 03 ) - ( 0x1 + 0b101 ) )", &status);
    ASSERT_TRUE(status == AST_INTEGER_UNDERFLOW);

    _evaluate_expression("( 0x100 / 04 ) - ( 0b1000000 + 1 )", &status);
    ASSERT_TRUE(status == AST_INTEGER_UNDERFLOW);

    _evaluate_expression("( 0xffffffffffffffff - 0x10 ) - ( 0xffffffffffffffff - 0x0f )", &status);
    ASSERT_TRUE(status == AST_INTEGER_UNDERFLOW);

    _evaluate_expression("( ( 0x10 + 0b10 ) * 02 ) - ( ( 03 + 05 ) * 05 )", &status);
    ASSERT_TRUE(status == AST_INTEGER_UNDERFLOW);

    _evaluate_expression("( ( 010 - 03 ) + ( 02 * 04 ) ) - ( 0x10 + 0b1 )", &status);
    ASSERT_TRUE(status == AST_INTEGER_UNDERFLOW);

    return true;
}
