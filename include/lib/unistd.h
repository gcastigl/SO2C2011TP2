#ifndef UNINSTD_H_
#define UNINSTD_H_

#include <defs.h>
#include <asm/libasm.h>
#include <stdarg.h>
#include <fs/file.h>

int open(const char *path, int oflag, ...);

int creat(const char *path, int mode);	// idem as open

int close(int fildes);

int read(int fildes, void* buffer, int n);

int write(int fildes, const void * buffer, int n);

int lseek(int fildes, int offset, int oflag);

#endif
