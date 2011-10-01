EXTERN  int_08
EXTERN	int_09
EXTERN	int_80
EXTERN	div0
EXTERN	bounds
EXTERN	gpf
EXTERN	ssf
EXTERN	snp
EXTERN	invop
EXTERN 	pageFault
GLOBAL  _int_08_hand
GLOBAL	_int_09_hand
GLOBAL _int_80_hand
GLOBAL _div0_hand
GLOBAL _bounds_hand
GLOBAL _gpf_hand
GLOBAL _ssf_hand
GLOBAL _snp_hand
GLOBAL _invop_hand
GLOBAL  _pageFault_hand

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

	mov ax, 0x10  ; load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
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
	mov		ax, 10h			; a utilizar.
	mov		ds, ax
	mov		es, ax
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
	mov		ax, 10h			; a utilizar.
	mov		ds, ax
	mov		es, ax
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

_div0_hand:				; Handler de excepxión "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax                  
	call div0                 
	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	jmp $
	iret

_bounds_hand:			; Handler de excepción "BOUND range exceeded"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax
	call bounds
	mov	al,20h			; Envio de EOI generico al PIC
	out	20h,al
	popf
	popa
	pop es
	pop ds
	jmp $
	iret

_gpf_hand:				; Handler de excepción "General protection exception"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov eax, 666666h
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax
	call gpf
	mov	al,20h			; Envio de EOI generico al PIC
	out	20h,al
	popf
	popa
	pop ax
	pop ax
	jmp $
	iret

_ssf_hand:				; Handler de excepción "Stack exception"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax                  
	call ssf
	mov	al,20h			; Envio de EOI generico al PIC
	out	20h,al
	popf
	popa
	pop ax
	pop ax
	jmp $
	iret

_snp_hand:				; Handler de excepción "Segment not present"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax
	call snp
	mov	al,20h			; Envio de EOI generico al PIC
	out	20h,al
	popf
	popa
	pop ax
	pop ax
	jmp $
	iret

_pageFault_hand:				; Handler de excepción "Segment not present"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax
	call pageFault
	mov	al,20h			; Envio de EOI generico al PIC
	out	20h,al
	popf
	popa
	pop ax
	pop ax
	jmp $
	iret

_invop_hand:				; Handler de excepción "Invalid opcode"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax
	call invop
	mov	al,20h			; Envio de EOI generico al PIC
	out	20h,al
	pop ax
	popa
	pop ax
	pop ax
	jmp $
	iret