#include <io.h>

int isTTY(int fd);

void sysRead(int fd, void * buffer, u32int count) {
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
void sysWrite(int fd, void * buffer, u32int count) {
	TTY* tty;
	if (fd == STD_OUT || fd == STD_ERROR) {
		tty = tty_getCurrentTTY();
	} else if (isTTY(fd)) {
		tty = tty_getCurrentTTY(fd - 3);
	}
	tty_write(tty, (char*) buffer, count);
	video_setOffset(0);
	video_write(tty->terminal, tty->offset + 1);
}

int sysOpen(char* fileName, int oflags, int cflags) {
    int inode;
    if (oflags & O_CREAT) {
        // FIXME: SHOULD CONSIDER CFLAGS
        inode = fs_createFile(tty_getCurrentTTY()->currDirectory, fileName);
        if ((oflags & O_EXCL) && (inode == E_FILE_EXISTS)) {
            return ERROR;
        }
    }
    TTY* tty = tty_getCurrentTTY();
	u32int currentiNode = tty->currDirectory;
	fs_node_t current;
	fs_getFsNode(&current, currentiNode);
    return 0;
}


int isTTY(int fd) {
	return 3 <= fd && fd < MAX_TTYs + 3;
}
