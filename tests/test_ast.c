#include "test_ast.h"
#include "test_assert.h"
#include "token.h"
#include "lexer.h"
#include "ast.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


static const test_case_t ast_tests[] = {
    TEST(test_evaluation_simple),
    TEST(test_evaluation_medium),
    TEST(test_evaluation_hard),
    TEST(test_evaluation_harder),
    TEST(test_evaluation_parens_madness)
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


