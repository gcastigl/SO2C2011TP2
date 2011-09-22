#include "../include/kasm.h"
#include "kernel/driver/screen.h"
#include "kernel/system/idt.h"
#include "kernel/driver/keyboard.h"
#include "kernel/system/keyboardlisteners.h"

/**********************************************
kmain()
Punto de entrada de codigo C.
*************************************************/

kmain()
{
    // Init system.
    init_descriptor_tables();
    init_int80();
    init_in_out();
    init_keyboard();
    init_timer_tick();
    init_screen();

    // Start Shell
    shell_start();
}
