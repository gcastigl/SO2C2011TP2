#include "stdio.h"

static void prints(char * string);

static char * numberBaseNtoString(unsigned int number, int base, char * out);

void putchar(char c) {
    __write(1, &c, 1);
}

void printf(char * formatString, ...) {
    int integer;
    unsigned int unsigenedInteger;
    char * string;
    char out[40];
    char c;

    va_list args;

    va_start(args, formatString);

    while (*formatString != '\0') {
        if (*formatString == '%') {

            formatString++;

            switch (*formatString) {
                case 'c':
                    c = va_arg(args, int);
                    putchar(c);
                    break;
                case 's':
                    string = va_arg(args, char *);
                    prints(string);
                    break;
                case 'd':
                    integer = va_arg(args, int);
                    if (integer < 0) {
                        integer = -integer;
                        putchar('-');
                    }
                    prints(numberBaseNtoString(integer, 10, out));
                    break;
                case 'u':
                    unsigenedInteger = va_arg(args, unsigned int);
                    prints(numberBaseNtoString(unsigenedInteger, 10, out));
                    break;
                case 'o':
                    integer = va_arg(args, unsigned int);
                    prints(numberBaseNtoString(integer, 8, out));
                    break;
                case 'x':
                    unsigenedInteger = va_arg(args, unsigned int);
                    prints(numberBaseNtoString(unsigenedInteger, 16, out));
                    break;
                case '%':
                    putchar('%');
                    break;
            }
        } else {
            putchar(*formatString);
        }
        formatString++;
    }
    va_end(args);
}

static void prints(char * string) {
    while (*string != '\0') {
        putchar(*string);
        string++;
    }
}

static char * numberBaseNtoString(unsigned int number, int base, char * out) {

    int digits[40];
    int position = 0;
    char * numbers = "0123456789ABCDEF";
    int index = 0;

    if (number != 0) {
        while (number > 0) {
            if (number < base) {
                digits[position] = number;
                number = 0;
            } else {
                digits[position] = number % base;
                number /= base;
            }
            position++;
        }

        for (index = 0; position > 0; position--, index++) {
            out[index] = numbers[digits[position - 1] % base];
        }
        out[index] = '\0';
    } else {
        out[0] = '0';
        out[1] = '\0';
    }

    return out;
}
