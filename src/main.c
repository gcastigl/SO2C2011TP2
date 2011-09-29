#include <main.h>

static int currentPID = 0;
static int nextPID = 1;

void doubleFlagsFix(double n);

int main(struct multiboot *mboot_ptr) {
	_cli();
		init_descriptor_tables();
		nextPID = 0;
		_mascaraPIC1(0xFC);
		_mascaraPIC2(0xFF);
		doubleFlagsFix(1.1);
		//initialise_paging();
		keyboard_init();
		video_init();
		tty_init();
		setFD(STD_OUT);
		shell_init();
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

void doubleFlagsFix(double n) {
}

