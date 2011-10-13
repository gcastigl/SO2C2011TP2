#include <main.h>

int currentPID = 0;
static int nextPID = 1;
PROCESS process[MAX_PROCESSES];

u32int initial_esp; // New global variable.

int kmain(struct multiboot *mboot_ptr, u32int initial_stack) {
	_cli();
        initial_esp = initial_stack;
		init_descriptor_tables();
		_mascaraPIC1(0xFC);
		_mascaraPIC2(0xFF);
		initialize_paging();
		keyboard_init();
		video_init();
		setFD(STD_OUT);
		shell_init();
		tty_init();
		fs_init();
        /*initScheduler();
        createProcessAt("idle", idle, 0, 0, (char **)0, 0x4000, 0, BACKGROUND, READY); //Crea el proceso Init, su pid va a ser el 1
        */
		_initTTCounter();
	_sti();
	while (1) {
		shell_update();
	}
	return 0;
}

int getNextPID() {
	return nextPID++;
}

PROCESS *getProcessByPID(int pid) {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (process[i].freeSlot != 0) {
            if (process[i].pid == pid) {
                return &process[i];
            }
        }
    }
    return NULL;
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

