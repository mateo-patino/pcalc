#include "printer.h"

#include <inttypes.h>
#include <limits.h>


int get_digits_in_base(value_t res, int base, char *buf) {
    if (!buf || base < 2) {
        return 0;
    }
    value_t quotient = res;
    char remainder;
    int digits = 0;
    do {
        remainder = quotient % base;
        quotient = quotient / base;
        buf[digits++] = remainder;
    } while (quotient > 0);

    return digits;
}


int find_highest_exponent_2(value_t res) {
    for (int exp = CHAR_BIT * sizeof(value_t) - 1; exp >= 0; exp--) {
        if (res & ((value_t)1 << exp)) {
            return exp;
        }
    }
    return 0;
}


int print_group_zero_padding(FILE *stream, int digits, int group_by) {
    if (!stream) {
        return -1;
    }
    int padding = 0;
    int rem;
    if ((rem = digits % group_by) != 0) {
        padding = group_by - rem;
        for (int i = 0; i < padding; i++) {
            fputc('0', stream);
        }
    }
    return padding;
}


void pretty_print_binary(FILE *stream, value_t res, int group_by, bool add_newline) {
    if (!stream) {
        return;
    }
    fprintf(stream, "0b ");

    int highest_exponent = find_highest_exponent_2(res); 
    int digits = highest_exponent + 1;

    /* Add padding to print whole groups if needed */
    int padding = print_group_zero_padding(stream, digits, group_by);
    
    int digits_printed = padding;
    int total_digits = digits + padding;
    for (int exp = highest_exponent; exp >= 0; exp--) {
        if (res & ((value_t)1 << exp)) {
            fputc('1', stream);
        }
        else {
            fputc('0', stream);
        }
        digits_printed++;

        if (digits_printed < total_digits && digits_printed % group_by == 0) {
            fputc(' ', stream);
        }
    }
    if (add_newline) {
        fprintf(stream, "\n");
    }
}


void pretty_print_octal(FILE *stream, value_t res, int group_by, bool add_newline) {
    if (!stream) {
        return;
    }
    fprintf(stream, "0 ");

    /* Write digits in octal to buffer */
    char buf[MAXLEN_OCTAL_STR];
    int digits = get_digits_in_base(res, 8, buf);

    /* Pad to achieve whole groups */
    int padding = print_group_zero_padding(stream, digits, group_by);
    
    int digits_printed = padding;
    int total_digits = digits + padding;
    for (int i = digits - 1; i >= 0; i--) {
        fputc('0' + buf[i], stream);
        digits_printed++;

        if (digits_printed < total_digits && digits_printed % group_by == 0) {
            fputc(' ', stream);
        }
    }

    if (add_newline) {
        fprintf(stream, "\n");
    }
}


void pretty_print_decimal(FILE *stream, value_t res, bool add_newline) {
    if (!stream) {
        return;
    }
    /* Compute least-significant thousand outside the recursion (to skip comma) */ 
    value_t rem = res % 1000;
    bool is_mst = pp_decimal_helper(stream, res / 1000);

    /* If this frame's 'rem' is the most-significant thousand, don't pad */
    if (!is_mst && rem < 100) {
        int padding = rem > 9 ? 1 : 2;
        for (int i = 0; i < padding; i++) {
            fputc('0', stream);
        }
    }
    fprintf(stream, "%" PRIu64, rem); /* Least significant thousand doesn't have a comma */
    if (add_newline) {
        fprintf(stream, "\n");
    }
}


bool pp_decimal_helper(FILE *stream, value_t quotient) {
    if (!quotient) {
        /* The caller whose quotient gave zero must be the MST */
        return true;
    }
    value_t rem = quotient % 1000;
    bool is_mst = pp_decimal_helper(stream, quotient / 1000);

    /* If this frame's 'rem' is the most-significant thousand, don't pad */
    if (!is_mst && rem < 100) {
        int padding = rem > 9 ? 1 : 2;
        for (int i = 0; i < padding; i++) {
            fputc('0', stream);
        }
    }
    fprintf(stream, "%" PRIu64 ",", rem);
    return false;
}


void pretty_print_hexadecimal(FILE *stream, value_t res, int group_by, bool caps, bool add_newline) {
    if (!stream) {
        return;
    }
    fprintf(stream, "0x ");

    char buf[MAXLEN_HEX_STR];
    int digits = get_digits_in_base(res, 16, buf);

    /* Add padding if needed */
    int padding = print_group_zero_padding(stream, digits, group_by);

    int digits_printed = padding;
    int total_digits = digits + padding;
    for (int i = digits - 1; i >= 0; i--) {
        fputc(num_to_hex_digit(buf[i], caps), stream);
        digits_printed++;

        if (digits_printed < total_digits && digits_printed % group_by == 0) {
            fputc(' ', stream);
        }
    }
    if (add_newline) {
        fprintf(stream, "\n");
    }
}


char num_to_hex_digit(char num, bool caps) {
    /* num is always >= 0 due to char type*/
    if (num > 15) {
        return '?';
    }

    if (num <= 9) {
        return '0' + num;
    }
    return (caps ? 'A' : 'a') + (num - 10);
}


void raw_print_binary(FILE *stream, value_t res, bool add_newline) {
    if (!stream) {
        return;
    }
    fprintf(stream, "0b");
    int highest_exp = find_highest_exponent_2(res);
    for (int exp = highest_exp; exp >= 0; exp--) {
        if (res & ((value_t)1 << exp)) {
            fputc('1', stream);
        }
        else {
            fputc('0', stream);
        }
    }

    if (add_newline) {
        fprintf(stream, "\n");
    }
}


void raw_print_octal(FILE *stream, value_t res, bool add_newline) {
    if (!stream) {
        return;
    }
    fprintf(stream, "0");
    char buf[MAXLEN_OCTAL_STR];
    int digits = get_digits_in_base(res, 8, buf);
    for (int i = digits - 1; i >= 0; i--) {
        fputc('0' + buf[i], stream);
    }
    
    if (add_newline) {
        fprintf(stream, "\n");
    }
}


void raw_print_decimal(FILE *stream, value_t res, bool add_newline) {
    if (!stream) {
        return;
    }
    fprintf(stream, "%" PRIu64, res);
    
    if (add_newline) {
        fprintf(stream, "\n");
    }
}


void raw_print_hexadecimal(FILE *stream, value_t res, bool caps, bool add_newline) {
    if (!stream) {
        return;
    }
    fprintf(stream, "0x");
    char buf[MAXLEN_HEX_STR];
    int digits = get_digits_in_base(res, 16, buf);
    for (int i = digits - 1; i >= 0; i--) {
        fputc(num_to_hex_digit(buf[i], caps), stream);
    }

    if (add_newline) {
        fprintf(stream, "\n");
    }
}
