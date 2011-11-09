#include <main.h>

int currentPID = -1;
static int nextPID = 0;
PROCESS process[MAX_PROCESSES];
u32int initial_esp; // New global variable.

int kmain(struct multiboot *mboot_ptr, u32int initial_stack) {
    _cli();
        initial_esp = initial_stack;
        init_descriptor_tables();
        _mascaraPIC1(0xFC);
        _mascaraPIC2(0xFF);
        port_serial_init();
        keyboard_init();
        video_init();
        setFD(STD_OUT);
        fs_init();
        group_init();
        user_init();
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

u32int yield(void) {
    _SysCall(SYSTEM_YIELD);
    return 0;
}
