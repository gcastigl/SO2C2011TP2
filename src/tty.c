#include "../include/tty.h"

void initTTY(int index);
void write(int tty, char* buffer, size_t size);

static TTY tty[MAX_TTYs];
static int currentTTY;

void initTTYs() {
	int i;
	currentTTY = 0;
	for (i = 0; i < MAX_TTYs; i++) {
		initTTY(i);
	}
}

void initTTY(int index) {
	tty[index].terminal = (char*) malloc(TOTAL_TTY_SIZE);
	int i;
	for ( i = 0; i < TOTAL_TTY_SIZE; i+=2) {
		tty[index].terminal[i] = 0;
		tty[index].terminal[i + 1] = 0x07;
	}
	tty[index].offset = 0;
	tty[index].buffer.head = 0;
	tty[index].buffer.tail = 0;
	tty[index].bgColor = BLACK;
	tty[index].fgColor = GREEN;
}

void tty_setCurrent(int tty) {
	currentTTY = tty;
}

TTY* tty_getCurrent() {
	return &tty[currentTTY];
}

void tty_write(int index, char* buffer, size_t size) {
	write(index, buffer, size);
}

// Function to copy from a buffer to video format
void write(int index, char* buffer, size_t size) {
	int i;
	char* temp = tty[index].terminal + tty[index].offset;
	for (i = 0; i < size; i++) {
		temp[2 * i] = buffer[i];
		temp[2 * i + 1] = tty[index].bgColor << 4 | (tty[index].bgColor & 0x0F);
	}
}
