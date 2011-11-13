#include <lib/stdio.h>
#include <main.h>

static void prints(char * string, int padding);
static char * numberBaseNtoString(unsigned int number, int base, char * out);

PRIVATE int currentFd;

void setFD(int fileDescriptor) {
	currentFd = fileDescriptor;
}

void putchar(char c) {
    write(currentFd, &c, 1);
}

void printf(char * formatString, ...) {
    int integer, padding;
    unsigned int unsigenedInteger;
    char * string;
    char out[40];
    char c;
    va_list args;
    va_start(args, formatString);
    while (*formatString != '\0') {
        if (*formatString == '%') {
            formatString++;
            padding = -1;
            if (isNumber(*formatString)) {
            	padding = (*formatString - '0');
            	formatString++;
            	if (isNumber(*formatString)) {
            		padding = padding * 10 + (*(formatString) - '0');
            		formatString++;
            	}
            	log(L_DEBUG, "read a padding format: %d", padding);
            }
            switch (*formatString) {
                case 'c':
                    c = va_arg(args, int);
                    putchar(c);
                    break;
                case 's':
                    string = va_arg(args, char *);
                    prints(string, padding);
                    break;
                case 'd':
                    integer = va_arg(args, int);
                    if (integer < 0) {
                        integer = -integer;
                        putchar('-');
                    }
                    prints(numberBaseNtoString(integer, 10, out), padding);
                    break;
                case 'u':
                    unsigenedInteger = va_arg(args, unsigned int);
                    prints(numberBaseNtoString(unsigenedInteger, 10, out), padding);
                    break;
                case 'o':
                    integer = va_arg(args, unsigned int);
                    prints(numberBaseNtoString(integer, 8, out), padding);
                    break;
                case 'x':
                case 'p':
                    unsigenedInteger = va_arg(args, unsigned int);
                    prints(numberBaseNtoString(unsigenedInteger, 16, out), padding);
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

static void prints(char * string, int padding) {
    int lenght = 0;
	while (*string != '\0' && (padding == -1 || lenght++ <= padding)) {
        putchar(*string);
        string++;
    }
	while(lenght++ <= padding) {
		putchar(' ');
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
	*s = '\0';
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
