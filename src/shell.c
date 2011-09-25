#include "../include/shell.h"

void excecuteCmd(char* buffer);
int parse_cmd(char* buffer);
char** getArguments(char* buffer, int* argc);
void checkReset();
void checkTTY();
void prntWelcomeMsg();
void printShellLabel();

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
	{"setAppearance",	HELP_SETAPPEARANCE, setAppearance_cmd},
	{"getchar", "Funcion para la catedra para testeo de getchar\n", getchar_cmd},
	{"printf", "Funcion para la catedra para testeo de printf\n", printf_cmd},
	{"scanf", "Funcion para la catedra para testeo de scanf\n", scanf_cmd},
	{"", "", NULL}
};


void shell_init() {
	cleanBuffer();
	prntWelcomeMsg();
	newTTY = -1;
	sprintf(shell_text, "@tty%d >", tty_getCurrent() + 1);
	printShellLabel();
}

/*
 *	Al ser invoacada se fija si se presiono una tecla (buffer del teclado
 *	no vacio) y en cuyo caso, se guaradra en el buffer de la shell y si es
 *	necesario, manda la ejecucion de un programa.
 */
void updateShell() {
	checkReset();
	checkTTY();
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

/*
	Verifica si en el buffer recibido existe un comando valido, y de ser asi,
	lo invoca. 
	Imprime en pantalla un cartel de error si no se pudo enontrar un comando
	 valido que concuerde con lo leido.
*/
void excecuteCmd(char* buffer) {
	int cmdLen, argc;
	char ** arguments;
	
	TTY* currTty = tty_getCurrentTTY();
	char format = video_getFormattedColor(currTty->fgColor, currTty->bgColor); // current format backup
	currTty->fgColor = DARK_GRAY;

	int cmdIndex = parse_cmd(buffer);
	if (cmdIndex != -1) {
		cmdLen = strlen(cmd_table[cmdIndex].name);
		arguments = getArguments(buffer + cmdLen, &argc);
		cmd_table[cmdIndex].func(argc, arguments);
		printf("\n");
	} else if(buffer[0]!='\0') {
		printf("\n\tUnknown command\n");
	}
	currTty->fgColor = video_getFGcolor(format);	// restore format
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

cmd_table_entry* getCmdsTable() {
	return cmd_table;
}

int getCmdIndex(char * cmdName) {
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
			sprintf(shell_text, "@tty%d >", newTTY + 1);
			if (tty_getCurrentTTY()->offset == 0) printShellLabel();
		}
		newTTY = -1;
	}
}

void printShellLabel() {
	TTY* currTty = tty_getCurrentTTY();
	char format = video_getFormattedColor(currTty->fgColor, currTty->bgColor); // current format backup
	currTty->bgColor = BLACK;
	currTty->fgColor = CYAN;
	printf(shell_text);
	currTty->bgColor = video_getBGcolor(format);	// restore format
	currTty->fgColor = video_getFGcolor(format);
}

void prntWelcomeMsg() {
	TTY* currTty = tty_getCurrentTTY();
	char format = video_getFormattedColor(currTty->fgColor, currTty->bgColor); // current format backup
	currTty->bgColor = GREEN;
	currTty->fgColor = BLUE;
	printf(WELCOME_MSG);
	currTty->bgColor = video_getBGcolor(format);	// restore format
	currTty->fgColor = video_getFGcolor(format);
	printf("\n\n");
}

