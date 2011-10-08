GLOBAL copy_page_physical
GLOBAL read_eip
GLOBAL loadStackFrame
GLOBAL switchProcess
EXTERN getNextTask

read_eip:
	mov eax, [esp]
	ret

copy_page_physical:
	push ebx
	pushf
	
	cli
	
	mov ebx, [esp + 12]
	mov ecx, [esp + 16]
	mov edx, cr0
	and edx, 0x7FFFFFFF
	mov cr0, edx
	
	mov edx, 1024
.loop:
	mov eax, [ebx]
	mov [ecx], eax
	add ebx, 4
	add ecx, 4
	dec edx
	jnz .loop
	
	mov edx, cr0
	or 	edx, 0x80000000
	mov cr0, edx
	
	popf
	pop ebx
	ret

switchProcess:
	cli
	pusha
    push esp
    call getNextTask
    pop esp
    mov esp, eax		; cambia el stack pointer
	popa
	sti
	ret
	

loadStackFrame:	
	; flags

	pushfd
	push ebp
	mov ebp, esp
	; sp por parametros
	mov eax, [ebp+16]
	; usar ese stack
	mov esp, eax
	; funcion a ejecutar
	mov eax, [ebp+12]
	; funcion cementerio

	mov edx, [ebp+24]
	; push de argv
	push edx

	mov edx, [ebp+20]
	; push de argc
	push edx

	mov edx, [ebp+28]
	; push de 'la nueva ret'
	push edx

	; seteo el bit de habilitar interrupciones
	mov ecx, 512
	; push flags (solo me interesa habilitar int)
	push ecx
	; para iret
	push cs
	push eax
	; resto de los registros

	mov eax, ebp
	mov ebp, 0
	pusha
	mov ebp, eax

	; devolver el nuevo esp
	mov eax, esp
	; y restaurar el original
	mov esp, ebp
	; restaurar flags y ebp
	pop ebp
	popfd

	retn