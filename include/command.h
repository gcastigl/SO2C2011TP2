#ifndef COMMAND_H
#define COMMAND_H

#include <defs.h>
#include <shell.h>
#include <tty.h>
#include <asm/libasm.h>
#include <lib/stdlib.h>
#include <driver/video.h>
#include <lib/math.h>
#include <interrupts/interrupts.h>
#include <process/process.h>
#include <lib/unistd.h>
#include <lib/file.h>

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
/* echo_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Muestra en la pantalla el texto ingresado como parámetro:
    $echo Esto es una prueba
    Esto es una prueba
**/
int echo_cmd(int argc, char **argv);

/* restart_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Reinicia la computadora
**/
int restart_cmd(int argc, char **argv);

/* clear_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Limpia la pantalla
**/
int clear_cmd(int argc, char **argv);

/* echo_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Muestra los comandos disponíbles
**/
int help_cmd(int argc, char **argv);

/* getCPUspeed_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Calcula la velocidad de la CPU
**/
int getCPUspeed_cmd(int argc, char **argv);

/* random_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Devuelve un número pseudo-aleatorio
**/
int random_cmd(int argc, char **argv);

/* format
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Formatea el dísco rígido
**/
int format(int argc, char **argv);

/* logout
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Cierra la sesión del usuario actual
**/
int logout(int argc, char **argv);

/* top_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Muestra los procesos corriendo
**/
int top_cmd(int argc, char**argv);

/* kill_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Mata al proceso indicado por pid
**/
int kill_cmd(int argc, char**argv);

/* eternumWhile_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Función que corre en un cíclo infinito, sirve para testear kill
**/
int eternumWhile_cmd(int argc, char** argv);

/* diskManagerTest
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Función de prueba para el dísco rígido
**/
int diskManagerTest(int argc, char **argv);

/* diskManagerTest
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Función de prueba para el dísco rígido
**/
int diskManagerTest2(int argc, char **argv);

/* shell_useradd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Agrega un usuario
**/
int shell_useradd(int argc, char **argv);

/* shell_userdel
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Borra un usuario
**/
int shell_userdel(int argc, char **argv);

/* shell_userlist
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Muestra todos los usuarios
**/
int shell_userlist(int argc, char **argv);

/* shell_usersetgid
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Setea el group id del usuario
**/
int shell_usersetgid(int argc, char **argv);

/* shell_groupadd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Agrega un grupo
**/
int shell_groupadd(int argc, char **argv);

/* shell_groupdel
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Borra un grupo
**/
int shell_groupdel(int argc, char **argv);

/* shell_grouplist
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Lista todos los grupos
**/
int shell_grouplist(int argc, char **argv);

int cacheStatus_cmd(int argc, char **argv);

/* pfiles
 * Lista los archivos por el proceso actual
 */
int pfiles(int argc, char **argv);


int cd_cmd(int argc, char **argv);
int ls_cmd(int argc, char **argv);
int mkdir_cmd(int argc, char **argv);
int pwd_cmd(int argc, char **argv);
int touch_cmd(int argc, char **argv);
int cat_cmd(int argc, char **argv);
int ln_cmd(int argc, char **argv);
int rm_cmd(int argc, char **argv);
int mkfifo_cmd(int argc, char **argv);
int cp_cmd(int argc, char **argv);
int mv_cmd(int argc, char **argv);

int chmod_cmd(int argc, char **argv);
int chown_cmd(int argc, char **argv);
int chgrp_cmd(int argc, char **argv);

int nice_cmd(int argc, char **argv);

// TESTS

int pageFault_cmd(int argc, char **argv);

/* scanf_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Función de prueba para scanf
**/
int scanf_cmd(int argc, char **argv);

/* printf_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Función de prueba para printf
**/
int printf_cmd(int argc, char **argv);

/* pipeTest_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Función de prueba para pipes
**/
int pipeTest_cmd(int argc, char **argv);

/* getchar_cmd
*
* Recibe como parametros:
* - Cantidad de Argumentos
* - Argumentos
*
* Función de prueba para getchar
**/
int getchar_cmd(int argc, char **argv);

/** infRecursion_cmd
 * 
 * Función de prueba para una recursión infinita. Chequeo de Stack dinámico.
 **/
int infRecursion_cmd(int argc, char **argv);

int testExpandStack_cmd(int argc, char **argv);

void testHeap();

int DMTest2(int argc, char **argv);

#endif
