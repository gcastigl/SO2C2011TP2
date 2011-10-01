#ifndef TTY_H
#define TTY_H

#include <defs.h>
#include <fs.h>
#include <lib/kheap.h>
#include <lib/stdlib.h>
#include <driver/video.h>

#define MAX_TTYs		8

#define TTY_BUFFER_SIZE	10

typedef struct {
	int buffer[TTY_BUFFER_SIZE];
	int head;
	int tail;
} BUFFERTYPE;

typedef struct {
	char* terminal;
	Directory_t* currDirectory;
	char currPath[64];
	int offset;
	BUFFERTYPE buffer;
	// FIXME: This values could be wrappeed up in a VideoAttributtes structure
	char bgColor;
	char fgColor;
} TTY;

void tty_init();

void tty_setCurrent(int tty);

int tty_getCurrent();

TTY* tty_getCurrentTTY();

TTY* tty_getTTY(int index);

void tty_write(TTY* tty, char* buffer, u32int size);

void tty_clean(TTY* tty);

void tty_setFormatToCurrTTY(char format);

char tty_getCurrTTYFormat();

#endif
