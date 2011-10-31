#include <io.h>

int isTTY(int fd);
PRIVATE int nextFreeFd = FD_OFFSET;

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
	} else if (fd >= FD_OFFSET) {
	    FILE *file = &openFiles[fd - FD_OFFSET];
        int read = read_fs(file->node, file->offset, count, buffer);
        file->offset += read;
	}
}

void sysWrite(int fd, void * buffer, u32int count) {
	TTY* tty;
	if (fd < FD_OFFSET) {
	    if (fd == STD_OUT || fd == STD_ERROR) {
    		tty = tty_getCurrentTTY();
    	} else if (isTTY(fd)) {
    		tty = tty_getCurrentTTY(fd - 3);
    	}
    	tty_write(tty, (char*) buffer, count);
    	video_setOffset(0);
    	video_write(tty->terminal, tty->offset + 1);
    } else {
        FILE *file = &openFiles[fd - FD_OFFSET];
        int written = write_fs(file->node, file->offset, count, buffer);
        file->offset += written;
    }
}

int sysOpen(char* fileName, int oflags, int cflags) {
    int inode;
    int fd = 0;
    int i;
    if (oflags & O_CREAT) {
        // FIXME: SHOULD CONSIDER CFLAGS
        inode = fs_createFile(tty_getCurrentTTY()->currDirectory, fileName, FS_FILE);
        if ((oflags & O_EXCL) && (inode == E_FILE_EXISTS)) {
            return ERROR;
        }
    }
    if (nextFreeFd < MAX_OPEN_FILES) {
        TTY* tty = tty_getCurrentTTY();
    	u32int currentiNode = tty->currDirectory;
    	fs_node_t current;
    	fs_getFsNode(&current, currentiNode);
        for (i = 0; i < MAX_OPEN_FILES; i++) {
            if (strcmp(fileName, openFiles[i].name) == 0) {
                break;
            }
        }
        if (i != MAX_OPEN_FILES) {
            fd = i + FD_OFFSET;
        }
        if (fd == 0) {
            fs_node_t *node = finddir_fs(&current, fileName);
            FILE *file = &openFiles[nextFreeFd - FD_OFFSET];
            strcpy(file->name, fileName);
            file->node = node;
            file->flags &= oflags;
            fd = nextFreeFd++;
        }
    }
    return fd;
}


int isTTY(int fd) {
	return 3 <= fd && fd < MAX_TTYs + 3;
}
