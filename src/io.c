#include <io.h>

PRIVATE int isTTY(int fd);
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
	} else if (fd >= MAX_TTYs) {
		fs_node_t node;
		u32int index = fd - FD_OFFSET;
		file_descriptor_entry* file = &(scheduler_getCurrentProcess()->fd_table[index]);
		fs_getFsNode(&node, file->inode);
        read_fs(&node, file->offset, count, buffer);
        return;
	}
}

void sysWrite(int fd, void * buffer, u32int count) {
	TTY* tty;
	if (fd == STD_OUT || fd == STD_ERROR) {
		tty = tty_getCurrentTTY();
	} else if (isTTY(fd)) {
		tty = tty_getTTY(fd);
    } else {
    	fs_node_t node;
    	u32int index = fd - FD_OFFSET;
    	file_descriptor_entry* file = &(scheduler_getCurrentProcess()->fd_table[index]);
    	fs_getFsNode(&node, file->inode);
        write_fs(&node, file->offset, count, buffer);
        return;
    }
	tty_write(tty, (char*) buffer, count);
	video_setOffset(0);
	if (tty->id == tty_getCurrentTTY()->id) {
		video_write(tty->terminal, tty->offset + 1);
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
    file_descriptor_entry* files = scheduler_getCurrentProcess()->fd_table;
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

int sysClose(char* fileName, int oflags, int cflags) {
	file_descriptor_entry* files = scheduler_getCurrentProcess()->fd_table;
	for (int i = 0; i < MAX_FILES_PER_PROCESS; ++i) {
		if (strcmp(files[i].name, fileName) == 0) {
			log(L_DEBUG, "closing file: %s", fileName);
			if (scheduler_getCurrentProcess()->waitingFlags == W_FIFO) {
				scheduler_getCurrentProcess()->status = READY;
			}
		}
	}
	return 0;
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

PRIVATE int isTTY(int fd) {
	return 0 <= fd && fd < MAX_TTYs;
}
