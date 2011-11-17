#include <lib/fifo.h>
#include <lib/file.h>

#define MAX_FIFOS	1

PRIVATE fifo_t fifos[MAX_FIFOS];

PRIVATE int pipe_index(u32int inode);

/*
PRIVATE boolean _checkReadingProcess(int inode, PROCESS** list, int currentPID);
PRIVATE boolean _checkWritingProcess(int inode, PROCESS** list, int currentPID);
PRIVATE boolean _checkConditionProcess(int inode, PROCESS** list, int currentPID, int condition);
PRIVATE int signalWriters(int inode, PROCESS** list, int currentPID);
*/

void fifo_init() {
	for (int i = 0; i < MAX_FIFOS; ++i) {
		sem_init(&fifos[i].readers, 0);
		sem_init(&fifos[i].writers, 0);
		sem_init(&fifos[i].lock, 1);
		fifos[i].inode = -1;
	}
}

PRIVATE int pipe_index(u32int inode) {
	for (int i = 0; i < MAX_FIFOS; ++i) {
		if (fifos[i].inode == inode) {
			return i;
		}
	}
	for (int i = 0; i < MAX_FIFOS; ++i) {
		if (fifos[i].inode == -1) {
			fifos[i].inode = inode;
			return i;
		}
	}
	return -1;
}

u32int fifo_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
	if (FILE_TYPE(node->mask) != FS_PIPE) {
		log(L_ERROR, "Trying to read from a file that is not a pipe!");
		errno = E_INVALID_ARG;
		return 0;
	}
	_sti();
	int index = pipe_index(node->inode);
	if (index == -1) {
		log(L_ERROR, "NO MORE SPACE IN SYSTEM FOR ANOTHER FIFO!");
		return 0;
	}
	fifo_t* fifo = &fifos[index];
	printf("< [Entering read]\n");
	sem_signal(&fifo->readers);
	while(fifo->writers.count == 0)
		;
	sem_wait(&fifo->lock);
	printf("<-- I'm reading the buffer....\n");
	sem_signal(&fifo->lock);
	sem_wait(&fifo->readers);
	return size;
}

// wait until no readers or writers
// access database
// wake up waiting readers or writers
u32int fifo_write(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
	if (FILE_TYPE(node->mask) != FS_PIPE) {
		log(L_ERROR, "Trying to write to a file that is not a pipe!");
		errno = E_INVALID_ARG;
		return 0;
	}
	_sti();
	int index = pipe_index(node->inode);
	if (index == -1) {
		log(L_ERROR, "NO MORE SPACE IN SYSTEM FOR ANOTHER FIFO!");
		return 0;
	}
	fifo_t* fifo = &fifos[index];
	printf("> [Entering write]\n");

	sem_signal(&fifo->writers);
	sem_wait(&fifo->lock);
	while(fifo->readers.count == 0)
		;
	printf("--> I'm writing to the buffer....\n");
	sem_signal(&fifo->lock);
	while(fifo->readers.count != 0)
		;
	sem_wait(&fifo->writers);
	return size;
}
/*
PRIVATE boolean _checkReadingProcess(int inode, PROCESS** list, int currentPID) {
	return _checkConditionProcess(inode, list, currentPID, O_RDONLY);
}

PRIVATE boolean _checkWritingProcess(int inode, PROCESS** list, int currentPID) {
	return _checkConditionProcess(inode, list, currentPID, O_WRONLY);
}

PRIVATE boolean _checkConditionProcess(int inode, PROCESS** list, int currentPID, int condition) {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		PROCESS* p = list[i];
		// p is not me, is waiting for a fifo with inode "inode"
		if (p != NULL && p->pid != currentPID &&
			p->waitingFlags == W_FIFO && p->waitingInfo == inode) {
			for (int j = 0; j < MAX_FILES_PER_PROCESS; ++j) {
				if (p->fd_table[j].mask != 0 && (p->fd_table[j].mode&&condition) != 0) {
					log(L_DEBUG, "inode: %s, %d => %d", p->name, j, p->fd_table[j].inode);
					return true;
				}
			}
		}
	}
	return false;
}

PRIVATE int signalWriters(int inode, PROCESS** list, int currentPID) {
	int signaled = 0;
	for (int i = 0; i < MAX_PROCESSES; i++) {
		PROCESS* p = list[i];
		// p is not me, is waiting for a fifo with inode "inode"
		if (p->pid != currentPID &&
			p->waitingFlags == W_FIFO && p->waitingInfo == inode) {
			for (int j = 0; j < MAX_FILES_PER_PROCESS; ++j) {
				if (p->fd_table[j].mask != 0 && (p->fd_table[j].mode&&O_WRONLY) != 0) {
					log(L_DEBUG, "inode: %s, %d => %d", p->name, j, p->fd_table[j].inode);
					scheduler_setStatus(p->pid, RUNNING);
					signaled++;
				}
			}
		}
	}
	return signaled;
}
*/
