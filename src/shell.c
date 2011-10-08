#include <shell.h>

// "user"@tty"n" "currPath" >
#define SHELL_PROMPT	"%s@tty%d %s > "
#define UPDATE_PROMPT	sprintf(shell_text, SHELL_PROMPT, user_getName(), tty_getCurrent() + 1, \
				tty_getCurrentTTY()->currPath);

void excecuteCmd(char* buffer);
int parse_cmd(char* buffer);
char** getArguments(char* buffer, int* argc);
void cleanBuffer();
void printShellLabel();
//FIXME: checkReset & checkTTY could be in a separated keyboard manager file
void checkReset();
void checkTTY();

static char shell_text[15];
static char* argv[MAX_ARG_DIM];
static int currPos;
static char shellBuffer[BUFFER_SIZE];
static int newTTY;

/*
 *	Tabla de comandos disponibles al usuario en esta shell
 */
cmd_table_entry cmd_table[] = {
	{"help", 			HELP_HELP, help_cmd},
	{"restart", 		HELP_RESTART, restart_cmd},
	{"clear", 			HELP_CLEAR, clear_cmd},
	{"getCPUspeed", 	HELP_GETCPUSPEED, getCPUspeed_cmd},
	{"random", 			HELP_RANDOM, random_cmd},
	{"echo", 			HELP_ECHO, echo_cmd},
	//{"setAppearance",	HELP_SETAPPEARANCE, setAppearance_cmd},
	{"cd", 				"switch current directory", cd},
	{"ls", 				"List information about the FILEs (the current directory by default).", ls},
	{"mkdir", 			"Create the DIRECTORY(ies), if they do not already exist.", mkdir},
	{"pwd",				"Show current user path", pwd},
	{"touch",			"Creates a new empty file", touch},
	{"format",			"Formats disk ATA0!", format},
	//{"getchar", "Funcion para la catedra para testeo de getchar\n", getchar_cmd},
	//{"printf", "Funcion para la catedra para testeo de printf\n", printf_cmd},
	//{"scanf", "Funcion para la catedra para testeo de scanf\n", scanf_cmd},
	{"logout", "Logout current user\n", logout},
	{"", "", NULL}
};


void shell_init() {
	cleanBuffer();
	newTTY = -1;
}

void shell_update() {
	checkReset();
	checkTTY();
	if (!user_isLoggedIn()) {
		user_doLogin();
		printShellLabel();
	}
	if (bufferIsEmpty()) {
		return;
	}
	char c = getKeyFromBuffer();
	if (currPos >= BUFFER_SIZE) {
		return;
	}
	if (c == '\n') {
		printf("\n");
		excecuteCmd(shellBuffer);
		printShellLabel();
		cleanBuffer();
	} else if (c == '\b') {
		if (currPos > 0) {
			printf("%c", c);
			currPos--;
			shellBuffer[currPos] = '\0';
		}
	} else {
		printf("%c", c);
		shellBuffer[currPos] = c;
		shellBuffer[currPos + 1] = '\0';
		currPos++;
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
	char ** arguments;

	char oldFormat = tty_getCurrTTYFormat();
	tty_setFormatToCurrTTY(video_getFormattedColor(LIGHT_BLUE, BLACK));

	int cmdIndex = parse_cmd(buffer);
	if (cmdIndex != -1) {
		cmdLen = strlen(cmd_table[cmdIndex].name);
		arguments = getArguments(buffer + cmdLen, &argc);
		cmd_table[cmdIndex].func(argc, arguments);
	} else if(buffer[0]!='\0') {
		tty_setFormatToCurrTTY(video_getFormattedColor(RED, BLACK));
		printf("\n\tUnknown command\n");
	}
	tty_setFormatToCurrTTY(oldFormat); // restore old format
}


int parse_cmd(char* buffer) {
	int i, cmdLength = -1, aux;
	int match = -1;

	for(i = 0; cmd_table[i].func != NULL; i++) {
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
	return  (next == ' ' || next == '\0') ? match : -1;
}

/*
	Coloca '\0' en cada espacio para poder usar el buffer como parametros de una 
	llamada a comando
*/
char** getArguments(char* buffer, int* argc) {
	int i = 0, arg = 0;
	while(buffer[i] != '\0' && arg < MAX_ARG_DIM) {
		if (buffer[i] == ' ') {
			argv[arg++] = buffer + i + 1;
			buffer[i] = '\0';
		}
		i++;
	}
	*argc = arg;
	return argv;
}

void cleanBuffer() {
	currPos = 0;
	shellBuffer[0] = '\0';
}

cmd_table_entry* shell_getCmdsTable() {
	return cmd_table;
}

int shell_getCmdIndex(char * cmdName) {
	int i;
	for( i=0; cmd_table[i].func != NULL; i++) {
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
		int i;
		for (i = 0; i < MAX_TTYs; ++i) {
			if (F_PRESSED(i)) {
				// printf("Se presiono: ctrl + %d\n", fKeys);
				newTTY = i;
				break;
			}
		}
	}
	if (!IS_CTRL() && newTTY != -1) {
		if (newTTY != tty_getCurrent()) { // Do not switch to the same tty!
			tty_setCurrent(newTTY);
			if (tty_getCurrentTTY()->offset == 0) printShellLabel();
		}
		newTTY = -1;
	}
}

void printShellLabel() {
	UPDATE_PROMPT;
	char oldFormat = tty_getCurrTTYFormat();
	tty_setFormatToCurrTTY(video_getFormattedColor(CYAN, BLACK));
	printf(shell_text);
	tty_setFormatToCurrTTY(oldFormat);
}


