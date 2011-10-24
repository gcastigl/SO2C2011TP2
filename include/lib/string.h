#ifndef STRING_H
#define STRING_H

#include <defs.h>
#include <lib/stdlib.h>

u32int strlen(char *string);

int strcmp(const char * s1, const char * s2);

int strncmp(char * s1, char * s2, unsigned int n);

int substr(const char * s1, const char *s2);

void strcpy(char* to, char* from);

void *memset(void *s, int c, u32int n);

void strncpy(char *to, const char *from, int size);

int strIndexOf(char* str, char c, int startIndex);

int strContains(char* str, char c);

int strspn(const char *string, const char *in);
char *strpbrk(register const char *string, register const char *brk);
char *strtok(register char *string, const char *separators);

#endif
