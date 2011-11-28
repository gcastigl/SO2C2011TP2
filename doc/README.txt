Set up:
    go to your home folder and create / set your .mtoolsrc file to: 
        drive a: file="**path to the folder**/SO2C2011TP2/img/tpe.img" exclusive 


0xC0000000  
    Kernel code, data, bss, etc.
    Probably too much but, hey, we might see a 256MB kernel one day.
0xD0000000  
    Kernel heap
0xE0000000  
    Space reserved for device drivers
0xF0000000
    Some physical memory (useful for video memory access)
    Page directory and page tables of the current process




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




