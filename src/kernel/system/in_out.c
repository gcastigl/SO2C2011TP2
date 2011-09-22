#include "int80.h"
#include "in_out.h"

buffer_t * in_out_vector[10]; 

void READ_INTERRUPT_handler(registers_t regs){
	int i;
	buffer_t * buff=in_out_vector[regs.ebx];
	for(i=0;i<regs.edx && buff->start!=buff->end;i++){
			*((char*)(regs.ecx+i))=buff->array[buff->start];
			buff->start=(buff->start+1)%buff->size;
	}
	if(i<regs.edx){
		*((char*)(regs.ecx+i))='\0';
	}
}

void WRITE_INTERRUPT_handler(registers_t regs){
	int i;
	int tmp;
	buffer_t * buff=in_out_vector[regs.ebx];
	tmp=(buff->end+1)%buff->size;
	for(i=0;i<regs.edx && tmp!=buff->start;i++,tmp=(buff->end+1)%buff->size){
		buff->array[buff->end]=*((char*)(regs.ecx+i));
		buff->end=tmp;
	}
}

void add_in_out(int n, buffer_t * buff){
	in_out_vector[n]=buff;
}


init_in_out(){
	register_functionality(3,READ_INTERRUPT_handler);
	register_functionality(4,WRITE_INTERRUPT_handler);
}
