#ifndef COMMAND_H
#define COMMAND_H

#include <defs.h>
#include <asm/libasm.h>
#include <shell.h>
#include <driver/video.h>
#include <lib/math.h>
#include <interrupts/interrupts.h>

#define HELP_HELP			"Shows this help function"

#define HELP_RESTART		"Restarts the OS"

#define HELP_CLEAR			"Clears the screen"

#define HELP_GETCPUSPEED	"Measures the CPU speed and prints out its value."

#define HELP_RANDOM			"Creates a random number from 0 to 100"

#define HELP_ECHO			"[arguments ...]\n\nPrints on screen the recived \
arguments"

#define HELP_SETAPPEARANCE	"[foregroundColor backgroundColor]\n\n Sets the \
foreground and background to the given colors.\nBoth arguments must be a number\
 between 0 and F.\n\nWarning: Using a background with highest bit in 1 will \
 make text blink"

int echo_cmd(int argc, char *argv[]);
int restart_cmd(int argc, char *argv[]);
int clear_cmd(int argc, char *argv[]);
int help_cmd(int argc, char *argv[]);
int setPit_cmd(int argc, char *argv[]);
int resetPit_cmd(int argc, char *argv[]);
int countDown_cmd(int argc, char *argv[]);
int getCPUspeed_cmd(int argc, char *argv[]);
int random_cmd(int argc, char *argv[]);
int setAppearance_cmd(int argc, char *argv[]);
int logout(int argc, char *argv[]);

int getchar_cmd(int argc, char *argv[]);
int scanf_cmd(int argc, char *argv[]);
int printf_cmd(int argc, char *argv[]);

#endif
