#include "../include/tty.h"

void initTTY(int index);

static TTY tty[MAX_TTYs];
static int currentTTY;

void tty_init() {
	int i;
	currentTTY = 0;
	for (i = 0; i < MAX_TTYs; i++) {
		initTTY(i);
	}
	char format = video_getFormattedColor(tty[currentTTY].fgColor, tty[currentTTY].bgColor);
	video_clearScreen(format);
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
	TTY* currTTY = tty_getCurrentTTY();
	video_clearScreen(video_getFormattedColor(currTTY->fgColor, currTTY->bgColor));
	video_writeFormattedBuffer(currTTY->terminal, 50, 0);
	//memcpy((void*) VIDEO_ADDRESS, currTTY->terminal, TOTAL_VIDEO_SIZE);
}

int tty_getCurrent() {
	return currentTTY;
}

TTY* tty_getTTY(int index) {
	return &tty[index];
}

TTY* tty_getCurrentTTY() {
	return &tty[currentTTY];
}

void tty_write(TTY* tty, char* buffer, size_t size) {
	int i;
	char* temp = tty->terminal + tty->offset;
	char format = video_getFormattedColor(tty->fgColor, tty->bgColor);
	for (i = 0; i < size; i++) {
		int pos = 2 * i;
		temp[pos] = buffer[i];
		temp[pos + 1] = format;
	}
	tty->offset += size * 2;
	tty->offset %= TOTAL_TTY_SIZE;
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
