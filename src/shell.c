#include <shell.h>

#define DEFAULT_COLOR_FG	WHITE
#define DEFAULT_COLOR_BG	BLACK

// "user"@tty"n" "currPath" >
#define SHELL_PROMPT	"%s@tty%d %s > "
#define UPDATE_PROMPT	sprintf(shell_text, SHELL_PROMPT, session_getName(), tty_getCurrent() + 1, \
				tty_getCurrentTTY()->currPath);

void excecuteCmd(char* buffer);
int parse_cmd(char* buffer);
char** getArguments(char* buffer, int* argc, int *background);
void cleanBuffer(TTY* tty);
void printShellLabel(int index);
//FIXME: checkReset & checkTTY could be in a separated keyboard manager file
void checkReset();
void checkTTY();

static char shell_text[15];
static char* argv[MAX_ARG_DIM];
static char shellBuffer[BUFFER_SIZE];
static int newTTY = -1;
extern PUBLIC int activeTTYs;

/*
 *	Tabla de comandos disponibles al usuario en esta shell
 */
cmd_table_entry cmd_table[] = {
	// FIXME: remove all this annoying hard coded text and send it to a help text file...
	{"help", 			HELP_HELP, help_cmd},
	{"restart", 		HELP_RESTART, restart_cmd},
	{"clear", 			HELP_CLEAR, clear_cmd},
	{"getCPUspeed", 	HELP_GETCPUSPEED, getCPUspeed_cmd},
	{"echo", 			HELP_ECHO, echo_cmd},
	{"cd", 				"switch current directory", cd_cmd},
	{"ls", 				"List information about the FILEs (the current directory by default).", ls_cmd},
	{"mkdir", 			"Create the DIRECTORY(ies), if they do not already exist.", mkdir_cmd},
	{"mkfifo", 			"Create named pipes (FIFOs) with the given NAMEs.", mkfifo_cmd},
	{"pwd",				"Show current user path", pwd_cmd},
	{"touch",			"Creates a new empty file", touch_cmd},
	{"cat",				"Shows the content for a specified filename", cat_cmd},
    {"ln",              "make links between files", ln_cmd},
    {"rm",              "Removes the file specified by the parameter", rm_cmd},
	{"logout", 			"Logout current user\n", logout},
	{"top", 			"Shows the current running processes", top_cmd},
	{"kill", 			"Kills process with given PID", kill_cmd},
	{"infWhile", 		"Process that loops till the end of time!", eternumWhile_cmd},
	{"useradd", 		"usage: useradd USERNAME PASSWORD", shell_useradd},
	{"userdel", 		"usage: userdel USERNAME", shell_userdel},
	{"userlist", 		"usage: userlist", shell_userlist},
	{"usersetgid", 		"usage: usersetgid USERNAME GID", shell_usersetgid},
	{"groupadd", 		"usage: groupadd GROUP PASSWORD", shell_groupadd},
	{"groupdel", 		"usage: groupdel GROUP", shell_groupdel},
	{"grouplist", 		"usage: grouplist", shell_grouplist},
	{"chmod", 			"usage: OCTALMODE FILE", chmod_cmd},
	{"chown", 			"usage: USERNAME FILE", chown_cmd},
	{"chgrp", 			"usage: GROUPNAME FILE", chgrp_cmd},
	{"cache", 			"prints the fs cache status", cacheStatus_cmd},
	{"random", 			HELP_RANDOM, random_cmd},
	// TESTS ====================================================================
	{"pfiles", 			"prints the files opened by the current process", pfiles},
	{"DMtest", 			"disk manager test", diskManagerTest},
	{"pitest", 			"pipes test", pipeTest_cmd},
	{"pageFault", 		"pageFault test", pageFault_cmd},
	{"", "", NULL}
};

void shell_update(int index) {
	checkReset();
	checkTTY();
	TTY* tty = tty_getCurrentTTY();
	if (tty->id != index) {
		return;
	}
	if (!session_isLoggedIn()) {
		session_login();
		printShellLabel(index);
	}
	if (tty->offset == 0) {
		printShellLabel(index);
	}
	if (bufferIsEmpty()) {
		return;
	}
	char c = getKeyFromBuffer();
	if (tty->bufferOffset >= BUFFER_SIZE) {
		return;
	}
	if (c == '\n') {
		printf("\n");
		excecuteCmd(tty->buffer);
		printShellLabel(index);
		cleanBuffer(tty);
	} else if (c == '\b') {
		if (tty->bufferOffset > 0) {
			printf("%c", c);
			tty->buffer[--tty->bufferOffset] = '\0';
		}
	} else {
		printf("%c", c);
		tty->buffer[tty->bufferOffset] = c;
		tty->buffer[++tty->bufferOffset] = '\0';
	}
}

void shell_cleanScreen() {
	TTY* tty = tty_getCurrentTTY();
	tty_clean(tty);
	video_setOffset(0);
	video_write(tty->terminal, TOTAL_VIDEO_SIZE);
}

/*
 *	Verifica si en el buffer recibido existe un comando valido, y de ser asi,
 *	lo invoca.
 *	Imprime en pantalla un cartel de error si no se pudo enontrar un comando
 *	 valido que concuerde con lo leido.
 */
void excecuteCmd(char* buffer) {
	int cmdLen, argc;
	char **argv;
	tty_setFormatToCurrTTY(video_getFormattedColor(LIGHT_BLUE, BLACK));
	int cmdIndex = parse_cmd(buffer);
	if (cmdIndex != -1) {
		cmdLen = strlen(cmd_table[cmdIndex].name);
        int background;
		argv = getArguments(buffer + cmdLen, &argc, &background);
        log(L_DEBUG, "Running %s in %s", cmd_table[cmdIndex].name, (background == true ? "background" : "foreground"));
		process_create(cmd_table[cmdIndex].name, cmd_table[cmdIndex].func, argc, argv, DEFAULT_STACK_SIZE, &clean, tty_getCurrent(),
            (background == true ? BACKGROUND : FOREGROUND), READY, NORMAL);
	} else if(buffer[0]!='\0') {
		tty_setFormatToCurrTTY(video_getFormattedColor(RED, BLACK));
		printf("\n\tUnknown command\n");
	}
	tty_setFormatToCurrTTY(video_getFormattedColor(DEFAULT_COLOR_FG, DEFAULT_COLOR_BG)); // restore old format
}


int parse_cmd(char* buffer) {
	int cmdLength = -1, aux;
	int match = -1;
	for(int i = 0; cmd_table[i].func != NULL; i++) {
		if (substr(cmd_table[i].name, buffer)) {
			aux = strlen(cmd_table[i].name);
			if (aux > cmdLength) {
				match = i;
				cmdLength = aux;
			}
		}
	}
	
	if (match == -1) {
		return -1;
	}
	char next = shellBuffer[strlen(cmd_table[match].name)];
	return  (next == ' ' || next == '\0' || next == '&') ? match : -1;
}

/*
	Coloca '\0' en cada espacio para poder usar el buffer como parametros de una 
	llamada a comando
*/
char** getArguments(char* buffer, int* argc, int *background) {
	int i = 0, arg = 0;
    int bg = false;
	while((buffer[i] != '\0') && (arg < MAX_ARG_DIM)) {
		if (buffer[i] == ' ') {
			argv[arg++] = buffer + i + 1;
			buffer[i] = '\0';
		} else if (buffer[i] == '&') {
            buffer[i] = '\0';
            bg = true;
		}
		i++;
	}
    *background = bg;
	*argc = arg;
	return argv;
}

void cleanBuffer(TTY *tty) {
    tty->bufferOffset = 0;
	tty->buffer[0] = '\0';
}

cmd_table_entry* shell_getCmdsTable() {
	return cmd_table;
}

int shell_getCmdIndex(char * cmdName) {
	for(int i=0; cmd_table[i].func != NULL; i++) {
		if (strcmp(cmdName, cmd_table[i].name) == 0) {
			return i;
		}
	}
	return -1;
}

void checkReset() {
	if (IS_CTRL() && IS_ALT() && IS_DEL()) {
		_reset();
	}
}

void checkTTY() {
	if (IS_CTRL() && newTTY == -1) {
		int ttys = (activeTTYs > MAX_TTYs ? MAX_TTYs : activeTTYs);
		for (int i = 0; i < ttys; ++i) {
			if (F_PRESSED(i)) {
				newTTY = i;
				break;
			}
		}
	}
	if (!IS_CTRL() && newTTY != -1) {
		if (newTTY != tty_getCurrent()) { // Do not switch to the same tty!
			tty_setCurrent(newTTY);
		}
		newTTY = -1;
	}
}

void printShellLabel(int index) {
	UPDATE_PROMPT;
	TTY* tty = tty_getTTY(index);
	tty_setFormat(tty, video_getFormattedColor(CYAN, BLACK));
	printf(shell_text);
	tty_setFormat(tty, video_getFormattedColor(DEFAULT_COLOR_FG, DEFAULT_COLOR_BG));
}


