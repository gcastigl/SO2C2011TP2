EXTERN  int_08
EXTERN	int_09
EXTERN	int_80
GLOBAL  _int_08_hand
GLOBAL	_int_09_hand
GLOBAL _int_80_hand

common ttcounter 4

_int_08_hand:				; Handler de INT 8 ( Timer tick)
	push	ds
	push	es              ; Se salvan los registros
	pusha                   ; Carga de DS y ES con el valor del selector
	
	push eax
	mov eax, [ttcounter]
	inc eax
	mov [ttcounter], eax
	pop eax
	
	mov		ax, 10h			; a utilizar.
	mov		ds, ax
	mov		es, ax
	call	int_08
	mov		al,20h			; Envio de EOI generico al PIC
	out		20h,al
	popa
	pop		es
	pop		ds
	iret

_int_09_hand:				; Handler de INT 9 ( Teclado )
	push	ds
	push	es
	pusha
	call	int_09
	mov		al,20h			; Envio de EOI generico al PIC
	out		20h,al
	popa
	pop		es
	pop		ds
	iret

_int_80_hand:				; Handler de INT 80h
	push ebp
	mov ebp, esp			;StackFrame
	
	push edx
	push ecx
	push ebx
	
	push esp				; Puntero al array de argumentos
	push eax				; Numero de Systemcall
	call int_80
	mov	al,20h			; Envio de EOI generico al PIC
	out	20h,al
	pop eax
	pop esp

	pop ebx
	pop ecx
	pop edx
	
	mov esp, ebp
	pop ebp
	iret