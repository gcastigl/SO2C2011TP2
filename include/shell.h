#ifndef _shell_
#define _shell_

#include "kernel.h"
#include "defs.h"
#include "interrupts.h"
#include "io.h"
#include "stdlib.h"
#include "command.h"

#define WELCOME_MSG	"Welcome to GAT O.S.\n\tFor a list of availables commands \
please type in help\n\n"
#define SHELL_TEXT	"GatOS> "

#define BUFFER_SIZE	1024

#define CMD_FAIL	0
#define CMD_OK		1

#define MAX_ARG_DIM	5

typedef struct cmd_table_entry {
	char* name;
	char* help;
	int (*func)(int argc, char *argv[]);
} cmd_table_entry;

void updateShell();
void cleanBuffer();
cmd_table_entry* getCmdsTable();
int getCmdIndex(char * cmdName);

#endif

