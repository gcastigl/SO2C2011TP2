#include "isr.h"
#include "int80.h"

#define SUB_FUNC_VEC_SIZE 10



int80_t sub_funcs_vec[SUB_FUNC_VEC_SIZE];


void register_functionality(uint8_t n, int80_t func) {
	if(n<SUB_FUNC_VEC_SIZE){
	    sub_funcs_vec[n] = func;
	}
}

void int80_handler(registers_t regs){
	if(regs.eax<SUB_FUNC_VEC_SIZE){
		sub_funcs_vec[regs.eax](regs);
	}
}

void nofunc(registers_t regs){
}



void init_int80(){
	int i;
	for(i=0;i<SUB_FUNC_VEC_SIZE;i++){
		sub_funcs_vec[i]=nofunc;
	}
	register_interrupt_handler(0X80,int80_handler);
}
