#include <lib/unistd.h>

int open(const char *path, int oflag, ...) {

	return 0;
}

int creat(const char *path, mode_t mode) { 		// idem as open
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
