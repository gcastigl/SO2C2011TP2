#include <main.h>

u32int initial_esp; // New global variable.

PRIVATE void testSegmentationFault();
PRIVATE void testHeap();

int kmain(struct multiboot *mboot_ptr, u32int initial_stack) {
    _cli();
    initial_esp = initial_stack;
    init_descriptor_tables();
    _mascaraPIC1(0xFC);
    _mascaraPIC2(0xFF);
    //testSegmentationFault();
    //testHeap();
    paging_init(0x1000000);
    //testHeap();
    //testSegmentationFault();
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

PRIVATE void testSegmentationFault() {
    u32int *ptr = (u32int*)0xA0000000;
    u32int do_page_fault = *ptr;
    do_page_fault = 1;
}

PRIVATE void testHeap() {
    int a = kmalloc(8);
    int b = kmalloc(8);
    log(L_INFO, "kmalloc: 0x%x", a);
    log(L_INFO, "kmalloc: 0x%x", b);
    kfree((void*)a);
    kfree((void*)b);
}
