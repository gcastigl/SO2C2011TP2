#ifndef FILE_H
#define FILE_H

#include <defs.h>
#include <lib/stdio.h>
#include <fs/fs.h>
#include <tty.h>
#include <access/permission.h>

#define MAX_OPEN_FILES          50

typedef struct {
    char        name[MAX_NAME_SIZE];
    u8int       permissions;
    u8int       inode;
    fs_node_t   *node;
    u8int       flags;
    u32int      offset;
} FILE;

FILE openFiles[MAX_OPEN_FILES];

#define FD_OFFSET (MAX_TTYs + 3)

#define O_RDONLY    1
#define O_WRONLY    2
#define O_RDWR      3

#define O_APPEND    (1 << 0)
#define O_CREAT     (1 << 1)
#define O_EXCL      (1 << 2)
#define O_NOFOLLOW  (1 << 3)
#define O_TRUNC     (1 << 4)

int cd(int argc, char **argv);

int ls(int argc, char **argv);

int mkdir(int argc, char **argv);

int pwd(int argc, char **argv);

int touch(int argc, char **argv);

int cat(int argc, char **argv);


#endif
