#ifndef PRINTER_H
#define PRINTER_H

#include "token.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


/* 
* Digits from binary, octal, and hex numbers are printed in groups.
* The constants below are conventional for most programmers, but feel free 
* to change them if you wish to format pretty-print in a different output.
*/
#define GROUP_BINARY_BY 4
#define GROUP_OCTAL_BY 3
#define GROUP_HEX_BY 2


/*
* Returns the highest power of 2 needed to represent 'res' in binary form.
*/
int find_highest_exponent_2(value_t res);

/*
* Prints 'res' in binary, octal, decimal, and hexadecimal base to 'stream'.
*/
void pretty_print_all_bases(FILE *stream, value_t res, bool caps);


/*
* Prints 'res' in binary format to 'stream'. 
*
* The digits of the printer number are arranged in groups of 'group_by' digits and 
* prefixed by 0b.
*/
void pretty_print_binary(FILE *stream, value_t res, int group_by);


/* Find the number of digits in the largest octal number. Recall one octal digit corresponds to 3 bits */
#define MAXLEN_OCTAL_STR (int)((CHAR_BIT * sizeof(value_t) + 2 ) / 3) /* Ceiling of n / d = ( n + d - 1 ) / d */

/*
* Prints 'res' in octal format to 'stream'.
* The digits are arranged in groups of 'group_by' digits.
*/
void pretty_print_octal(FILE *stream, value_t res, int group_by);


/*
* Prints in 'res' in decimal format using commas to separate thousands.
*/
void pretty_print_decimal(FILE *stream, value_t res);


/* 
* Pretty prints a value 'res' to 'stream' in the 'base' number system.
* If 'caps' is true, it prints the value in caps. If 'add_newline' is true,
* it prints a newline after the number.
*/
void pretty_print_value(FILE* stream, value_t res, int base, bool caps, bool add_newline);


#endif
