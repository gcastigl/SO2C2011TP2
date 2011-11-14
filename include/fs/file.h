#ifndef FILE_H
#define FILE_H

#include <defs.h>
#include <lib/stdio.h>
#include <fs/fs.h>
#include <tty.h>
#include <access/permission.h>
#include <fs/diskManager.h>
#include <lib/stdlib.h>


typedef struct {
    char        name[MAX_NAME_LENGTH];
    u8int       permissions;
    u8int       inode;
    fs_node_t   *node;
    u8int       flags;
    u32int      offset;
} FILE;

#define FD_OFFSET (MAX_TTYs + 4)

#define O_RDONLY    0x0001
#define O_WRONLY    0x0010
#define O_APPEND    0x0020
#define O_CREAT     0x0040
#define O_EXCL      0x0080
#define O_NOFOLLOW  0x0100
#define O_TRUNC     0x0200

#define O_RDWR      O_RDONLY | O_WRONLY

#endif
