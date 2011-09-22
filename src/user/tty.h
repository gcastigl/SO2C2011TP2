#ifndef TTY_H
#define	TTY_H

#include "../stdlib/stdlib.h"

#define NUMBER_OF_TTYs  8
#define ROWS            25
#define COLUMNS         80

typedef struct {
    int buffer[10];
    int head;
    int tail;
} BUFFERTYPE;

typedef struct {
    char* screen;
    int offset;
    BUFFERTYPE buffer;
} TTY;

#endif	/* TTY_H */

