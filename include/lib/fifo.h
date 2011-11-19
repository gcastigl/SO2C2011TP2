#ifndef FIFO_H
#define FIFO_H

#include <defs.h>
#include <fs/fs_API.h>
#include <process/scheduler.h>
#include <lib/stdio.h>
#include <lib/stdlib.h>
#include <util/semaphore.h>
#include <util/list.h>

#define PIPE_BUF 200

typedef struct {
	char buff[PIPE_BUF];
	List waitingQueue;
	semaphore_t readers;
	semaphore_t writers;
	semaphore_t lock;
	int inode;
	int offset;
	int lenght;
} fifo_t;

void fifo_init();

u32int fifo_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

u32int fifo_write(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

#endif
