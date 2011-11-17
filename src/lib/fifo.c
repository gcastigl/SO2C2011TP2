#include <lib/fifo.h>
#include <lib/file.h>

#define PIPE_BUF 200

PRIVATE char pipe_buffer[PIPE_BUF];
PRIVATE int msgLen = 0;
PRIVATE int inode = -1;
PRIVATE boolean writing = false;

PRIVATE boolean _checkReadingProcess(int inode, PROCESS** list, int currentPID);
PRIVATE boolean _checkWritingProcess(int inode, PROCESS** list, int currentPID);
PRIVATE boolean _checkConditionProcess(int inode, PROCESS** list, int currentPID, int condition);
PRIVATE int signalWriters(int inode, PROCESS** list, int currentPID);

u32int fifo_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
	if (FILE_TYPE(node->mask) != FS_PIPE) {
		log(L_ERROR, "Trying to read from a file that is not a pipe!");
		errno = E_INVALID_ARG;
		return 0;
	}
	PROCESS* me = scheduler_getCurrentProcess();
	int writers = signalWriters(node->inode, scheduler_getAllProcesses(), me->pid);
	if (writers == 0) {
		me->status = BLOCKED;
		me->waitingFlags = W_FIFO;
		me->waitingInfo = node->inode;
		yield();
	}
	log(L_DEBUG, "Waiting for my turn!! - writers: %d / node: %s", writers, node->name);
	while (writing || inode != node->inode) {
		log(L_DEBUG, "%d || %d", writing, inode);
		me->waitingFlags = W_FIFO;
		me->waitingInfo = node->inode;
		yield();	// Wait for my turn...
	}
	if (size > PIPE_BUF) {
		size = PIPE_BUF;
	}
	memcpy(buffer, pipe_buffer + offset, size);
	if (offset >= msgLen) {
		msgLen = 0;
		inode = -1;
	}
	log(L_DEBUG, "READING from a pipe!!! - %d bytes / buff: %c / p: %s / offset: %d / msglen: %d", size, *buffer, pipe_buffer, offset, msgLen);
	return size;
}

u32int fifo_write(fs_node_t *node, u32int offset, u32int size, u8int *buffer) {
	if (FILE_TYPE(node->mask) != FS_PIPE) {
		log(L_ERROR, "Trying to write to a file that is not a pipe!");
		errno = E_INVALID_ARG;
		return 0;
	}
	log(L_DEBUG, "waiting for a process to read from the pipe...");
	PROCESS* me = scheduler_getCurrentProcess();
	if (!_checkReadingProcess(node->inode, scheduler_getAllProcesses(), me->pid)) {	// No process has this file opened for reading
		log(L_DEBUG, "%d has gotten block!", me->pid);
		me->status = BLOCKED;
		me->waitingFlags = W_FIFO;
		me->waitingInfo = node->inode;
		yield();
	}
	log(L_DEBUG, "%d -> Somebody opened the pipe for READING!!");
	while(writing || inode != -1)
		yield();// Wait for my turn
	writing = true;
	if (size > PIPE_BUF) {
		size = PIPE_BUF;
	}
	memcpy(pipe_buffer, buffer, size);

	msgLen = size;
	writing = false;
	inode = node->inode;

	log(L_DEBUG, "Data written to the pipe! : %s / bytes: %d", pipe_buffer, size);
	while(msgLen != 0)
		yield();
	return size;
}

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

