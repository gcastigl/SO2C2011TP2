#include "../include/tty.h"

void initTTY(int index);
void write(TTY* tty, char* buffer, size_t size);

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
	tty[index].terminal = (char*) calloc(TOTAL_TTY_SIZE);
	tty[index].offset = 0;
	tty[index].buffer.head = 0;
	tty[index].buffer.tail = 0;
	tty[index].bgColor = BLACK;
	tty[index].fgColor = GREEN;
}

void tty_setCurrent(int tty) {
	currentTTY = tty;
}

TTY* tty_getTTY(int index) {
	return &tty[index];
}

TTY* tty_getCurrent() {
	return &tty[currentTTY];
}

void tty_write(int ttyIndex, char* buffer, size_t size) {
	write(&tty[ttyIndex], buffer, size);
}

// Function to copy from a buffer to video format
void write(TTY* tty, char* buffer, size_t size) {
	int i;
	char* temp = tty->terminal + tty->offset;
	for (i = 0; i < size; i++) {
		temp[2 * i] = buffer[i];
		temp[2 * i + 1] = ' ';
	}
	tty->offset += size * 2;
	/*char* temp = tty->terminal + tty->offset;
	int i, erasedChars = 0;
	for (i = 0; i < size; i++) {
		int pos = 2 * (i - erasedChars) + 1;
		if (buffer[i] != '\b') {
			temp[pos] = buffer[i];
			temp[pos + 1] = tty->bgColor << 4 | (tty->bgColor & 0x0F);
		} else {
			temp[pos] = ' ';
			erasedChars++;
		}
	}
	tty->offset += 2 * (size - erasedChars);
	tty->offset %= TOTAL_TTY_SIZE;*/
}
