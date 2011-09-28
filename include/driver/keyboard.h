#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../asm/libasm.h"
#include "../defs.h"
#include "../shell.h"
#include "../driver/video.h"
#include "../lib/stdlib.h"

#define ESCAPE	0x01
#define UCASE	0x02
#define CTRL	0x04
#define ALT		0x08
#define DEL		0x10
#define FN		0x20

#define CLEAR_BREAK_BIT(A)	((A) & 0x7F)
#define SHIFT_PRESSED() 	(kbFlags & UCASE)
#define IS_MAKE(A) 			((A) & 0x80)
#define IS_ESCAPE()			(kbFlags & ESCAPE)
#define IS_CTRL() 			(kbFlags & CTRL)
#define IS_ALT() 			(kbFlags & ALT)
#define IS_DEL() 			(kbFlags & DEL)
#define F_PRESSED(i) 		((fKeys >> (i)) & 1)

typedef struct {
	int from;
	int to;
	char buffer[K_BUFFER_SIZE]; 
} K_BUFFER;

K_BUFFER keyboard_buffer;
char kbFlags;
char fKeys;

void handleScanCode(unsigned char scanCode);
int checkSpecialKey(unsigned char scanCode);
char translateSc(unsigned char scanCode);

void putKeyInBuffer(char c);
char getKeyFromBuffer();
void keyboard_init();
int bufferIsEmpty();
int keyboard_getPressedFkey();

#endif


