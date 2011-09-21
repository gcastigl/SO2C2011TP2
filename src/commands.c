#include "../include/defs.h"
#include "../include/kc.h"

//Comando Kill que mata tareas.
void kill(char* buffer)
{
int i, contador, aux, longitud, PID, prior;
char temp[10];
//char* video= (char*)0xB8000;
void* nada;

	contador=lenspace(buffer);
	contador++;
	aux = contador;
	contador += strlen2(buffer + aux);
	longitud = contador - aux;
	memcpy2(temp, &buffer[aux], longitud);
	temp[longitud] = '\0';
	PID = atoi2(temp);
	Destroy(PID);
	
}

//Comando Nice que cambia prioridades
void nice(char* buffer)
{
int contador, aux, longitud, PID, prior;
char temp[10];
void* nada;
char prueba[111];
	if (lenspace(buffer)==strlen2(buffer))
	{
		puterr("Error de Uso: nice <pid> <prioridad>");
		return;
	}
	contador=lenspace(buffer);
	contador++;
	aux = contador;
	contador += lenspace(buffer + aux);
	longitud = contador - aux;
	memcpy2(temp, &buffer[aux], longitud);
	temp[longitud] = '\0';
	PID = atoi2(temp);
	if ((nada = GetProcessByPID(PID)) == NULL)
	{
		if((nada = GetProcessByName(temp)) == NULL)
		{
			puterr("Error en el PID");
			return;
		}
	}
	while (contador < 80 && buffer[contador] == ' ')
		contador++;
	aux = contador;
	contador += strlen2(&buffer[aux]);
	longitud = contador - aux;
	memcpy2(temp, &buffer[aux], longitud);
	temp[longitud] = '\0';
	prior = atoi2(temp);
	puterr("Nivel Alterado");
	if (prior >= 0 && prior <= MAX_PRIORITY)
		((PROCESS*)nada)->priority = prior;
	else
	{
		puterr("Error en la prioridad");
		return;
	}
}
