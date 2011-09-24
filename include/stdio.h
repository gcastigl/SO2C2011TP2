#include "varargs.h"
#include "stdarg.h"

#ifndef STDIO_H
#define STDIO_H

#define F_PRECISION 6

char getchar();

void putchar(char c);

void printf(char * formatString, ...);

int sscanf(char* formatString, char *format, ...);

static int currentFd;

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

int scanf(const char *format, ...);
int gets(char* ans);
int getd(int* n);
int getf(double* n);

int parseHexa(char c);
int isDigit(char c);
int isNumber(char c);
char toLower(char c);
int digits(int n);

#endif //STDIO_H
