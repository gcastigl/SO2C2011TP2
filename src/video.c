#include "../include/video.h"

void initVideo() {
	video.address = (char*)VIDEO_ADDRESS;
	setVideoColor(BLACK, GREEN);
	cls();
	setOffset(0);
	setCursor(0, 0);
}

/*
	Escribe en pantalla count caracteres, desde string con color de fuente 
	fgc y color de fondo bgs.
	Estos caracteres se agregan al final del ultimo caracter mandado. 
*/
void writeInVideoColors(char *string, size_t count, int fgc, int bgc) {
	int i = 0;
	while (i < count) {
		char ascii = string[i];
		if (!specialAscii(ascii)) {
			video.address[getOffset()] = ascii;
			video.address[getOffset() + 1] = bgc << 4 | fgc & 0x0F;
			if (getOffset() == TOTAL_VIDEO_SIZE - 2) {
				scroll(1);
				setPosition(getCurrRow(), 0);
			}
			setOffset(getOffset() + 2);
			setCursor(getCurrRow(), getCurrColumn());
		}
		i++;
	}
	return;
}

void writeInVideo(char *string, size_t count) {
	writeInVideoColors(string, count, video.fgColor, video.bgColor);
}

/*
	Corre lo mostrado en pantalla lines lineas hacia arriba.
*/
void scroll(char lines) {
	int i;
	int start = getCurrRow();
	for (i = lines; i <= start; i++) {
		copyRow(i, i - lines);
	}
	clearLinesRange(start - lines + 1, start);
}

/*
	Copia toda la fila source a la posicion dest. Se copian los caracteres y
	su formato de color.
*/
void copyRow(int source, int dest) {
	if (dest < 0) {
		return;
	}
	int posBak = getOffset();
	int column;
	for (column = 0; column < COLUMNS; column++) {
		setPosition(source, column);
		int index = getOffset();
		setPosition(dest, column);
		video.address[getOffset()] = video.address[index];
		video.address[getOffset() + 1] = video.address[index + 1];
	}
	setOffset(posBak);
}

/*
	Borra un rango de lineas. Es decir, pisa desde la linea from a la linea
	to con espacios. No borra el formato de color.
*/
void clearLinesRange(int from, int to) {
	to = to >= ROWS ? ROWS - 1 : to;
	int i;
	for (i = from; i <= to; i++) {
		clearRow(i);
	}
}

void clearToEnd(int from) {
	clearLinesRange(from, ROWS);
}

void clearRow(int row) {
	int i;
	int offsetBackup = getOffset();
	for (i = 0; i < COLUMNS; i++) {
		setPosition(row, i);
		video.address[getOffset()] = ' ';
	}
	setOffset(offsetBackup);
}

void setPosition(int row, int column) {
	int offset;
	if ( 0 <= row && row < ROWS && 0 <= column && column < COLUMNS) {
		offset = (row * COLUMNS) + column;
	} else if (row >= ROWS) {
		row = ROWS - 1;
		offset = row * COLUMNS;
		scroll(1);
	}
	
	setOffset(offset * 2);
}

/*
	Columns actual de fin de la ultimo caracter
*/
int getCurrRow() {
	return (getOffset() / 2) / COLUMNS; 
}

/*
	Fila actual de fin de la ultimo caracter
*/
int getCurrColumn() {
	return (getOffset() / 2) % COLUMNS; 
}

void setVideoBackground(byte color) {
	video.bgColor = color;
}

void setVideoForeground(byte color) {
	video.fgColor = color;
}

void setVideoColor(byte bg, byte fg) {
	setVideoForeground(fg);
	setVideoBackground(bg);
	int i;
	for (i = 1; i < TOTAL_VIDEO_SIZE; i+=2) {
		video.address[i] = getVideoColor();
	}
}

char getVideoColor() {
	return (video.bgColor << 4) | (video.fgColor & 0x0F);
}

int getOffset() {
	return video.offset;
}

void setOffset(int offset) {
	if (offset < 0) {
		offset = 0;
	}
	video.offset = offset;
}

/*
	Setea el cursor en la posicion row, column.
*/
void setCursor(ushort row, ushort column) {
	if (row >= ROWS || row < 0 || column >= COLUMNS || column < 0) {
		return;
	}
	ushort position = (row * COLUMNS) + column;

	// cursor LOW port to vga INDEX register
	outb(0x3D4, 0x0F);
	outb(0x3D5, (ushort)(position & 0xFF));
	// cursor HIGH port to vga INDEX register
	outb(0x3D4, 0x0E);
	outb(0x3D5, (ushort)((position >> 8) & 0xFF));
}

/*
	Borra la pantalla en modo texto color.
*/
void cls() {
	unsigned int i = 0;
	while (i < TOTAL_VIDEO_SIZE) {
		video.address[i] = ' ';
		i++;
		video.address[i] = getVideoColor();
		i++;
	};
	setOffset(0);
}

/*
	Imprime caracter especiales a ontinuacion del ultimo caracter agregado a 
	pantalla.
*/
int specialAscii(char ascii) {
	int ret = TRUE;
	int tab;
	switch (ascii) {
		case '\n':
			setPosition(getCurrRow() + 1, 0);
			break;
		case '\t': //Tab
				tab = (getCurrColumn() % TAB_SIZE != 0) ? 
					TAB_SIZE - (getCurrColumn() % TAB_SIZE) : TAB_SIZE;
				if (getCurrColumn() + tab < COLUMNS) {
					setPosition(getCurrRow(), getCurrColumn() + tab);
				}
			break;
		case '\b': //Backspace
			setOffset(getOffset() - 2);
			video.address[getOffset()] = ' ';
			break;
		default:
			ret = FALSE;
			break;
	}
	setCursor(getCurrRow(), getCurrColumn());
	return ret;
}

