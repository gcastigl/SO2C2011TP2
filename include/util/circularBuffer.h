#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <defs.h>

typedef struct {
	char* buffer;
	int size;
	int readPos;
	int writePos;
	int count;
} c_buffer_t;

void circularBuffer_init(c_buffer_t* buffer, int bufferSize);

boolean circularBuffer_add(c_buffer_t* buffer, char c);

char circularBuffer_get(c_buffer_t* buffer);

boolean circularBuffer_isEmpty(c_buffer_t* buffer);

boolean circularBuffer_isFull(c_buffer_t* buffer);

void circularBuffer_clear(c_buffer_t* buffer);

#endif
