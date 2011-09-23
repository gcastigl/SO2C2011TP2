#include "../include/io.h"

int isTTY(int fd);

void sysRead(int fd, void * buffer, size_t count) {
	int i;
	char c;
	char * aux;
	if (fd == STD_IN) {
		//COPIA DEL BUFFER DE TECLADO al BUFFER ENTREGADO
		for (i = 0; i < count; i++) {
			aux = (char*) buffer;
			c = getKeyFromBuffer();
			*(aux+i) = c;
		}
	}
}

void sysWrite(int fd, void * buffer, size_t count) {
	if (fd == STD_OUT || fd == STD_ERROR) {
		// COPIA DEL BUFFER ENTREGADO A LA PANTALLA
		// FIXME: THIS should write to the current TTY....
		writeInVideo((char*) buffer, count);
	} else if (isTTY(fd)) {
		tty_write(fd - 3, (char*) buffer, count);
		TTY* tty = tty_getTTY(fd - 3);
		count *= 2; // For each caracter, there is one more for the format
		int offset = tty->offset - count;
		video_writeFormattedBuffer(tty->terminal + offset, count, offset);
	}
}

int isTTY(int fd) {
	return 3 <= fd && fd < MAX_TTYs + 3;
}
