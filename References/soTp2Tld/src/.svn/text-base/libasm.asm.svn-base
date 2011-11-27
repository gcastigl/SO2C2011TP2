GLOBAL  _read_msw,_lidt
GLOBAL  _mascaraPIC1,_mascaraPIC2,_Cli,_Sti
GLOBAL  _debug


EXTERN  int_80
EXTERN  int_09
EXTERN  int_85
EXTERN	putc
EXTERN  getNextProcess

;funciones que manejan las exepciones del micro
EXTERN div0
EXTERN bounds
EXTERN opCode
EXTERN seg_no_present
EXTERN stack_fault
EXTERN gral_protection
EXTERN pageFault
EXTERN	execp01hand
EXTERN	execp02hand
EXTERN	execp03hand
EXTERN	execp04hand
EXTERN	execp07hand
EXTERN	execp08hand
EXTERN	execp09hand
EXTERN	execp10hand
EXTERN	execp15hand
EXTERN	execp16hand
EXTERN	execp17hand
EXTERN	execp18hand
EXTERN	stackTop


GLOBAL  _ArmaStackFrame
GLOBAL  _int_08_hand
GLOBAL  _int_80_hand
GLOBAL  _int_09_hand
GLOBAL  _int_85_hand
GLOBAL  _read
GLOBAL  _write
GLOBAL  _in
GLOBAL  _out
GLOBAL  _switchProcess
GLOBAL  _kill
GLOBAL  _exec
GLOBAL  _sleep
GLOBAL  _myEBP


; Rutinas de atecion de excepciones
GLOBAL _div_0__hand ; division por cero
GLOBAL _bounds_hand ; bounds error
GLOBAL _opCode_hand ; operation code invalido
GLOBAL _snPres_hand ; segmento no presente
GLOBAL _stackf_hand ; saturacion de segmento de pila
GLOBAL _gralPf_hand ; error de proteccion general
GLOBAL _pFault_hand ; segmentation fault!

GLOBAL  _execp01hand
GLOBAL	_execp02hand
GLOBAL	_execp03hand
GLOBAL	_execp04hand
GLOBAL	_execp07hand
GLOBAL	_execp08hand
GLOBAL	_execp09hand
GLOBAL	_execp10hand
GLOBAL	_execp15hand
GLOBAL	_execp16hand
GLOBAL	_execp17hand
GLOBAL	_execp18hand
GLOBAL  _writeStackReg
GLOBAL	_readEBP
GLOBAL	_readESP



executeCode    equ 1
killCode       equ 2
sleepCode      equ 3



SECTION .text


_Cli:
	cli			; limpia flag de interrupciones
	ret

_Sti:

	sti			; habilita interrupciones por flag
	ret

_mascaraPIC1:			; Escribe mascara del PIC 1
	push    ebp
        mov     ebp, esp
        mov     ax, [ss:ebp+8]  ; ax = mascara de 16 bits
        out	21h,al
        pop     ebp
        retn

_mascaraPIC2:			; Escribe mascara del PIC 2
	push    ebp
        mov     ebp, esp
        mov     ax, [ss:ebp+8]  ; ax = mascara de 16 bits
        out	0A1h,al
        pop     ebp
        retn

_read_msw:
        smsw    ax		; Obtiene la Machine Status Word
        retn


_lidt:								; Carga el IDTR
        push    ebp
        mov     ebp, esp
        push    ebx
        mov     ebx, [ss: ebp + 6]	; ds:bx = puntero a IDTR 
		rol		ebx,16		    	
		lidt    [ds: ebx]       	; carga IDTR
        pop     ebx
        pop     ebp
        retn



; Debug para el BOCHS, detiene la ejecucion Para continuar colocar en el BOCHSDBG: set $eax=0
_debug:
        push    bp
        mov     bp, sp
        push	ax
vuelve:	mov     ax, 1
        cmp		ax, 0
		jne		vuelve
		pop		ax
		pop     bp
		retn

;************************************************************
; FUNCIONES NUESTRAS 
;************************************************************
;------------------------------------------------------------

_write:
	push    ebp
	mov     ebp, esp
	pusha

	mov 	eax,0   	; write
	mov 	ebx,[ebp+8] 	; fd 	
	mov 	ecx,[ebp+12] 	; buffer
	mov 	edx,[ebp+16] 	; cantidad
	int 	80h;
	
	popa
	mov esp,ebp
	pop ebp

	ret


_writeStackReg:
	mov 	eax,[esp] 	; direccion de retorno
	mov 	ebp,[esp+8] 	; ebp
	mov 	esp,[esp+4] 	; esp
	
	push eax  ; argumento 1
	push eax  ; argumento 2
	push eax  ; direccion de retorno
	ret


_readEBP:
	mov eax,ebp	
	ret

_readESP:
	mov eax,esp	
	ret



_read:
	push    ebp
	mov     ebp, esp
	pusha
	
	mov 	eax, 1		 	; read
	mov 	ebx,[ebp+8] 	; fd
	mov 	ecx,[ebp+12] 	; buffer
	mov 	edx,[ebp+16]  	; cantidad
	int 	80h
	popa
	mov esp,ebp
	pop ebp

	ret

_switchProcess:
    push ebp
    mov ebp, esp
    int 8h
    mov esp, ebp
    pop ebp
    ret

_ArmaStackFrame:
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
    
_myEBP:
	mov eax,ebp
	ret
    

_int_08_hand:			; Handler de INT 8 ( Timer tick)

    cli
    pusha

    push esp

    call getNextProcess

    pop esp

    mov esp, eax		; cambia el stack pointer

    mov al, 20h
    out 20h, al
    popa
    sti

    iret

_int_09_hand:			; Handler de INT 9 ( teclado)
	cli
	push    ds
	push    es			; Se salvan los registros
	pusha
	
	call int_09
	
	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al

	popa
	pop     es
	pop     ds
	sti
	iret


;EAX: 0-write, 1-read
;EBX: fd
;ECX: buffer
;EDX: cantidad

_int_80_hand:	; Handler de INT 80
	cli
	push    ds
	push    es  ; Se salvan los registros
	pusha       ; Carga de DS y ES con el valor del selector
	
	push edx	; cantidad de caracteres
	push ecx	; buffer
	push ebx	; fd
	push eax	; system call
	call int_80
	
	pop eax
	pop ebx
	pop ecx
	pop edx

	popa
	pop     es
	pop     ds
	sti	
	iret

_int_85_hand:
    cli                     
    push    ds
    push    es
    pusha

    push    edx             ; Parametro 3
    push    ecx             ; Parametro 2
    push    ebx             ; Parametro 1
    push    eax             ; Codigo 
    call    int_85

    pop     eax             
    pop     eax
    pop     eax
    pop     eax

    popa
    pop     es
    pop     ds
    sti
    iret

_flushTLB: ; actualiza el cache del micro para refrescar la table de directorios
	push    ebp
	mov     ebp, esp
	pusha

	mov eax,cr3
	mov cr3, eax

	popa
	mov esp,ebp
	pop ebp

	ret

; ***********************************
; RUTINAS DE ATENCION DE EXCEPCIONES
;************************************

_div_0__hand:				; "Divide by zero"
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
	
	iret

_bounds_hand:			        ; Handler de excepci�n "BOUND range exceeded"
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
	iret

_opCode_hand:                            ; "Invalid opcode"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax

	call opCode

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	pop ax
	popa
	pop ax
	pop ax
	iret

_snPres_hand: 			; "Segment not present"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax

	call seg_no_present

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop ax
	pop ax
	iret



_stackf_hand:				; Handler de excepci�n "Stack exception"


        push ds
	push es			; Se salvan los registros
	pusha			; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h		; a utilizar.
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call stack_fault

	mov al,20h		; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
        sti
	jmp $
	iret

; 	push ds
; 	push es				; Se salvan los registros
; 	pusha				; Carga de DS y ES con el valor del selector
; 	pushf
; 	mov ax, 10h			; a utilizar.
; 	mov ds, ax
; 	mov es, ax      
;             
; 	call stack_fault
; 
; 	mov al,20h			; Envio de EOI generico al PIC
; 	out 20h,al
; 	popf
; 	popa
; 	pop ax
; 	pop ax
; 	iret

_gralPf_hand:			; "General protection exception"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax

	call gral_protection

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop ax
	pop ax
	iret


_pFault_hand:			; Page Fault Handler
        push ds
	push es			; Se salvan los registros
	pusha			; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h		; a utilizar.
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call pageFault

	mov al,20h		; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
        sti
	jmp $
	iret


_execp01hand:				; "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax          
        
	call execp01hand

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	
	iret


_execp02hand:				; "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax          
        
	call execp02hand

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	
	iret


_execp03hand:				; "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax          
        
	call execp03hand

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	
	iret


_execp04hand:				; "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax          
        
	call execp04hand

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	
	iret

_execp07hand:				; "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax          
        
	call execp07hand

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	
	iret


_execp08hand:				; "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax          
        
	call execp08hand

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	
	iret

_execp09hand:				; "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax          
        
	call execp09hand

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	
	iret


_execp10hand:				; "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax          
        
	call execp10hand

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	
	iret

_execp15hand:				; "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax          
        
	call execp15hand

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	
	iret


_execp16hand:				; "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax          
        
	call execp16hand

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	
	iret


_execp17hand:				; "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax          
        
	call execp17hand

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	
	iret


_execp18hand:				; "Divide by zero"
	push ds
	push es				; Se salvan los registros
	pusha				; Carga de DS y ES con el valor del selector
	pushf
	mov ax, 10h			; a utilizar.
	mov ds, ax
	mov es, ax          
        
	call execp18hand

	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	popf
	popa
	pop es
	pop ds
	
	iret


;lee 1, 2 o 4 bytes
_in:
	push    ebp
	mov     ebp, esp
	pusha

	mov	ebx,[ebp+8]; buffer	
	mov	edx,[ebp+12] ;address
	mov	ecx,[ebp+16] ;count
	
	cmp ecx,4
	jnz iLee8b
	
	in 	eax,dx
	mov 	[ebx],eax
	jmp iFin
iLee8b: cmp ecx,2
	jnz iLee4b
	in 	ax,dx;
	mov 	[ebx],ax
	jmp iFin

iLee4b	in 	al,dx;
	mov 	[ebx],al
	
iFin:	popa
	mov esp,ebp
	pop ebp
	ret



;escribe 1, 2 o 4 bytes
_out:

	push    ebp
	mov     ebp, esp
	pusha

	mov	edx,[ebp+8]; adress	
	mov 	ebx,[ebp+12] ;buffer
	mov	ecx,[ebp+16] ;count
	cmp ecx,4
	
	jnz oLee8b

	mov	eax,[ebx]
	out	dx,eax
	jmp oFin

oLee8b: cmp ecx,2
	jnz oLee4b 
	mov	ax,[ebx]
	out	dx,ax
	jmp oFin

oLee4b:	mov	al,[ebx]
	out	dx,al
	
oFin:	popa
	mov esp,ebp
	pop ebp
	ret




_kill:
    push    ebp             ; arma stack frame
    mov     ebp, esp
    pusha
    mov     eax, killCode   ;codigo kill
    mov     ebx, [ebp+8]    ; pid

    int     085h
    popa
    mov     esp, ebp        ; destruye stack frame
    pop     ebp
    ret

_exec:
    push    ebp
    mov     ebp, esp
    
    pusha
    mov     eax, executeCode
    mov     ebx, [ebp+8]    ;nombre
    mov     ecx, [ebp+12]   ;funcion
    mov     edx, [ebp+16]   ;background
    
    int     085h

    popa
    mov     esp, ebp
    pop     ebp
    ret

_sleep:
    push    ebp
    mov     ebp, esp

    pusha
    mov     eax, sleepCode
    mov     ebx, [ebp+8]    ;cantidad de cilos de timertick

    
    int     085h

    popa
    mov     esp, ebp
    pop     ebp
    ret