#include "../../include/driver/video.h"

void terminal_scroll(char* terminal) {
	int row;
	for (row = 1; row < ROWS; row++) {
		terminal_copyRow(terminal, row, row - 1);
	}
	terminal_clearRow(terminal, ROWS - 1, -1);
}

void terminal_copyRow(char* terminal, int from, int to) {
	int offsetFrom = (2 * COLUMNS) * from;
	int offsetTo = (2 * COLUMNS) * to;
	int i;
	for (i = 0; i < 2 * COLUMNS; i++) {
		terminal[offsetTo + i] = terminal[offsetFrom + i];
	}
}

void terminal_clearRow(char* terminal, int row, char format) {
	int offset = (2 * COLUMNS) * row;
	int i;
	for (i = 0; i < 2 * COLUMNS; i+=2) {
		terminal[offset + i] = ' ';
		if (format != -1) terminal[offset + i + 1] = format;
	}
}

void terminal_clearRowRange(char* terminal, int from, int to) {
	int i;
	for (i = from; i <= to; i++) {
		terminal_clearRow(terminal, i, -1);
	}
}

void terminal_clearAll(char* terminal, char format) {
	int i;
	for (i = 0; i <= ROWS; i++) {
		terminal_clearRow(terminal, i, format);
	}
}

int terminal_getRow(int offset) {
	return (offset / 2) / COLUMNS;
}

int terminal_getColumn(int offset) {
	return (offset / 2) % COLUMNS;
}

int terminal_getOffset(int row, int column) {
	int offset = -1;
	if (0 <= row && row < ROWS && 0 <= column && column < COLUMNS) {
		offset = (row * COLUMNS * 2) + column * 2;
	}
	return offset;
}

void terminal_formatRange(char* terminal, int offsetFrom, int offsetTo, char format) {
	int i;
	offsetFrom = offsetFrom % 2 == 0 ? offsetFrom : offsetFrom + 1; // Even spaces are for characters
	for (i = offsetFrom; i < offsetTo; i+=2) {
		terminal[i + 1] = format;
	}
}

int terminal_prtSpecialCharater(char* terminal, int offset, char ascii, char format) {
	int initOffset = offset;
	int endOfRow, tab;
	switch (ascii) {
		case '\n':
			endOfRow = offset + 2 * COLUMNS - (offset % (2 * COLUMNS));
			terminal_formatRange(terminal, offset, endOfRow, format);
							offset = endOfRow;
			break;
		case '\t': // Tab
				tab = 2 * TAB_SIZE - (offset % (2 * TAB_SIZE));
				terminal_formatRange(terminal, offset, offset + tab, format);
				offset += tab;
			break;
		case '\b': // Backspace
			offset -= 2;
			terminal[offset] = ' ';
			if (format != -1) {
				terminal[offset + 1] = format;
			}
			break;
	}
	return offset - initOffset;
}
