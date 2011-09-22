Driver ATA:
	http://wiki.osdev.org/ATA_PIO_Mode

cosas a tener:

struct proceso {
	int pid
	char* stack   (contexto del programa)
	enum state state
	int esp
	
}

				-------------
				|			|
				|			|
				|			|
registros	->	|-----------| (si el proceso no esat en ejecucion)
				|			|
				|			|
				-------------
				|puntero a func| que se encarga de dejar el proceso en terminado
				
	
proceso1(running)				




proceso2 (ready)

stack:
	regs
	flags
	eip
	
	
	
Para cambiar de contexto, simplemete cambiamos el esp. (y se backapea todo)

--->> 
hay que tratar que todas las interrupciones entren ys algan siempre por el mismo lugar del kernel!!!




pseudocodigo de cambio de contexto


int 80:

.asm
guardo_contexto_stack(pushad, ...)
guardo_esp(push_esp, call func)
pido_esp(call get_esp, mov esp, eax)    <---- context switch!
levantar_contexto(popad, ...)

sched.c
guardar_esp(int esp)




como creo un proceso?
creo un stack, 
le pongo un esp
le ponga un estado que sea valido(es decir que funcione con las funciones de guardar y levantar contextos)
los registros que importan son: eip




