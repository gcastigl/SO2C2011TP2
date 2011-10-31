#ifndef MATH_H
#define MATH_H

/* pow
*
* Recibe como parametros:
* - Base
* - Potencia
*
* Devuelve base ^ power
**/
int pow(int base, int power);

/* user_init
*
* Devuelve un número pseudo-aleatorio
**/
int random();

/* setSeed
*
* Recibe como parametros:
* - Semilla
*
* Setea la semilla para el generador de número pseudo-aleatorios
**/
void setSeed(long s);

/* abs
*
* Recibe como parametros:
* - Número
*
* Devuelve el valor absoluto de n
**/
int abs(int n);

#endif
