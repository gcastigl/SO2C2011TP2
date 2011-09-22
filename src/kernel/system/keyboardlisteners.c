#include "keyboardlisteners.h"


key_listener KEY_LISTENERS[4][MAX_SCAN_CODE];
key_listener * actual_key_listeners;

int mode0(){
	actual_key_listeners=KEY_LISTENERS[0];
	return 0;
}

int mode1(){
	actual_key_listeners=KEY_LISTENERS[1];
	return 0;
}

int mode2(){
	actual_key_listeners=KEY_LISTENERS[2];
	return 0;
}

int mode3(){
	actual_key_listeners=KEY_LISTENERS[3];
	return 0;
}


int print(){
	return 1;
}

int no_print(){
	return 0;
}

int activate(int scan_code){
	return actual_key_listeners[scan_code]();
}

void add_key_listener(int mode, int scan_code, key_listener listener){
	if(mode!=-1){
		KEY_LISTENERS[mode][scan_code]=listener;
	}
	else{
		KEY_LISTENERS[0][scan_code]=listener;
		KEY_LISTENERS[1][scan_code]=listener;
		KEY_LISTENERS[2][scan_code]=listener;
		KEY_LISTENERS[3][scan_code]=listener;
	}
}

void init_key_listeners(){
        int i;
        for(i=0;i<58;i++){
        	add_key_listener(-1,i, print);
        }
        for(i=58;i<MAX_SCAN_CODE;i++){
        	add_key_listener(-1,i, no_print);
        }
        
        add_key_listener(0, CTRL_KEY_PRESED_SCAN_CODE, mode1);
        add_key_listener(1, CTRL_KEY_RELESED_SCAN_CODE, mode0);
        
        add_key_listener(1, ALT_KEY_PRESED_SCAN_CODE, mode3);
        add_key_listener(3, ALT_KEY_RELESED_SCAN_CODE, mode1);        
        
        add_key_listener(0, ALT_KEY_PRESED_SCAN_CODE, mode2);
        add_key_listener(2, ALT_KEY_RELESED_SCAN_CODE, mode0);
        
        add_key_listener(2, CTRL_KEY_PRESED_SCAN_CODE, mode3);
        add_key_listener(3, CTRL_KEY_RELESED_SCAN_CODE, mode2);
        
        actual_key_listeners=KEY_LISTENERS[0];
}
