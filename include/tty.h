#ifndef TTY_H_
#define TTY_H_

#include "defs.h"

#define MAX_TTYs			4

#define ROWS 			25
#define COLUMNS 		80
#define TOTAL_TTY_SIZE 	(ROWS * COLUMNS * 2)

#define TTY_BUFFER_SIZE	10

typedef struct {
	int buffer[TTY_BUFFER_SIZE];
	int head;
	int tail;
} BUFFERTYPE;

typedef struct {
	char* terminal;
	int offset;
	BUFFERTYPE buffer;
} TTY;

#endif
