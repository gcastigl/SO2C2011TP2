#include <main.h>

u32int initial_esp; // New global variable.

int kmain(struct multiboot *mboot_ptr, u32int initial_stack) {
    _cli();
        initial_esp = initial_stack;
        init_descriptor_tables();
        _mascaraPIC1(0xFC);
        _mascaraPIC2(0xFF);
        paging_init(mboot_ptr->mem_upper * 1024);
        port_serial_init();
        keyboard_init();
        video_init();
        setFD(0);
        fs_init();
        group_init();
        user_init();
        process_initialize();
        scheduler_init(true);
        _initTTCounter();
        startTTYs();
    _sti();
    while (1);
    return 0;
}

u32int yield(void) {
    _SysCall(SYSTEM_YIELD);
    return 0;
}
