#include <io.h>

int isTTY(int fd);
PRIVATE fs_node_t* getFile(char* fileName);
PRIVATE int nextFreeIndex(file_descriptor_entry* files);

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
		fs_node_t node;
		file_descriptor_entry* file = &(getCurrentProcess()->fd_table[fd - FD_OFFSET]);
		fs_getFsNode(&node, file->inode);
        int read = read_fs(&node, file->offset, count, buffer);
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
    	fs_node_t node;
    	printf("opening descriptor: %d", fd - FD_OFFSET);
    	file_descriptor_entry* file = &(getCurrentProcess()->fd_table[fd - FD_OFFSET]);
    	fs_getFsNode(&node, file->inode);
        int written = write_fs(&node, file->offset, count, buffer);
        file->offset += written;
    }
}

int sysOpen(char* fileName, int oflags, int cflags) {
    int fd = 0;
    /*if (oflags & O_CREAT) {
    	errno = 0;
        // FIXME: SHOULD CONSIDER CFLAGS
        fs_node_t current;
        fs_getFsNode(&current, tty_getCurrentTTY()->currDirectory);
        inode = createdir_fs(&current, fileName, FS_FILE);
        if ((oflags & O_EXCL) && (errno == E_FILE_EXISTS)) {
            return ERROR;
        }
    }*/
    file_descriptor_entry* files = getCurrentProcess()->fd_table;
    int next = nextFreeIndex(files);
    if (next != -1) {							// Check if process has available slot to open a file
    	fs_node_t* node = getFile(fileName);		// Get file to be opened
    	if (node == NULL) {
    		errno = E_FILE_NOT_EXISTS;
			return -1;
		}
    	fd = next;
		file_descriptor_entry *file = &files[next];
		strcpy(file->name, fileName);
		file->inode = node->inode;
		file->mode = oflags;
		file->mask = FILE_TYPE(node->mask);		// POSSIBLE FIXME HERE!
		file->count = 0;
		file->offset = 0;
    }
    return fd + FD_OFFSET;
}

PRIVATE int nextFreeIndex(file_descriptor_entry* files) {
	for (int i = 0; i < MAX_FILES_PER_PROCESS; i++) {
		if (files[i].mask == 0) {
			return i;
		}
	}
	return -1;
}

PRIVATE fs_node_t* getFile(char* fileName) {
	fs_node_t current;
	fs_getFsNode(&current, tty_getCurrentTTY()->currDirectory);
	return finddir_fs(&current, fileName);
}

int isTTY(int fd) {
	return 3 <= fd && fd < MAX_TTYs + 3;
}
