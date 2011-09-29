#include <lib/stdio.h>

static void prints(char * string);

static char * numberBaseNtoString(unsigned int number, int base, char * out);

static int currentFd;

void setFD(int fileDescriptor) {
	currentFd = fileDescriptor;
}

void putchar(char c) {
    __write(currentFd, &c, 1);
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

static char* numberBaseNtoString(unsigned int number, int base, char * out) {
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

int sprintf(char* s, const char *template, ...) {
	int len, read = 0;
	char * string;
	char c;
	va_list args;
	va_start(args, template);
	while (*template != '\0') {
		if (*template == '%') {
			template++;
			switch (*template) {
				case 'c':
					c = va_arg(args, int);
					*s = c;
					s++;
					read++;
					break;
				case 's':
					string = va_arg(args, char *);
					len = strlen(string);
					memcpy(s, string, len);
					s += len;
					read++;
					break;
				case 'd':
					string = itoa(va_arg(args, int));
					int len = strlen(string);
					memcpy(s, string, len);
					s += len;
					read++;
					break;
			}
		} else {
			*s = *template;
			s++;
		}
		template++;
	}
	va_end(args);
	return read;
}

#define INT_DIGITS 19		/* enough for 64 bit integer */

char* itoa(int i) {
  static char buf[INT_DIGITS + 2];	/*INT_DIGITS digits, - and '\0' */
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}
