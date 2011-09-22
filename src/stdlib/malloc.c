#include "stdlib.h"

int nextfree = 0x300000;

void* malloc(int size) {
    void* temp = (void*) nextfree;
    nextfree = nextfree + size;
    return temp;
}

void* calloc(int size) {
    char* temp;
    int i;
    temp = (char*) malloc(size);
    for(i = 0; i < size; i++) {
        temp[i] = 0;
    }
    return (void*)temp;
}

void free(void* p) {
    // TODO: hacer!
}