#include <lib/unistd.h>
// FIXME: This does not go here
#include <fs/fs.h>
#include <tty.h>

int open(const char *path, int oflags, ...) {
    int create_flags = -1;
    if (oflags & O_CREAT) {
        va_list ap;
        va_start(ap, oflags);
        create_flags = va_arg(ap, int);
    }
    int fd = (int)_SysCall(SYSTEM_OPEN, path, oflags, create_flags);
    return fd;
}

int creat(const char *path, int mode) { 		// idem as open
	return 0;
}

int close(int fildes) {
	return 0;
}

/*
 *	fd = File descriptor referring to the open file.
 *	buffer = Storage location for data.
 *	count = Maximum number of bytes.
 */
u32int read(int fd, void * buffer, u32int count) {
	_SysCall(SYSTEM_READ,fd, buffer, count);
	return count;
}

/*
 *	fd = File descriptor of file into which data is written.
 *	buffer = Data to be written..
 *	count = Maximum number of bytes.
 */
u32int write(int fd, const void * buffer, u32int count) {
	_SysCall(SYSTEM_WRITE, fd, buffer, count);
	return count;
}

int lseek(int fildes, int offset, int oflag) {
	return 0;
}

// FIXME: system call missing here
int mkfifo(char *name, int mode) {
    fs_node_t current, *fifo;
    fs_getFsNode(&current, tty_getCurrentTTY()->currDirectory);
    fifo = finddir_fs(&current, name);
	if (fifo == NULL) {
		createdir_fs(&current, name, FS_PIPE);
	}
	return 0;
}

