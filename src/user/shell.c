#include "shell.h"
#include "../std/systemcall.h"
#include "../std/stdio.h"
#include "../std/string.h"

#include "commands.h"

#define NULL 0
#define COMAND_LINE_MAX 1000
#define EXIT_SYSTEM -15

#define HISTORY_MAX 20

#define NAME_MAX_LENGTH 50
char name[NAME_MAX_LENGTH] = "unknown";
char * pcname = "itba";

char * strnormalise(char * str) {
    int j, i;
    // cambia enters por espacios
    for (j = 0; str[j] != '\0'; j++) {
        if (str[j] == '\n' || str[j] == '\t') {
            str[j] = ' ';
        }
    }
    // elimina espacios del principio
    while (str[0] == ' ') {
        str = str + 1;
    }
    //elimina espacios del final
    for (i = strlen(str) - 1; i > 0 && str[i] == ' '; i--) {
        str[i] = '\0';
    }
    //elimina espacios repetidos en el medio
    for (j = 0; str[j] != '\0'; j++) {
        if (str[j] == ' ' && str[j + 1] == ' ') {
            strcpy(str + j, str + j + 1);
            j--;
        }
    }
    return str;
}

void printuser() {
    printf("\x1B[36;1m%s@%s:~$ \x1B[0m", name, pcname);
}

int execute(char* comand, int argcant, char * argvec[]) {
    if (comand[0] == '\0') {
        return 0;
    }
    main start = get_command(comand);
    if (start == NULL) {
        printf("invalid comand: %s\n", comand);
        return -1;
    }
    return start(argcant, argvec);
}

int parseline() {
    char c;
    int i = 0;
    char comand_line[COMAND_LINE_MAX];
    while ((c = getchar()) != '\n' && i < COMAND_LINE_MAX - 3) {
        comand_line[i] = c;
        i++;
    }
    if (i >= COMAND_LINE_MAX - 3) {
        while (getchar() != '\n');
        printf("\n");
    }
    comand_line[i] = '\0';
    char* command = strnormalise(comand_line);
    int argcant = 0;
    char * argvec[50];
    int in_quotes = 0;
    for (i = 0; command[i] != '\0'; i++) {
        if (command[i] == ' ' && !in_quotes) {
            command[i] = '\0';
            argvec[argcant] = &command[i + 1];
            argcant++;
        } else if (command[i] == '"') {
            if(!in_quotes) {
                argvec[argcant-1] = &command[i + 1];
            }
            command[i] = '\0';
            in_quotes = !in_quotes;
        }
    }
    return execute(command, argcant, argvec) == EXIT_SYSTEM;
}

int exit_shell(int argc, char* argv[]) {
    clear_shell();
    return EXIT_SYSTEM;
}

int echo_shell(int argc, char* argv[]) {
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    printf("\n");
    return 0;
}

int getCPUspeed_shell(int argc, char* argv[]) {
    unsigned long ips;
    __cpuspeed(&ips);
    //printf("Su procesador esta ejecutando %d instrucciones por segudo.\n",ips);
    printf("The CPU speed is: %d.%d MHz\n", (ips) / (1024 * 1024), ((10 * ips) / (1024 * 1024)) % 10);
    return 0;
}

int clear_shell(int argc, char* argv[]) {
    printf("\x1B[2J");
    return 0;
}

int isodd_shell(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Usage: isodd <number>\n");
        return -1;
    }
    int number;
    sscanf(argv[0], "%d", &number);

    if (number % 2 == 0) {
        printf("The number %d is NOT ODD, its EVEN.", number);
    } else {
        printf("The number %d is ODD", number);
    }
    printf("\n");
    return 0;
}

int help_shell(int argc, char* argv[]) {
    printf("\x1B[33mThese are the commands available: \x1B[0m\n\n");
    command_t *commands = (command_t *)get_command_list();
    int i = 0;
    while (i < get_commands_added()) {
        printf("\x1B[4m%s\x1B[0m\t\t%s\n", commands[i].name, commands[i].help);
        i++;
    }
    printf("\nPress CTRL+ALT+SUPR to reboot the system\n");
    return 0;
}

int rename_shell(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Usage: rename <newname>.\n");
        return -1;
    }
    strncpy(name, argv[0], NAME_MAX_LENGTH);
}

static void test_shell_print_usage(){
    printf("Usage: test <testcase>.\n");
    printf("testcases:\n\tprintf\n\tscanf\n");
}

int test_shell(int argc, char* argv[]) {
    if (argc < 1) {
        test_shell_print_usage();
        return -1;
    }
    int integer;
    unsigned int uinteger;
    char* string;
    char chr;
    
    if (!strcmp(argv[0], "printf")) {
        printf("\x1B[32mPlease verify the OKval is the same as RETval\n\x1B[1mNOTATION: case[OKval]: RETval\x1B[0m");
        printf("\n");
        printf("string[hola mundo]: %s", "hola mundo");
        printf("\n");
        printf("char[c]: %c", 'c');
        printf("\n");
        printf("integer[-123]: %d", -123);
        printf("\n");
        printf("unsigned integer[123]: %u", 123);
        printf("\n");
        printf("hexa[FFFFFFFF]: %x", -1);
        printf("\n");
    } else if (!strcmp(argv[0],"scanf")) {
        printf("\x1B[32mPlease verify the OKval is the same as RETval\n\x1B[1mNOTATION: case[OKval]: RETval\x1B[0m");
        printf("\n");
        sscanf("-123", "%d", &integer);
        printf("integer[-123]: %d", integer);
        printf("\n");
        sscanf("123", "%u", &uinteger);
        printf("unsigned integer[123]: %u", uinteger);
        printf("\n");
        sscanf("hello world scanf", "hello %s scanf", string);
        printf("string[world]: %s", string);
        printf("\n");
        sscanf("c", "%c", &chr);
        printf("char[c]: %c", chr);
    } else {
        test_shell_print_usage();
        return -1;
    }
    printf("\n");
    return 0;
}

void shell_start() {
    int exit = 0;
    add_command("test", test_shell, "test cases for functionality");
    add_command("rename", rename_shell, "changes the name of the user of this pc");
    add_command("echo", echo_shell, "echoes some text, don't forget the quotes (\") if you use spaces");
    add_command("clear", clear_shell, "clears the screen");
    add_command("help", help_shell, "shows help");
    add_command("isodd", isodd_shell, "tells if the number is odd or not");
    add_command("exit", exit_shell, "exits the system.");
    add_command("getCPUspeed", getCPUspeed_shell, "shows actual CPU speed");
    do {
        printf("\x1B[33mHi! Whats your name? \x1B[0m");
        char c = '\0';
        int i = 0;
        while ((c = getchar()) != '\n' && i < NAME_MAX_LENGTH) {
            name[i++] = c;
        }
        name[i] = '\0';
        if (i == NAME_MAX_LENGTH) {
            while (getchar() != '\n');
        }
        printf("\x1B[2J\x1B[33mWelcome to arnix (ARg uNIX) %s!\x1B[0m\n\nYou may type \x1B[1mhelp\x1B[0m for more information\n\n", name);
        while (!exit) {
            printuser();
            exit = parseline();
        }
        exit = 0;
    } while (1);
}



