#ifndef TTY_H
#define TTY_H

#include <defs.h>
#include <lib/kheap.h>
#include <fs/fs.h>
#include <lib/stdlib.h>
#include <driver/video.h>

#define MAX_TTYs		1

#define BUFFER_SIZE	1024

typedef struct {
    int id;
	char* terminal;
	int offset;
	char buffer[BUFFER_SIZE];
    int bufferOffset;
	u32int currDirectory;
	char currPath[64];
	int currPathOffset;
	// FIXME: This values could be wrappeed up in a VideoAttributtes structure
	char bgColor;
	char fgColor;
} TTY;

int initTTY();

void tty_setCurrent(int tty);

int tty_getCurrent();

TTY* tty_getCurrentTTY();

void tty_getCurrentNode(fs_node_t* node);

TTY* tty_getTTY(int index);

void tty_write(TTY* tty, char* buffer, u32int size);

void tty_clean(TTY* tty);

void tty_setFormatToCurrTTY(char format);

char tty_getCurrTTYFormat();

#endif
