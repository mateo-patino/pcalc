#ifndef TEST_ASSERT_H
#define TEST_ASSERT_H

/* Colors */
#define ANSI_RED "\x1b[31m"
#define ANSI_RESET "\x1b[0m"
#define BOLD "\x1b[1m"

/* 
* Prints information about the the test that failed a true assertion.
*/
void assert_true_failed(const char *file_name, int line, const char *func, const char *expr); 

#define ASSERT_TRUE(x) \
    do { \
        if (!(x)) { \
            assert_true_failed(__FILE__, __LINE__, __func__, #x);  \
            return false; \
        } \
    } while (0) 

#endif
