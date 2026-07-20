# BitPeek

Online calculators are clunky and don't provide a useful representation of numbers in bases other than 10. I made this program to see numbers in binary, octal, and hexadecimal easily with convenient features that online calculators do not generally support. 

It is especially designed for students learning about binary, octal, and hexadecimal and people working in systems programming.

## Usage

`bitpeek` is a command-line program that evaluates mathematical expressions and shows you the result in binary, octal, decimal, and hexadecimal.

```sh
$ ./bitpeek "10 + 20"
    Base 2:        0b11110
    Base 8:        036
    Base 10:       30
    Base 16:       0x1e
```

Bitwise operations (AND, XOR, OR, NOT, and shifts) are supported. These operations follow the precedence and associativity rules of the C programming language.

```sh
$ ./bitpeek "1 << 3"
    Base 2:        0b1000
    Base 8:        010
    Base 10:       8
    Base 16:       0x8
```
Every logical token in the expression must be surrounded by whitespaces so that "2 * ( 1 + 1 )" is a valid expression but "2 * (1 + 1)" is not. In other words, all parentheses, operators, and operands must be separated by whitespaces.

Instead of evaluation a mathematical expression, you can pass a single number into `bitpeek` to see the number represented in the different bases.

```sh
$ ./bitpeek "0xff"
    Base 2:        0b11111111
    Base 8:        0377
    Base 10:       255
    Base 16:       0xff
```

## Binary, Octal, and Hex Literals

`bitpeek` can parse expressions with numbers in any of the four supported bases. Use the `0b` prefix to write binary numbers, a leading `0` for octal numbers, and the `0x` prefix for hexadecimal numbers. For example,

```sh
$ ./bitpeek "0b1010 + 010 + 67 + 0xff"
    Base 2:        0b101110101
    Base 8:        0565
    Base 10:       373
    Base 16:       0x175
```

## Grouping

A fundamental idea in computer science is that **four binary digits represent one hexadecimal digit**. Hence, it is conventional to write binary numbers in groups of four digits. `bitpeek` lets you do this via command-line arguments. For example, to group binary digits in sets of four bits, pass `-b 4` to the program.

```sh
$ ./bitpeek -b 4 "1023"
    Base 2:        0b 0011 1111 1111
    Base 8:        01777
    Base 10:       1,023
    Base 16:       0x3ff
```

To group octal and hexadecimal digits, use the options `-o` and `-x`, respectively. Below we group hexadecimal by 2 digits and octal by 3 digits because these groupings map to 8 and 9 bits, respectively, and most programmers visualize numbers this way.

```sh
$ ./bitpeek -b 4 -x 2 -o 3 "1024 - 1"
    Base 2:        0b 0011 1111 1111
    Base 8:        0 001 777
    Base 10:       1,023
    Base 16:       0x 03 ff
```
If you struggle to count the number of digits in a number, an argument of 1 prints a whitespace between every digit as so,
```sh
$ ./bitpeek -b 1 -o 1 -x 1 "( 1 << 10 ) - 1"
    Base 2:        0b 1 1 1 1 1 1 1 1 1 1
    Base 8:        0 1 7 7 7
    Base 10:       1,023
    Base 16:       0x 3 f f
```
