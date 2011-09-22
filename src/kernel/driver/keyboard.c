#include "../system/isr.h"
#include "../system/in_out.h"
#include "../system/keyboardlisteners.h"

#define KEYBOARD 0x60
#define BUFFER_SIZE 100

#define LSHIFT_KEY_PRESED_SCAN_CODE 42
#define LSHIFT_KEY_RELESED_SCAN_CODE 170
#define RSHIFT_KEY_PRESED_SCAN_CODE 54
#define RSHIFT_KEY_RELESED_SCAN_CODE 182

#define BLOQ_MAYUS_SCAN_CODE 58

char array[BUFFER_SIZE];

buffer_t stdin;

char * actual_scan_code_table;

char SCAN_CODE_TABLE[60]={'\x1B','@','1','2','3','4','5','6','7','8','9','0','-','+','\x08','\t','q','w','e','r','t','y','u','i','o','p','{','}','\n','@','a','s','d','f','g','h','j','k','l','+','@','@','@','@','z','x','c','v', 'b','n','m',',','.','@','@','@','@',' '};
char SHIFT_SCAN_CODE_TABLE[60]={'\x1B','@','!','"','#','$','¿','&','?','/','(',')','_', '=','\x08','\t','Q','W','E','R','T','Y','U','I','O','P','[',']','\n','@','A','S','D','F','G','H','J','K','L','\\','"','@','@','@','Z','X','C','V','B','N','M',';',':','/','@','@','@',' '};

int shift;
int bloq_mayusc;

int bloq_mayusc_unpresed();
int bloq_mayusc_presed();

int bloq_mayusc_presed(){
	bloq_mayusc=0;
        add_key_listener(-1,BLOQ_MAYUS_SCAN_CODE, bloq_mayusc_unpresed);
	return 0;
}

int bloq_mayusc_unpresed(){
	bloq_mayusc=1;
        add_key_listener(-1,BLOQ_MAYUS_SCAN_CODE, bloq_mayusc_presed);
	return 0;
}

int shift_presed(){
	shift++;
	actual_scan_code_table=SHIFT_SCAN_CODE_TABLE;
	return 0;
}

int shift_relesed(){
	shift--;
	if(shift==0){
		actual_scan_code_table=SCAN_CODE_TABLE;
	}
	return 0;
}

void IRQ1_handler(registers_t reg){
	int tmp;
	int i=inb(KEYBOARD);
	if(activate(i)){
		tmp=(stdin.end+1)%stdin.size;
		if(tmp!=stdin.start){
			char c=actual_scan_code_table[i];
			if(bloq_mayusc){
				if(c>='a' && c<='z'){
					c=c+'A'-'a';
				}else if(c>='A' && c<='Z'){
					c=c+'a'-'A';
				}
			}
			stdin.array[stdin.end]=c;
			stdin.end=tmp;
		} else {
                    //TODO: beep
		}
	}
}

static void reset(){
	outb(0x64,0xFE);
}

static int cnrl_alt_supr_manager(){
	reset();
	return 0;
}


void init_keyboard(){
	register_interrupt_handler(IRQ1,IRQ1_handler);
	stdin.start=stdin.end=0;
	stdin.array=array;
	stdin.size=BUFFER_SIZE;
	add_in_out(0,&stdin);
	actual_scan_code_table=SCAN_CODE_TABLE;
        bloq_mayusc=0;
        init_key_listeners();
        add_key_listener(-1,LSHIFT_KEY_PRESED_SCAN_CODE, shift_presed);
        add_key_listener(-1,RSHIFT_KEY_PRESED_SCAN_CODE, shift_presed);
        add_key_listener(-1,LSHIFT_KEY_RELESED_SCAN_CODE, shift_relesed);
        add_key_listener(-1,RSHIFT_KEY_RELESED_SCAN_CODE, shift_relesed);
        add_key_listener(-1,BLOQ_MAYUS_SCAN_CODE, bloq_mayusc_unpresed);

        add_key_listener(3, 83, cnrl_alt_supr_manager);
}
