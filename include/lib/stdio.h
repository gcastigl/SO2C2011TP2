#ifndef STDIO_H
#define STDIO_H

#include <varargs.h>
#include <stdarg.h>

#define F_PRECISION	6

typedef struct {
	int fd;
} FILE;

char getchar();

void putchar(char c);

void printf(char * formatString, ...);

int sscanf(char* formatString, char *format, ...);

void setFD(int fileDescriptor);

void putc(char c, int fd);
char getc(int fd);
void puti(int n);
void putui(unsigned int n);
void puth(int n, int upperCase);
void puts(char* s);
void putf(double n);
void pute(double n, int upperE);
char getchar();
void putchar(char c);
int sprintf(char *out, const char *format, ...);

int scanf(const char *format, ...);
int gets(char* ans);
int getd(int* n);
int getf(double* n);

int parseHexa(char c);
int isDigit(char c);
int isNumber(char c);
char toLower(char c);
int digits(int n);

char* itoa(int i);

// =========================================
// 		Character input/output
// =========================================

//fclose
//fopen


char fgetc(FILE* steam);

void fprintf(FILE* steam, char* tpt, ...);

int fclose(FILE *stream);

#endif //STDIO_H
