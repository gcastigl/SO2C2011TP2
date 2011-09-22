#ifndef COMMANDS_H
#define COMMANDS_H

typedef int (*main)(int argc,char * argv[]);


struct command_struct {
    char * name;
    main start;
    char * help;
};

typedef struct command_struct command_t;

void add_command(char * name,main function, char* help);
main get_command(char * name);

char * autocomplete(char * name);

#endif //COMMANDS_H
