#include <main.h>

u32int initial_esp; // New global variable.

int kmain(struct multiboot *mboot_ptr, u32int initial_stack) {
    _cli();
    initial_esp = initial_stack;
    init_descriptor_tables();
    _mascaraPIC1(0xFC);
    _mascaraPIC2(0xFF);
    int a = kmalloc(8);
    int b = kmalloc(8);
    log(L_INFO, "kmalloc: 0x%x", a);
    log(L_INFO, "kmalloc: 0x%x", b);
    paging_init(0x1000000);
    int c = kmalloc(8);
    int d = kmalloc(8);
    log(L_INFO, "kmalloc: 0x%x", c);
    log(L_INFO, "kmalloc: 0x%x", d);
    port_serial_init();
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
    while (1)
        ;
    return 0;
}

