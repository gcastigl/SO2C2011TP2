#include <lib/stdlib.h>

static int nextfree = 0x300000;

void* malloc(u32int size) {
	void* temp = (void*) nextfree;
	nextfree += size;
	return temp;
}

// Malloc inicializado en 0
void* calloc(u32int size) {
	char* temp;
	int i;
	temp = (char*) malloc(size);
	for(i = 0;i < size; i++)
		temp[i] = 0;
	return (void*) temp;
}

void free(void * pointer) {
	// TODO: implementame!
}


void memcpy(void* to, void* from, u32int count) {
	u32int i;
	char* cfrom = (char *) from;
	char* cto = (char *) to;
	for (i = 0; i < count; i++) {
		cto[i] = cfrom[i];
	}
}

void panic(char* str) {
    TTY* tty = tty_getCurrent();
    tty_clean(tty);
    tty_write(tty, str, strlen(str));
    while(1);
}

