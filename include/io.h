#ifndef IO_H
#define IO_H

#include <defs.h>
#include <driver/keyboard.h>
#include <driver/video.h>
#include <tty.h>

void sysRead(int fd, void * buffer, u32int count);

void sysWrite(int fd, void * buffer, u32int count);

static inline
unsigned char inb( unsigned short port ) {
    unsigned char ret;
    asm volatile( "inb %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

static inline
void outb( unsigned short port, unsigned char val ) {
    asm volatile( "outb %0, %1"
                  : : "a"(val), "Nd"(port) );
}

#endif

