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

// FIXME: how to recognize each FD according to its value
// We are now assuming that all fd are TTYs
void sysWrite(int fd, void * buffer, size_t count) {
	TTY* tty;
	if (fd == STD_OUT || fd == STD_ERROR) {
		tty = tty_getCurrentTTY();
	} else if (isTTY(fd)) {
		tty = tty_getCurrentTTY(fd - 3);
	}
	tty_write(tty, (char*) buffer, count);
	count *= 2; // For each caracter, there is one more for the format
	int offset = tty->offset - count;
	video_writeFormattedBuffer(tty->terminal + offset, count, offset);
}

int isTTY(int fd) {
	return 3 <= fd && fd < MAX_TTYs + 3;
}
