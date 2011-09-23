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

/* Inicializa el struct del video */
void initVideo();

/* writes in video a string */
void writeInVideo(char *string, size_t count);

/* Setea el cursor en la posición deseada */
void setCursor(ushort row, ushort col);

void video_clearScreen(char format);

/* Chequea si el parámetro es un ascii especial */
int specialAscii(char ascii);

/* Setea la posición en la cual va escribirse en la pantalla */
void setPosition(int row, int column);

/* Devuelve la columna en la cual se va a escribir */
int getCurrRow();

/* Devuelve la fila en la cual se va a escribir */
int getCurrColumn();

/* Escribe un char directamente en la memoria */
void dummyWrite(char ascii);

/* Sube la pantalla n líneas */
void scroll(char lines);

/* Setea el offset con respecto al inicio de la memoria de video */
void setOffset(int offset);

/* Devuelve el offset con respecto al inicio de la memoria de video */
int getOffset();

/* Copia una linea de "source" a "dest" */
void copyRow(int source, int dest);

/* limpia n líneas en un rango */
void clearLinesRange(int from, int to);

/* limpia desde la línea n hasta el final de la pantalla */
void clearToEnd(int from);

/* limpia una linea */
void clearRow(int row);

// ===========================================
// Functions to operate with ttys
// ===========================================

/*
 * Dado un buffer con formato, se compia tal cual es entregado a pantalla.
 * Analogo a llamar memcpy a la direccion de video;
 */
void video_writeFormattedBuffer(char* buffer, size_t size, int videOffset);

char video_getFormattedColor(char fg, char bg);

char video_getBGcolor(char format);

char video_getFGcolor(char format);

#endif
