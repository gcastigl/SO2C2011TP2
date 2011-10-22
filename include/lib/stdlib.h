#ifndef STDLIB_H
#define STDLIB_H

#include <defs.h>

/*
void* malloc(u32int neededMem);

void* calloc(u32int size);

void free(void * pointer);
*/
void memcpy(void* to, void* from, u32int count);

void panic(char* msg, int line, int lock);

void* realloc(void* ptr, int newsize);

int atoi(const char *str);

#endif

