#include <lib/fifo.h>
#include <lib/file.h>
#include <util/logger.h>

#define MAX_FIFOS	3

PRIVATE fifo_t fifos[MAX_FIFOS];

PRIVATE int pipe_index(u32int inode);

PRIVATE int signalWriters(fifo_t* f);
PRIVATE int signalReaders(fifo_t* f);
PRIVATE int wakeUpWaiters(List* list, int condition);

void fifo_init() {
	for (int i = 0; i < MAX_FIFOS; ++i) {
		sem_init(&fifos[i].readers, 0);
		sem_init(&fifos[i].writers, 1);
		sem_init(&fifos[i].lock, 1);
		list_init(&fifos[i].waitingReaders, "waitingReaders");
		list_init(&fifos[i].waitingWriters, "waitingWriters");
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
	int index = pipe_index(node->inode);
	if (index == -1) {
		log(L_ERROR, "NO MORE SPACE IN SYSTEM FOR ANOTHER FIFO!");
		return 0;
	}
	fifo_t* fifo = &fifos[index];
	log(L_DEBUG, "< [Entering read]\n");

	sem_signal(&fifo->readers);
	if (sem_value(&fifo->writers) == 1) {       // if no writers, block
        list_add(&fifo->waitingReaders, scheduler_getCurrentProcess());
        scheduler_blockCurrent(W_FIFO);
	}
	signalWriters(fifo);
	sem_wait(&fifo->lock);                      // wait on lock until msg is written

	log(L_DEBUG, "<-- I'm reading the buffer %d bytes from offset %d....\n", size, fifo->offset);
    memcpy(buffer, fifo->buff + fifo->offset, size);

    sem_signal(&fifo->lock);
    sem_wait(&fifo->readers);                   // Decrement readers
    fifo->offset += size;
    yield();
	return size;
}

u32int fifo_write(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
	if (FILE_TYPE(node->mask) != FS_PIPE) {
		log(L_ERROR, "Trying to write to a file that is not a pipe!");
		errno = E_INVALID_ARG;
		return 0;
	}
	int index = pipe_index(node->inode);
	if (index == -1) {
		log(L_ERROR, "NO MORE SPACE IN SYSTEM FOR ANOTHER FIFO!");
		return 0;
	}
	fifo_t* fifo = &fifos[index];
	log(L_DEBUG, "> [Entering write]\n");
    sem_wait(&fifo->writers);               // If there is already a writer, wait until it finishes
    sem_wait(&fifo->lock);                  // Grab buffer lock
    if (sem_value(&fifo->readers) == 0) {   // If there are no readers, wait until there are some
        // Add current to the waiting list and block it
	    list_add(&fifo->waitingWriters, scheduler_getCurrentProcess());
        scheduler_blockCurrent(W_FIFO);
    }
    signalReaders(fifo);
	memcpy(fifo->buff, buffer, size);
	fifo->lenght = size;
	sem_signal(&fifo->lock);                // Release the buffer lock
    while(fifo->offset < size)
        ; // FIXME: The writer should be set to BLOCKED here...
    sem_signal(&fifo->writers);             // notify waiting writers the fifo is ready
    if (fifo->writers.count == 1) {         // If last writer...
        fifo->offset = 0;
        fifo->inode = -1;
    }
	return size;
}

PRIVATE int signalWriters(fifo_t* fifo) {
    return wakeUpWaiters(&fifo->waitingWriters, O_WRONLY);
}

PRIVATE int signalReaders(fifo_t* fifo) {
    return wakeUpWaiters(&fifo->waitingReaders, O_RDONLY);
}

PRIVATE int wakeUpWaiters(List* list, int condition) {
	int signaled = 0;
	for (int i = 0; i < list_size(list); i++) {
		PROCESS* p = list_get(list, i);
		for (int j = 0; j < MAX_FILES_PER_PROCESS; ++j) {
			if (p->fd_table[j].mask != 0 && (p->fd_table[j].mode&&condition) != 0) {
				scheduler_setStatus(p->pid, READY);
				list_remove(list, i);
				log(L_DEBUG, "%d has been removed from the queue / size: %d", p->pid, list->size);
				signaled++;
				break;
			}
		}
	}
	return signaled;
}
