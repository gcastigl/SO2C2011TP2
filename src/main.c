#include <main.h>

static int currentPID = 0;
static int nextPID = 1;

u32int initial_esp; // New global variable.

int kmain(struct multiboot *mboot_ptr, u32int initial_stack) {
	_cli();
        initial_esp = initial_stack;
		init_descriptor_tables();
		_mascaraPIC1(0xFC);
		_mascaraPIC2(0xFF);
		initialise_paging();
		keyboard_init();
		video_init();
		setFD(STD_OUT);
		shell_init();
		fs_init();
		tty_init();
		_initTTCounter();
	_sti();
	while (1) {
		shell_update();
	}
	return 0;
}

int getCurrPID() {
	return currentPID;
}

int getNextPID() {
	return nextPID++;
}

/*
 *	fd = File descriptor referring to the open file.
 *	buffer = Storage location for data.
 *	count = Maximum number of bytes.
 */
u32int __read(int fd, void * buffer, u32int count) {
	_SysCall(SYSTEM_READ,fd, buffer, count);
	return count;
}

/*
 *	fd = File descriptor of file into which data is written.
 *	buffer = Data to be written..
 *	count = Maximum number of bytes.
 */
u32int __write(int fd, const void * buffer, u32int count) {
	_SysCall(SYSTEM_WRITE, fd, buffer, count);
	return count;
}

