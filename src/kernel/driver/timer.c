#include "../system/isr.h"
#include "../system/int80.h"

#define SUB_FUNC_VEC_SIZE 10

int80_t sub_handler_vec[SUB_FUNC_VEC_SIZE];

int ticks;
int count_ticks;
int sub_func_count;
unsigned long k;

void register_tick_subhandler(int80_t func) {
    if (sub_func_count < SUB_FUNC_VEC_SIZE - 1) {
        sub_handler_vec[sub_func_count] = func;
        sub_func_count++;
    }
}

void IRQ0_handler(registers_t regs) {
    int i;
    if (count_ticks) {
        if (ticks == 0) {
            k = getRDTSC();
        }
        ticks++;
    }
    for (i = 0; i < sub_func_count; i++) {
        sub_handler_vec[i](regs);
    }
}

void cpu_speed(registers_t regs) {
    count_ticks = 1;
    ticks = -1;
    _Sti();
    while (ticks < 30);
    k = getRDTSC() - k;
    _Cli();
    count_ticks = 0;
    *((unsigned long*) regs.ebx) = (k / ticks)*18 + k / (ticks * 5);
}

void init_timer_tick() {
    sub_func_count = 0;
    count_ticks = 0;
    register_interrupt_handler(IRQ0, IRQ0_handler);
    register_functionality(5, cpu_speed);
}



