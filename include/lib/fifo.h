#ifndef FIFO_H
#define FIFO_H

#include <defs.h>
#include <fs/fs_API.h>

#include <process/process.h>
#include <lib/stdlib.h>

u32int fifo_read(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

u32int fifo_write(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

#endif
