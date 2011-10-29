#include <main.h>

int currentPID = -1;
static int nextPID = 0;
PROCESS process[MAX_PROCESSES];
extern int nf_address;
u32int initial_esp; // New global variable.

int kmain(struct multiboot *mboot_ptr, u32int initial_stack) {
	_cli();
        initial_esp = initial_stack;
		init_descriptor_tables();
		_mascaraPIC1(0xFC);
		_mascaraPIC2(0xFF);
		port_serial_init();
		//initialize_paging();
		keyboard_init();
		video_init();
		group_init();
		user_init();
		setFD(STD_OUT);
		fs_init();
        initScheduler(true);
		_initTTCounter();
        startTTYs();
	_sti();
    while (1);
	return 0;
}

int getNextPID() {
	return nextPID++;
}

PROCESS *getProcessByPID(int pid) {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (process[i].slotStatus != FREE) {
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

u32int yield(void) {
    _SysCall(SYSTEM_YIELD);
    return 0;
}

u32int open(char* path, int oflags, ...) {
    int create_flags = 0;
    if (oflags & O_CREAT) {
        va_list ap;
        va_start(ap, oflags);
        create_flags = va_arg(ap, int);
    }
    _SysCall(SYSTEM_OPEN, path, oflags, create_flags);
    return 0;
}
