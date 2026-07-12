#include "printer.h"

#include <inttypes.h>
#include <limits.h>


int find_highest_exponent_2(value_t res) {
    for (int exp = CHAR_BIT * sizeof(value_t) - 1; exp >= 0; exp--) {
        if (res & ((value_t)1 << exp)) {
            return exp;
        }
    }
    return 0;
}


void pretty_print_value(FILE *stream, value_t res, int base, bool caps, bool add_newline) {
    if (!stream) {
        return;
    }
    switch(base) {
        case 2:
            pretty_print_binary(stream, res, GROUP_BINARY_BY);
            break;
        case 8:
            pretty_print_octal(stream, res, GROUP_OCTAL_BY);
            break;
        case 10:
            fprintf(stream, "%" PRIu64, res);
            break;
        case 16:
            if (caps) {
                fprintf(stream, "0x%" PRIX64, res);
            }
            else {
                fprintf(stream, "0x%" PRIx64, res);
            }
            break;
        default:
            break;
    }

    if (add_newline) {
        fprintf(stream, "\n");
    }
}

void pretty_print_all_bases(FILE *stream, value_t res, bool caps) {
    if (!stream) {
        return;
    }
    fprintf(stream, "Binary:      ");
    pretty_print_value(stream, res, 2, caps, true);

    fprintf(stream, "Octal:       ");
    pretty_print_value(stream, res, 8, caps, true);

    fprintf(stream, "Decimal:     ");
    pretty_print_value(stream, res, 10, caps, true);

    fprintf(stream, "Hexadecimal: ");
    pretty_print_value(stream, res, 16, caps, true);
}


void pretty_print_binary(FILE *stream, value_t res, int group_by) {
    if (!stream) {
        return;
    }
    fprintf(stream, "0b ");

    int highest_exponent = find_highest_exponent_2(res); 
    int digits = highest_exponent + 1;
    int digits_printed = 0;

    /* Add padding to print whole groups if needed */
    int rem;
    if ((rem = digits % group_by) != 0) {
        int zero_padding = group_by - rem;
        for (int i = 0; i < zero_padding; i++) {
            fputc('0', stream);
            digits_printed++;
        }
    }
    
    for (int exp = highest_exponent; exp >= 0; exp--) {
        if (res & ((value_t)1 << exp)) {
            fputc('1', stream);
        }
        else {
            fputc('0', stream);
        }

        digits_printed++;
        if (digits_printed < digits && digits_printed % group_by == 0) {
            fputc(' ', stream);
        }
    }
}


void pretty_print_octal(FILE *stream, value_t res, int group_by) {
    if (!stream) {
        return;
    }
    fprintf(stream, "0 ");

    char buf[MAXLEN_OCTAL_STR];
    value_t quotient = res;
    int remainder;
    int digits = 0;

    /* Compute octal digits via division by 8 and recording remainders */
    do {
        remainder = quotient % 8;
        quotient = quotient / 8;
        buf[MAXLEN_OCTAL_STR - 1 - digits] = remainder;
        digits++;
    } while (quotient > 0);

    /* Pad to achieve whole groups */
    int digits_printed = 0;
    int rem;
    if ((rem = digits % group_by) != 0) {
        int padding = group_by - rem;
        for (int i = 0; i < padding; i++) {
            fputc('0', stream);
            digits_printed++;
        }
    }

    for (int i = MAXLEN_OCTAL_STR - digits; i < MAXLEN_OCTAL_STR; i++) {
        fputc('0' + buf[i], stream);
        digits_printed++;

        if (digits_printed < digits && digits_printed % group_by == 0) {
            fputc(' ', stream);
        }
    }

}

