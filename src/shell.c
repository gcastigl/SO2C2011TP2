#include <shell.h>
#include <tty.h>
#include <lib/string.h>
#include <command.h>
#include <session.h>
#include <driver/keyboard.h>
#include <process/process.h>
#include <lib/file.h>

#define DEFAULT_COLOR_FG	WHITE
#define DEFAULT_COLOR_BG	BLACK

// "user"@tty"n" "currPath" >
#define SHELL_PROMPT	"%s@tty%d %s > "
#define UPDATE_PROMPT(tty)	sprintf(shell_text, SHELL_PROMPT, session_getName(), tty + 1, \
                tty_getTTY(tty)->currPath);

int shell_readCommand(TTY* tty);
char** getArguments(char* buffer, int* argc, int *background);
void cleanBuffer(TTY* tty);
void printShellLabel(TTY* tty);

static char shell_text[15];
static char* argv[MAX_ARG_DIM];
static char shellBuffer[TTY_BUFFER_SIZE];

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
    {"cd", 				"switch current directory: cd DIRECTORY", cd_cmd},
    {"ls", 				"List information about the FILEs (the current directory by default). Use -a modifier to show hidden files", ls_cmd},
    {"mkdir", 			"Create the DIRECTORY(ies), if they do not already exist: mkdir DIRECTORY", mkdir_cmd},
    {"mkfifo", 			"Create named pipes (FIFOs) with the given NAMEs: mkfifo FIFO", mkfifo_cmd},
    {"pwd",				"Show current user path", pwd_cmd},
    {"touch",			"Creates a new empty file: touch FILE CONTENT", touch_cmd},
    {"cat",				"Shows the content for a specified filename: cat FILE", cat_cmd},
    {"ln",              "make links between files: ln SOURCE TARGET", ln_cmd},
    {"rm",              "Removes the file specified by the parameter: rm TARGET", rm_cmd},
    {"cp",              "Copy SOURCE to DEST: cp SOURCE DEST", cp_cmd},
    {"mv",              "Rename SOURCE to DEST: mv SOURCE DEST", mv_cmd},
    {"logout", 			"Logout current user\n", logout},
    {"top", 			"Shows the current running processes", top_cmd},
    {"kill", 			"Kills process with given PID: kill PID", kill_cmd},
    {"infWhile", 		"Process that loops till the end of time!", eternumWhile_cmd},
    {"useradd", 		"Adds a user: useradd USERNAME PASSWORD", shell_useradd},
    {"userdel", 		"Deletes a user: userdel USERNAME", shell_userdel},
    {"userlist", 		"Lists all available users: userlist", shell_userlist},
    {"usersetgid", 		"Set's user group id: usersetgid USERNAME GID", shell_usersetgid},
    {"groupadd", 		"Adds a group: groupadd GROUP PASSWORD", shell_groupadd},
    {"groupdel", 		"Deletes a group: groupdel GROUP", shell_groupdel},
    {"grouplist", 		"Lists all available groups: grouplist", shell_grouplist},
    {"chmod", 			"Changes file permissions: chmod OCTALMODE FILE", chmod_cmd},
    {"chown", 			"Changes file owner: chown USERNAME FILE", chown_cmd},
    {"chgrp", 			"Changes file group: chgrp GROUPNAME FILE", chgrp_cmd},
    {"cache", 			"Prints the fs cache status", cacheStatus_cmd},
    {"random", 			HELP_RANDOM, random_cmd},
    {"sudo",            "sudo", sudo_cmd},
    {"nice",            "Run COMMAND with an adjusted niceness, which affects process scheduling: nice PID PRIORITY", nice_cmd},
    // TESTS ====================================================================
    {"pfiles", 			"Prints the files opened by the specified process: pfiles PID", pfiles},
    {"DMtest", 			"Disk manager test, creates a very big file", diskManagerTest},
    {"pitest", 			"Pipe test: pitest r/w", pipeTest_cmd},
    {"pageFault", 		"Page fault test", pageFault_cmd},
    {"infRec", 		    "Infinite recursion test", infRecursion_cmd},
    {"expStack",        "Stack expand test", testExpandStack_cmd},
    {"getchar",         "Getchar test", getchar_cmd},
    {"pInfo",   "Shows all running processes stack and paging information", processInfo_cmd},
    {"opp", "Tries to access another process page", tryToAccessAnotherProcess_cmd},
    {"", "", NULL}
};

void shell_update() {
    TTY* tty = tty_getTTY(scheduler_getCurrentProcess()->tty);
    if (!session_isLoggedIn()) {
        session_login();
    }
    int cmd = shell_readCommand(tty);
    if (cmd != -1) {
        tty_setFormatToCurrTTY(video_getFormattedColor(LIGHT_BLUE, BLACK));
        excecuteCmd(cmd, tty->buffer);
    }
    cleanBuffer(tty);
}

int shell_readCommand(TTY* tty) {
    boolean validCmd;
    int cmdIndex;
    do {
        tty_setFormatToCurrTTY(video_getFormattedColor(DEFAULT_COLOR_FG, DEFAULT_COLOR_BG)); // restore old format
        printShellLabel(tty);
        gets_max(tty->buffer, TTY_BUFFER_SIZE);
        cmdIndex = parse_cmd(tty->buffer);
        if (cmdIndex != -1) {
            validCmd = true;
        } else if (tty->buffer[0] != '\0') {
            tty_setFormatToCurrTTY(video_getFormattedColor(RED, BLACK));
            printf("\n\tUnknown command\n");
        }
    } while(!validCmd);
    return cmdIndex;
}


void shell_cleanScreen() {
    TTY* tty = tty_getCurrentTTY();
    tty_clean(tty);
    video_setOffset(0);
    video_write(tty->screen, TOTAL_VIDEO_SIZE);
}

void excecuteCmd(int cmd, char* buffer) {
    log(L_DEBUG, "buffer: %s", buffer);
    int cmdLen, argc;
    char **argv;
    if (cmd != -1) {
        cmdLen = strlen(cmd_table[cmd].name);
        int background;
        argv = getArguments(buffer + cmdLen, &argc, &background);
        // log(L_DEBUG, "Running %s in %s", cmd_table[cmd].name, (background == true ? "background" : "foreground"));
        scheduler_schedule(cmd_table[cmd].name, cmd_table[cmd].func, argc, argv, DEFAULT_STACK_SIZE, tty_getCurrent(),
            (background == true ? BACKGROUND : FOREGROUND), READY, NORMAL);
    }
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
    tty->screenOffset = 0;
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

void printShellLabel(TTY* tty) {
    UPDATE_PROMPT(tty->id);
    tty_setFormat(tty, video_getFormattedColor(CYAN, BLACK));
    printf(shell_text);
    tty_setFormat(tty, video_getFormattedColor(DEFAULT_COLOR_FG, DEFAULT_COLOR_BG));
}


