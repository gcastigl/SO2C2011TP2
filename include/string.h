#ifndef STRING_H
#define STRING_H

#include "defs.h"
#include "stdio.h"

size_t strlen(char *string);

int strcmp(const char * s1, const char * s2);

int strncmp(char * s1, char * s2, unsigned int n);

int substr(const char * s1, const char *s2);

void strcpy(char* to, char* from);

#endif
