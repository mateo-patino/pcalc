#!/usr/bin/env bash

# This shell script tests for memory leaks in bitpeek. It must be ran in a Linux-based environment.
if ! command -v valgrind >/dev/null 2>&1; then
    cat << END >&2
Error: valgrind was not found.
Use a Linux-based machine or another kind of environment where valgrind is supported.
END
    exit 1
fi

# Check bitpeek executable exists
if [ ! -x "./bitpeek" ]; then
    echo "No 'bitpeek' executable was found. Build 'bitpeek' executable before memory checking." >&2
    exit 1
fi

# Table of expressions to feed bitpeek. These include successful and unsuccessful execution paths.
expressions=(
    "1 + 1"
    "1 + 0x1 + 01 + 0b1"
    "0b1010 & 0x0f"
    "077 | 0b10000000"
    "~ 0x0f"
    "0x40 >> 03"
    "0b1011 << 02 ^ 0x0f"
    "( 0x20 + 010 ) & ( 0b1111 * 03 )"
    "( 0xff - 0b1010 ) ^ ( 077 / 03 )"
    "not ( 0x0f | 0b10100000 ) & 0xff"
    "( 0b11110000 >> ( 01 + 01 ) ) | ( 0x3 << 03 )"
    "( ( 0x80 - 010 ) lshift 02 ) xor ( 0b110011 & 0x1f )"
    "( ( bitnot 0b1010 & 0xff ) + ( 0x20 / 04 ) )"
    "( ( 0x1234 ^ 0b101010101010 ) & ( 0777 | 0x0f0f ) ) >> 02"
    "( ( 0x400 >> 02 ) - ( 0b11 << 04 ) ) | ( ( 077 ^ 0x1f ) >> 01 )"
    "~ ( ( 0x10 + 03 ) * ( 02 + 01 ) )"
    "( ( 0x00ff00ff << 04 ) | ( 0xf000f000 >> 04 ) ) ^ ( ( 0x33333333 & 0x0f0f0f0f ) << 01 )"
    "( ( 0xaaaaaaaa and 0x0f0f0f0f ) or ( 0x55555555 bitxor 0x33333333 ) )"
    "( ( ( 0xffff0000ffff0000 rshift ( 0b10 + 01 ) ) or ( 0x0000ffff0000ffff leftshift 04 ) ) bitxor ( ( 0xaaaaaaaaaaaaaaaa bitand 0x0f0f0f0f0f0f0f0f ) rshift 01 ) )"
    "( ( ( not ( ( 0x0123456789abcdef << 04 ) ^ ( 0xfedcba9876543210 >> 04 ) ) & 0x0f0f0f0f0f0f0f0f ) | ( bitnot ( 0b1111000011110000 / 0b10 ) & 0x00ff00ff ) ) ^ ( ( 0777 * 02 ) + ( 0x3f & 0b101010 ) ) )"

    # TOKENS_INVALID_ARG
    "1 + 0b102"
    "0x1g ^ 03"
    "0xff $ 0b1"

    # TOKENS_ULL_OVERFLOW
    "18446744073709551616"
    "0x10000000000000000"
    "99999999999999999999"

    # TOKENS_UNMATCHED_LPARENS
    "( 1 + 2"
    "( ( 0xff & 0b1010 )"
    "~ ( ( 03 + 05 ) << 01"

    # TOKENS_UNMATCHED_RPARENS
    "1 + 2 )"
    "( 0xff ^ 0b1010 ) )"
    "~ 0x0f ) & 0xff"

    # TOKENS_DIV_BY_ZERO
    "1 / 0"
    "( 0xff + 01 ) / 0x0"
    "0b1010 ^ 03 / 00"

    # AST_INTEGER_OVERFLOW
    "18446744073709551615 + 1"
    "0xffffffffffffffff + 0b1"
    "0x8000000000000000 * 02"
    "( 0x7fffffffffffffff + 01 ) * 02"
    "( 0xffffffffffffffff / 02 ) * 03"

    # AST_INTEGER_UNDERFLOW
    "0 - 1"
    "0x10 - 0b10001"
    "( 02 + 03 ) - ( 0x1 + 0b101 )"
    "( 0x100 / 04 ) - ( 0b1000000 + 01 )"
    "( 0xffffffffffffffff - 0x10 ) - ( 0xffffffffffffffff - 0x0f )"

    # AST_DIV_BY_ZERO
    "1 / ( 1 - 1 )"
    "0x40 / ( ( 0b101 + 03 ) * 02 - 020 )"
    "123 / ( ( 2 + 3 ) * ( 7 - 5 ) - ( 04 + 06 ) )"
    "999 / ( ( 3 * ( 4 + 2 ) ) - ( 0x10 + 02 ) )"
    "0xffff / ( ( 0x10 >> 01 ) - 010 )"

    # AST_SHIFT_COUNT_TOO_LARGE
    "1 << 64"
    "0xffff >> 0x40"
    "0b1010 << ( 0x20 + 0x20 )"
    "( 0x1 << ( 02 * 040 ) )"
    "( 0xffff rshift ( 0b1000000 | 01 ) )"

    # AST_INVALID_EXPRESSION
    "3 +"
    "1 + +"
    "* 2"
    "1 2"
    "not"

    # Overall edge cases
    ""
    "0"
    "( ( ( 0x2a ) ) )"
    "not ~ bitnot"
    "( )"
)

# valgrind's exit code if a memory error is detected
memerr_code=67

# Total and pass counters
total_tests="${#expressions[@]}"
pass=0

# Calls valgrind on a single expression and increments the mem_pass counter accordingly.
# Takes ONE argument only: the string expression fed into bitpeek
runMemoryCheck() {
    local expr="$1"

    valgrind \
    --quiet \
    --leak-check=full \
    --error-exitcode="$memerr_code" \
    ./bitpeek "$expr" >/dev/null 2>&1
}

line="------------"
ANSI_RESET="\033[0m"
ANSI_BOLD="\033[1m"
ANSI_RED="\033[31m"
ANSI_GREEN="\033[32m"
ANSI_CYAN="\033[36m"

echo ""
printf "${ANSI_BOLD}%sMEMORY TESTS%s${ANSI_RESET}\n" "$line" "$line"

# Iterate through expression table
for expr in "${expressions[@]}"; do
    runMemoryCheck "$expr"
    status=$?
    #
    # The valgrind process called in runMemoryCheck will exit with code memerr_code if a memory error is detected (regardless of
    # bitpeek's exit status, this is the default valgrind behavior). If no memory error is detected, valgrind will exit with the 
    # status code of bitpeek, which will be 0 if bitpeek succeeded and 1 or non-zero if it failed. 
    #
    # This test suite considers a "failure" to be cases where valgrind finds a memory error. Executions of bitpeek
    # that return EXIT_FAILURE are not considered a failure by this memory-checker. Hence, we check for valgrind to 
    # return exactly memerr_code to print a failure.
    #
    if [ $status -eq $memerr_code ]; then
        printf "./bitpeek ${ANSI_BOLD}%s${ANSI_RESET} ${ANSI_RED}FAIL${ANSI_RESET}\n" "$expr" >&2
        
        # Rerun the failing test case without --quiet flag to show the Valgrind report
        echo ""
        valgrind --leak-check=full ./bitpeek "$expr"
        echo ""
    else 
        (( pass++ ))
        printf "./bitpeek ${ANSI_BOLD}%s${ANSI_RESET} ${ANSI_GREEN}PASS${ANSI_RESET}\n" "$expr"

    fi
done

# Overall summary
echo ""
printf "${ANSI_BOLD}%sSUMMARY%s${ANSI_RESET}\n" "$line" "$line"
printf "${ANSI_BOLD}Total tests: %i${ANSI_RESET}\n" "$total_tests"
printf "${ANSI_BOLD}Successful: %i${ANSI_RESET}\n" "$pass"
printf "${ANSI_BOLD}Failed: %i${ANSI_RESET}\n" $(( total_tests-pass ))
printf "${ANSI_BOLD}${ANSI_CYAN}Overall success %.0f%%${ANSI_RESET}\n" $(( 100*pass/total_tests ))
