#include <main.h>

u32int initial_esp; // New global variable.

int kmain(struct multiboot *mboot_ptr, u32int initial_stack) {
    port_serial_init();
    _cli();
    initial_esp = initial_stack;
    init_descriptor_tables();
    _mascaraPIC1(0xFC);
    _mascaraPIC2(0xFF);
    paging_init();
    keyboard_init();
    video_init();
    setFD(STD_OUT);
    fs_init();
    group_init();
    user_init();
    scheduler_init(true);
    _initTTCounter();
    startTTYs();
    _sti();
    log(L_INFO, "system initialized");
    while (1)
        ;
    return 0;
}
