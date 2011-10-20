#ifndef SHELL_H
#define SHELL_H

#include <tty.h>
#include <lib/string.h>
#include <command.h>
#include <session.h>
#include <driver/keyboard.h>
#include <process/process.h>
#include <fs/file.h>

#define WELCOME_MSG	"\t\t\t\t\tWelcome to GAT O.S.\n\tFor a list of availables commands \
please type in help\n"

#define CMD_FAIL	0
#define CMD_OK		1

#define MAX_ARG_DIM	5

typedef struct {
	char* name;
	char* help;
	int (*func)(int argc, char **argv);
} cmd_table_entry;

/*
 *	Al ser invoacada se fija si se presiono una tecla (buffer del teclado
 *	no vacio) y en cuyo caso, se guaradra en el buffer de la shell y si es
 *	necesario, manda la ejecucion de un programa.
 */
void shell_update(int index);

void shell_cleanScreen();

cmd_table_entry* shell_getCmdsTable();

int shell_getCmdIndex(char * cmdName);

#endif

