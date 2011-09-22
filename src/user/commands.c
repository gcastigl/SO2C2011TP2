#include "commands.h"

#include "../std/string.h"

#define NULL 0
#define COMMAND_MAX_CANT 20

command_t command_list[COMMAND_MAX_CANT];
int commands_added=0;

command_t * get_command_list() {
    return command_list;
}

int get_commands_added() {
    return commands_added;
}

void add_command(char * name,main function,char* helpDescription){
	if(commands_added<COMMAND_MAX_CANT){
		command_list[commands_added].name=name;
		command_list[commands_added].start=function;
                command_list[commands_added].help=helpDescription;
		commands_added++;
	}
}

main get_command(char * name){
	int i;
	for(i=0;i<commands_added;i++){
		if(!strcmp(command_list[i].name,name)){
			return command_list[i].start;
		}
	}
	return NULL;
}