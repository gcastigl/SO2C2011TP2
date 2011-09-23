#include "../include/shell.h"

void excecuteCmd(char* buffer);
int parse_cmd(char* buffer);
char** getArguments(char* buffer, int* argc);
void prntWelcomeMsg();

char* argv[MAX_ARG_DIM];
int currPos;
char shellBuffer[BUFFER_SIZE];

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
	printf(SHELL_TEXT);
}


/*
 *	Al ser invoacada se fija si se presiono una tecla (buffer del teclado
 *	no vacio) y en cuyo caso, se guaradra en el buffer de la shell y si es
 *	necesario, manda la ejecucion de un programa.
 */
void updateShell() {
	if (IS_CTRL() && IS_ALT() && IS_DEL()) {
		_reset();
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
		printf(SHELL_TEXT);
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
	
	int cmdIndex = parse_cmd(buffer);
	if (cmdIndex != -1) {
		cmdLen = strlen(cmd_table[cmdIndex].name);
		arguments = getArguments(buffer + cmdLen, &argc);
		cmd_table[cmdIndex].func(argc, arguments);
		printf("\n");
	} else if(buffer[0]!='\0') {
		printf("\n\tUnknown command\n");
	}
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

/*Retorna true si s1 es subString de s2*/
int substr(const char * s1, const char *s2) {
	int i = 0, isSubstr = true;
	while(isSubstr && s1[i] != '\0') {
		if(s1[i] != s2[i]) {
			isSubstr = false;
		}
		i++;
	}
	return isSubstr;
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

void prntWelcomeMsg() {
	//TODO: Podria mostrarse un msj con colores y logo ascii eventualmente.
	printf(WELCOME_MSG);
}

