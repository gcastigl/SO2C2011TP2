#include <lib/fifo.h>
#include <fs/file.h>

//PRIVATE char queue[200];

boolean noReadingProcess(int inode);

u32int fifo_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
	if (FILE_TYPE(node->mask) != FS_PIPE) {
		log(L_ERROR, "Trying to read from a file that is not a pipe!");
		errno = E_INVALID_ARG;
		return 0;
	}
	log(L_DEBUG, "reading from a pipe!!!");
	/*if (fifo->size < bytes) {
		bytes = fifo->size;
	}
	read_fs(fifo, fifo->size - bytes, bytes, msg);*/
	return 0;
}
u32int fifo_write(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
	_sti();
	if (FILE_TYPE(node->mask) != FS_PIPE) {
		log(L_ERROR, "Trying to write to a file that is not a pipe!");
		errno = E_INVALID_ARG;
		return 0;
	}
	log(L_DEBUG, "waiting for a process to read from the pipe...");
	while (noReadingProcess(node->inode))
		;	// Wait
	log(L_DEBUG, "writing to a pipe!!!");
	//write_fs(fifo, fifo->size, bytes, msg);
	return 0;
}

boolean noReadingProcess(int inode) {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		/*if (process[i].status == BLOCKED) {
			for (int j = 0; j < MAX_FILES_PER_PROCESS; j++) {
				printf("waiting %d, inode: %d\n", i, process[i].fd_table[j].inode);
				if (process[i].fd_table[j].inode == inode && process[i].fd_table[j].mode == O_RDONLY) {
					return false;
				}
			}
		}*/
	}
	return true;
}
