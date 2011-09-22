/**
 * screen.h | Interfaz para manejo de pantalla.
 */
#include "../system/common.h"

#ifndef SCREEN_H
#define SCREEN_H
/**
 * Escribe un caracter en pantalla.
 * @param char c: el caracter a escribir.
 *  Los siguientes ANSI scape Characters fueron implementados:   
 *       
 *     Esc[2J           Borra la pantalla y mueve el cursor a (line 0, column 0). 
 *     Esc[#;#;...m 	Cambia el modo de graficos segun los siguientes atributos:
 * 
 * Text attributes
 * 0	All attributes off
 * 1	Bold on
 * 4	Underscore (on monochrome display adapter only)
 * 5	Blink on
 *  
 * Foreground colors    Background colors
 * 30	Black           40	Black
 * 31	Red             41	Red
 * 32	Green           42	Green
 * 33	Yellow          43	Yellow
 * 34	Blue            44      Blue
 * 35	Magenta         45      Magenta
 * 36	Cyan            46      Cyan
 * 37	White           47      White 
 * 
 * Ej: Esc[34;47m (azul en fondo blanco)
 **/
void screen_put(char c);


#endif
