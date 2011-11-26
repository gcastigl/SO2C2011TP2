#include <util/circularBuffer.h>
#include <lib/stdlib.h>

PRIVATE int _nextIndex(int current, int size);

void circularBuffer_init(c_buffer_t* buffer, int bufferSize) {
	buffer->buffer = kmalloc(bufferSize * sizeof(char));
	buffer->size = bufferSize;
	buffer->readPos = -1;
	buffer->writePos = -1;
	buffer->count = 0;
}

boolean circularBuffer_add(c_buffer_t* buffer, char c) {
	if (circularBuffer_isFull(buffer)) {
		log(L_ERROR, "buffer is full!!");
		return false;
	}
	buffer->count++;
	buffer->writePos = _nextIndex(buffer->writePos, buffer->size);
	buffer->buffer[buffer->writePos] = c;
	return true;
}

char circularBuffer_get(c_buffer_t* buffer) {
	if (circularBuffer_isEmpty(buffer)) {
		log(L_ERROR, "buffer is empty!!");
		return '\0';
	}
	log(L_DEBUG, "GET KEY FROM BUFFER!");
	buffer->count--;
	buffer->readPos = _nextIndex(buffer->readPos, buffer->size);
	return buffer->buffer[buffer->readPos];
}

boolean circularBuffer_isEmpty(c_buffer_t* buffer) {
	return buffer->count == 0;
}

boolean circularBuffer_isFull(c_buffer_t* buffer) {
	return buffer->count == buffer->size;
}

PRIVATE int _nextIndex(int current, int size) {
	return (current + 1) % size;
}

void circularBuffer_clear(c_buffer_t* buffer) {
	buffer->readPos = -1;
	buffer->writePos = -1;
	buffer->count = 0;
}
