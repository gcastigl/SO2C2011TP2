#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "kasm.h"
#include "defs.h"
#include "video.h"
#include "shell.h"
#include "video.h"
#include "stdlib.h"

#define ESCAPE	0x01
#define UCASE	0x02
#define CTRL	0x04
#define ALT		0x08
#define DEL		0x10
#define CLEAR_BREAK_BIT(A) ((A) & 0x7F)
#define SHIFT_PRESSED() (kbFlags & UCASE)
#define IS_MAKE(A) ((A) & 0x80)
#define IS_ESCAPE() (kbFlags & ESCAPE)
#define IS_CTRL() (kbFlags & CTRL)
#define IS_ALT() (kbFlags & ALT)
#define IS_DEL() (kbFlags & DEL)

typedef struct {
	int from;
	int to;
	char buffer[K_BUFFER_SIZE]; 
} K_BUFFER;

K_BUFFER keyboard_buffer;
char kbFlags;

void handleScanCode(unsigned char scanCode);
int checkSpecialKey(unsigned char scanCode);
char translateSc(unsigned char scanCode);

void putKeyInBuffer(char c);
char getKeyFromBuffer();
void keyboard_init();
int bufferIsEmpty();

#endif


