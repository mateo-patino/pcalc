#!/usr/bin/env bash

# This shell script tests for memory leaks in bitpeek. It must be ran in a Linux-based environment.
if ! command -v valgrind >/dev/null 2>&1; then
    cat << end >&2
Error: valgrind was not found.
Use a Linux-based machine or another kind of environment where valgrind is supported.
end
    exit 1
fi

# Check bitpeek executable exists
if [ ! -f "bitpeek" ]; then
    echo "No 'bitpeek' executable was found. Build 'bitpeek' executable before memory checking." >&2
    exit 1
fi

# Table of expressions to feed bitpeek
expressions=(
    "1 + 1",
    "1 + 0x1 + 01 + 0b1"
)

# Total and pass counters
total_tests="${#expressions[@]}"
pass=0

# Calls valgrind on a single expression and increments the mem_pass counter accordingly.
# Takes ONE argument only: the string expression fed into bitpeek
runMemoryCheck() {
    local expr="$1"
    success=1

    if valgrind \
        --quiet \
        --leak-check=full \
        --error-exitcode=1 \
        ./bitpeek "$expr"
    then
        ((pass++))
        success=0 # Global variable checked by the caller
    fi
}

line="------------"
ANSI_RESET="\033[0m"
ANSI_BOLD="\033[1m"
ANSI_RED="\033[31m"
ANSI_GREEN="\033[32m"
ANSI_CYAN="\x1b[36m"

echo ""
printf "${ANSI_BOLD}%sMEMORY TESTS%s${ANSI_RESET}\n" "$line" "$line"

# Iterate through expression table
for expr in "${expressions[@]}"; do
    runMemoryCheck "expr"
    if $success
    then
        printf "./bitpeek ${ANSI_BOLD}%s${ANSI_RESET} ${ANSI_GREEN}PASS${ANSI_RESET}\n" "$expr"
    else 
        printf "./bitpeek ${ANSI_BOLD}%s${ANSI_RESET} ${ANSI_RED}FAIL${ANSI_RESET}\n" "$expr" >&2
    fi
done

# Overall summary
echo ""
printf "${ANSI_BOLD}%sSUMMARY%s${ANSI_RESET}\n" "$line" "$line"
printf "${ANSI_BOLD}Total tests: %i${ANSI_RESET}\n" "$total_tests"
printf "${ANSI_BOLD}Successful: %i${ANSI_RESET}\n" "$pass"
printf "${ANSI_BOLD}Failed: %i${ANSI_RESET}\n" $(( total_tests-pass ))
printf "${ANSI_BOLD}${ANSI_CYAN}Overall success %.0f%%${ANSI_RESET}\n" $(( pass / total_tests ))
