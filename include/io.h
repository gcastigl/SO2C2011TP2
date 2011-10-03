#ifndef IO_H
#define IO_H

#include <defs.h>
#include <driver/keyboard.h>
#include <driver/video.h>
#include <fs.h>
#include <tty.h>

void sysRead(int fd, void * buffer, u32int count);

void sysWrite(int fd, void * buffer, u32int count);

#endif

