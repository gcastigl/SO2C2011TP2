#include <lib/stdio.h>
#include <lib/unistd.h>
#include <lib/string.h>
#include <lib/math.h>
#include <main.h>

char getchar() {
	return getc(STD_IN);
}

char getc(int fd) {
	char c;
	while(!tty_hasInput(tty_getCurrentTTY())) {
		// WAIT...
	}
	read(fd, &c, 1);
	return c;
}

int sscanf(char *stream, char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int i = 0;
    int j = 0;
    int converted = false;

    int *integer, iTmp, iTmp2;
    char* string;
    char *chr;
    unsigned int *uinteger;

    while (format[i]) {
        if (format[i] == '%') {
            i++;
            switch (format[i++]) {
                case 'c':
                    chr = va_arg(ap, char*);
                    *chr = stream[j++];
                    break;
                case 'd':
                    integer = va_arg(ap, int *);
                    iTmp = 0;
                    iTmp2 = 1;
                    if (stream[j] == '-') {
                        iTmp2 = -1;
                        j++;
                    }
                    while (isNumber(stream[j])) {
                        iTmp = iTmp * 10 + (stream[j] - '0');
                        j++;
                    }
                    *integer = iTmp*iTmp2;
                case 'u':
                    uinteger = va_arg(ap, unsigned int *);
                    iTmp = 0;
                    while (isNumber(stream[j])) {
                        iTmp = iTmp * 10 + (stream[j] - '0');
                        j++;
                    }
                    *uinteger = iTmp;
                    break;
                case 's':
                    string = va_arg(ap, char *);
                    iTmp = 0;
                    while (stream[j] != ' ') {
                        string[iTmp++] = stream[j++];
                    }
                    string[iTmp] = '\0';
                    break;
                default:
                    // WRONG %X
                    return converted;
            }
        } else {
            if (format[i] == stream[j]) {
                i++;
                j++;
            } else {
                //WRONG FORMAT STRING
                return converted;
            }
        }
    }
    return true;
}

int scanf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	int i = 0, parsed = 0;
	int insidePercentage = false;
	char* c;
	while(format[i] != '\0') {
		//IGNORING ALL BUT %
		if (insidePercentage) {
			switch(format[i]) {
				case 's':
					parsed += gets(va_arg(args, char *));
					break;
				case 'd':
					parsed += getd(va_arg(args, int *));
					break;
				case 'c':
					c = va_arg(args, char *);
					*c = getchar();
					parsed++;
					break;
				case 'f':
					parsed += getf(va_arg(args, double *));
					break;
			}
			insidePercentage = false;
		}

		if (format[i] == '%') {
			insidePercentage = true;
		}
		i++;
	}
	return parsed;
}

int isDigit(char c) {
	return (0 <= c - '0') && (c - '0' <= 9);
}

int isNumber(char c) {
    return (c >= '0' && c <= '9');
}

char toLower(char c) {
	char diff = 'a' - 'A';
	return ((c - 'a') < 0) ? c + diff : c;
}

int parseHexa(char c) {
	if (isDigit(c)) {
		return c - '0';
	} else if (toLower(c) <= 'f' && toLower(c) >= 'a') {
		return c - 'a' + 10;
	}
	return -1;
}

int gets(char* ans) {
	char c;
	int index = 0;
	do {
		c = getchar();
		if (c != '\b' || (c == '\b' && index > 0)) {
			if (c == '\b') {
				ans[index--] = '\0';
			} else {
				ans[index++] = c;
			}
			putchar(c);
		}
	} while(c != '\n');
	ans[index - 1] = '\0';
	return (index - 1) == 0 ? 0 : 1;
}

int getd(int* n) {
	char c;
	int total = 0;
	int index = 0;
	int isDig;
	do {
		c = getchar();
		isDig = isDigit(c);
		if (isDig) {
			total *= 10;
			total += c - '0';
		}
		putchar(c);
	} while(isDig);
	*n = total;
	return (index == 0) ? 0 : (c==' ' || c=='\n');
}

int getf(double* ans) {
	int n1 = 0, n2, isDig, n1IsValid = false;
	char c;
	double aux = 0;
	do {
		//Se lee primero la parte entera
		c = getchar();
		isDig = isDigit(c);
		if (isDig) {
			n1 *= 10;
			n1 += c - '0';
			n1IsValid = true;
		}
		putchar(c);
	} while(isDig);

	//luego de la parte entera tiene que venir un punto.
	if (n1IsValid && c == '.') {
		getd(&n2);
		aux = (double) pow(10, digits(n2));
		n2 /= aux;
		aux = n1 + n2;
		*ans = aux;
		return 1;
	}
	return 0;
}

int digits(int n) {
	int digits = 0;
	while(n) {
		n /= 10;
		digits++;
	}
	return digits;
}

