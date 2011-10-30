#include <lib/unistd.h>

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

int read(int fildes, void* buffer, int n) {
	return 0;
}

int write(int fildes, const void * buffer, int n) {
	return 0;
}

int lseek(int fildes, int offset, int oflag) {
	return 0;
}

int mkfifo(char *name, int mode) {
    fs_createFile(tty_getCurrentTTY()->currDirectory, name, FS_PIPE);
    return 0;
}