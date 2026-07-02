/* Miscellaneous helpers used in unit and integration tests */
#include "test_assert.h"
#include <stdio.h>

void assert_true_failed(const char *file_name, int line, const char *func, const char *expr) {
    fprintf(stderr, ANSI_RED BOLD "TEST FAILED" ANSI_RESET "\n");
    fprintf(stderr, "Assertion failed: %s\n", expr);
    fprintf(stderr, "   at %s:%i\n", file_name, line);
    fprintf(stderr, "   in %s\n", func);
}
