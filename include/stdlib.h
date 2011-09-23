#ifndef STDLIB_H
#define STDLIB_H

#include "defs.h"
#include "video.h"

#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_copy(d,s)  __builtin_va_copy(d,s)

#define F_PRECISION 6

typedef __builtin_va_list va_list;

void setMemory();
void* malloc(size_t neededMem);
void* calloc(size_t size);
void free(void * pointer);
void printf(const char *fmt, ...);
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
char toLower(char c);
int digits(int n);

#endif

