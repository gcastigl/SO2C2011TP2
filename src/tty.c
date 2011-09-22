#include "../include/tty.h"

void initTTY(int index);

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
	tty[index].terminal = malloc(TOTAL_TTY_SIZE);
	int i;
	for ( i = 0; i < TOTAL_TTY_SIZE; i+=2) {
		tty[index].terminal[i] = 0;
		tty[index].terminal[i + 1] = 0x07;
	}
	tty[index].offset = 0;
	tty[index].buffer.head = 0;
	tty[index].buffer.tail = 0;
}

