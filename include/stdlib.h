#ifndef STDLIB_H
#define STDLIB_H

#include "defs.h"

void* malloc(size_t neededMem);

void* calloc(size_t size);

void free(void * pointer);

void memcpy(void* to, void* from, size_t count);

#endif

