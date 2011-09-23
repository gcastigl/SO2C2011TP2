#include "../include/stdlib.h"

static int nextfree = 0x300000;
static int currentFd;

void setFD(int fileDescriptor) {
	currentFd = fileDescriptor;
}

void* malloc(size_t size) {
	void* temp = (void*) nextfree;
	nextfree = nextfree + size;
	return temp;
}

// Malloc inicializado en 0
void* calloc(size_t size) {
	char* temp;
	int i;
	temp = (char*) malloc(size);
	for(i=0;i<size;i++)
		temp[i] = 0;
	return (void*) temp;
}

void free(void * pointer) {
	// TODO: implementame!
}

char getchar() {
	return getc(STD_IN);
}

char getc(int fd) {
	char c;
	while(bufferIsEmpty()) {
	}
	__read(fd, &c, 1);
	return c;
}

void putchar(char c) {
	putc(c, currentFd);
}

void putc(char c, int fd) {
	__write(fd, &c, 1);
}

void puti(int n) {
	if (n < 0) {
		putchar('-');
		n = 0 - n;
	}
	putui(n);
}

void putui(unsigned int n) {
	if (n <= 0) {
		return;
	}
	putui(n / 10);
	putchar((n % 10) + '0');
}

void puth(int n, int upperCase) {
	char *letters = upperCase ? "ABCDEF" : "abcdef";
	char c;
	int num;
	if (n <= 0) {
		return;
	}
	num = n % 16;
	c = num > 9 ? letters[num - 10] : (num + '0');
	
	puth(n / 16, upperCase);
	putchar(c);
}

void puts(char* s) {
	char c;
	while ((c = *s++) != 0) {
		putchar(c);
	}
}

void putf(double n) {
	puti((int)n);
	if(n == 0) {
		return;
	}
	putchar('.');
	n = n - (int)n;
	n *= pow(10, F_PRECISION);
	puti((int)n);
}

void pute(double n, int upperE) {
}

void printf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	double t;
	char c;
	int n;
	while ((c = *fmt++) != 0) {
		if (c != '%') {
			putchar(c);
		} else {
			c = *fmt++;
			switch(c) {
				case '%':
					putchar('%');
					break;
				case 'i':
				case 'd':
					n = va_arg(args, int);
					if (n == 0) {
						putchar('0');
					} else puti(n);
					break;
				case 'u':
					putui(va_arg(args, int));
				case 'x':
					puth(va_arg(args, int), false);
					break;
				case 'X':
					puth(va_arg(args, int), true);
					break;
				case 's':
					puts(va_arg(args, char*));
					break;
				case 'f':
					putf(va_arg(args, double));
					break;
				case 'e':
					pute(va_arg(args, double), false);
					break;
				case 'E':
					pute(va_arg(args, double), true);
					break;
				case 'c':
					c = (char)va_arg(args, int);
					putchar(c);
					break;
				default:
					break;
			}
		}
	}
	va_end(args);
}

void memcpy(void* from, void* to, size_t count) {
	size_t i;
	char* cfrom = (char *) from;
	char* cto = (char *) to;
	for (i = 0; i < count; i++) {
		cto[i] = cfrom[i];
	}
}

int isDigit(char c) {
	return (0 <= c - '0') && (c - '0' <= 9);
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

int gets(char* ans) {
	char c;
	int index = 0;
	do {
		c = getchar();
		if ( c!= '\b' || (c == '\b' && index >= 0) ) {
			if (c == '\b') {
				ans[index--] = '\0';
			} else {
				ans[index++] = c;
			}
			putchar(c);
		}
	} while( c != '\n');
	ans[index++] = '\0';
	return index == 0 ? 0 : 1;
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
	double total = 0, aux = 0;
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

