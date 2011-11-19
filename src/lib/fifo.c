#include <lib/fifo.h>
#include <lib/file.h>

#define MAX_FIFOS	1

PRIVATE fifo_t fifos[MAX_FIFOS];

PRIVATE int pipe_index(u32int inode);

PRIVATE int signalWriters(fifo_t* f);
/*
PRIVATE boolean _checkReadingProcess(int inode, PROCESS** list, int currentPID);
PRIVATE boolean _checkWritingProcess(int inode, PROCESS** list, int currentPID);
PRIVATE boolean _checkConditionProcess(int inode, PROCESS** list, int currentPID, int condition);
*/

void fifo_init() {
	for (int i = 0; i < MAX_FIFOS; ++i) {
		sem_init(&fifos[i].readers, 0);
		sem_init(&fifos[i].writers, 0);
		sem_init(&fifos[i].lock, 1);
		list_init(&fifos[i].waitingQueue, "WaitingQue");
		fifos[i].inode = -1;
		fifos[i].offset = 0;
		fifos[i].lenght = 0;
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
	log(L_DEBUG, "< [Entering read]\n");

	sem_signal(&fifo->readers);
	if (fifo->writers.count == 0) {
		list_add(&fifo->waitingQueue, scheduler_getCurrentProcess());
		scheduler_setStatus(scheduler_currentPID(), BLOCKED);
		yield();
	}
	signalWriters(fifo);
	//printf("[read] Lock value: %d\n", fifo->lock.count);
	sem_wait(&fifo->lock);
	//printf("READ: %s - %d bytes / offset: %d", fifo->buff, fifo->offset, size);
	log(L_DEBUG, "<-- I'm reading the buffer %d bytes from offset %d....\n", size, fifo->offset);
	memcpy(buffer, fifo->buff + fifo->offset, size);
	fifo->offset += size;
	sem_signal(&fifo->lock);
	sem_wait(&fifo->readers);
	yield();
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
	log(L_DEBUG, "> [Entering write]\n");

	sem_signal(&fifo->writers);
	sem_wait(&fifo->lock);
	if (fifo->readers.count == 0) {
		// Add current to the waiting list and block it
		list_add(&fifo->waitingQueue, scheduler_getCurrentProcess());
		scheduler_setStatus(scheduler_currentPID(), BLOCKED);
		yield();
	}
	memcpy(fifo->buff, buffer, size);
	fifo->lenght = size;
	log(L_DEBUG, "--> I'm writing to the buffer %d bytes....\n", size);
	sem_signal(&fifo->lock);
	while(fifo->offset < size)
		;
	sem_wait(&fifo->writers);
	fifo->offset = 0;
	return size;
}

PRIVATE int signalWriters(fifo_t* f) {
	_cli();
	int signaled = 0;
	for (int i = 0; i < list_size(&f->waitingQueue); i++) {
		PROCESS* p = list_get(&f->waitingQueue, i);
		for (int j = 0; j < MAX_FILES_PER_PROCESS; ++j) {
			if (p->fd_table[j].mask != 0 && (p->fd_table[j].mode&&O_WRONLY) != 0) {
				scheduler_setStatus(p->pid, READY);
				PROCESS* p = list_remove(&f->waitingQueue, i);
				log(L_DEBUG, "%d has been removed from the queue / size: %d", p->pid, f->waitingQueue.size);
				signaled++;
				break;
			}
		}
	}
	_sti();
	return signaled;
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


*/
