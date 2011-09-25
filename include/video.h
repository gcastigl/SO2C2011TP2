#ifndef VIDEO_H
#define VIDEO_H

#include "io.h"

#define VIDEO_ADDRESS 	0xb8000

#define ROWS 				25
#define COLUMNS 			80
#define TOTAL_VIDEO_SIZE 	(ROWS * COLUMNS * 2)

#define TAB_SIZE		4
/*
 * Color Defines
 * See http://wiki.osdev.org/Text_mode for color visuals
 * Warning: Using a background with highest bit in 1 will make text blink
 */
#define BLACK			0x0
#define BLUE			0x1
#define GREEN			0x2
#define CYAN			0x3
#define RED				0x4
#define MAGENTA			0x5
#define BROWN			0x6
#define LIGHT_GREY		0x7
#define DARK_GRAY		0x8
#define LIGHT_BLUE		0x9
#define LIGHT_GREEN		0xA
#define LIGHT_CYAN		0xB
#define LIGHT_RED		0xC
#define LIGHT_MAGENTA	0xD
#define YELLOW			0xE
#define WHITE			0xF


typedef struct {
	char* address;
	int offset;
} VideoInfo;

VideoInfo video;

/* Inicializa los valores iniciales para el manejo de la pantalla*/
void video_init();

void video_write(char *string, size_t count, char format);

void video_setCursor(int offset);

/* Limpia toda la pantalla de acuerdo al formato dado por parametro*/
void video_clearScreen(char format);

void video_setOffset(int offset);

char video_getFormattedColor(char fg, char bg);

char video_getBGcolor(char format);

char video_getFGcolor(char format);

int terminal_prtSpecialCharater(char* terminal, int offset, char ascii, char format) ;

void video_clearRow(int row, char format);

// ====================================

void terminal_scroll(char* terminal);

void terminal_copyRow(char* terminal, int from, int to);

void terminal_clearRow(char* terminal, int row, char format);

void terminal_clearRowRange(char* terminal, int from, int to);

void terminal_clearAll(char* terminal, char format);

int terminal_getRow(int offset);

int terminal_getColumn(int offset);

int terminal_getOffset(int row, int column);

void terminal_formatRange(char* terminal, int offsetFrom, int offsetTo, char format);

#endif
