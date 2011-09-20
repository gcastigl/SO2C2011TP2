GLOBAL read_msw
GLOBAL _lidt
GLOBAL _lgdt
GLOBAL _mascaraPIC1
GLOBAL _mascaraPIC2
GLOBAL _Cli
GLOBAL _Sti
GLOBAL _int8

GLOBAL _debug
GLOBAL _int_08_hand
GLOBAL _int_09_hand
GLOBAL _invop_hand
GLOBAL _ssf_hand
GLOBAL _snp_hand
GLOBAL _div0_hand
GLOBAL _gpf_hand
GLOBAL _bounds_hand
GLOBAL _getgdt
GLOBAL _hacersnp
GLOBAL _hacerbounds
GLOBAL _hacergpf
GLOBAL _hacerssf
GLOBAL _hacerinvop
GLOBAL beep
GLOBAL _movercursor
GLOBAL _ponerss
GLOBAL _ponercs
GLOBAL _ponerds
GLOBAL _ejecutar
GLOBAL _debugBuenaOnda
GLOBAL _execute
GLOBAL _GetESP
GLOBAL _GetEBP
GLOBAL _GetEAX
GLOBAL _GetEBX
GLOBAL _GetECX
GLOBAL _GetEDX
GLOBAL _GetESI
GLOBAL _GetEDI
GLOBAL _GetEFLAGS

EXTERN kmain
EXTERN int_08
EXTERN int_09
EXTERN invop
EXTERN div0
EXTERN gpf
EXTERN ssf
EXTERN snp
EXTERN bounds

EXTERN backuper
EXTERN Schedule 
EXTERN ExecuteProcess
EXTERN LoadESP
EXTERN SaveESP
EXTERN GetTemporaryESP
EXTERN GetNextProcess


SECTION .text

_GetESP:
	mov eax, esp;
	ret
	
_GetEBP:
	mov eax, ebp;
	ret

_GetEAX:
	ret

_GetEBX:
	mov eax, ebx;
	ret

_GetECX:
	mov eax, ecx;
	ret

_GetEDX:
	mov eax, edx;
	ret

_GetESI:
	mov eax, esi;
	ret

_GetEDI:
	mov eax, edi;
	ret
	
_GetFLAGS:
	pushf
	pop eax
	ret	
	
_int8:
	call int_08
	ret

_Cli:
	cli						; limpia flag de interrupciones
	ret

_Sti:
	sti						; habilita interrupciones por flag
	ret

_mascaraPIC1:				; Escribe mascara del PIC 1
	push ebp
	mov ebp, esp
	mov ax, [ss:ebp + 8]	; ax = mascara de 16 bits
	out 21h, al
	pop ebp
	retn

_mascaraPIC2:				; Escribe mascara del PIC 2
	push ebp
	mov ebp, esp
	mov ax, [ss:ebp + 8]	; ax = mascara de 16 bits
	out 0A1h, al
	pop ebp
	retn

read_msw:
	smsw ax					; Obtiene la Machine Status Word
	retn

_lidt:						; Carga el IDTR
	push ebp
	mov ebp, esp
	push ebx
	mov ebx, [ss: ebp + 6]	; ds:bx = puntero a IDTR
	rol ebx, 16
	lidt [ds: ebx]			; carga IDTR
	pop ebx
	pop ebp
	retn

_getgdt:
	sgdt [0x1234]
	retn

_hacersnp:
;	push 0x30
;	pop ds
	retn

_hacergpf:
	mov ax, 77
	mov ds, ax
	retn
	
_lgdt:
	lgdt [0x1234]
	retn

_hacerssf:
	mov ax, 0x30
	mov ss, ax
	retn

_hacerbounds:
	mov ax, 100
	bound ax, [0x1234]
	retn

_hacerinvop:
	mov ebx, 0xC8C70FF0
	mov [0x123456], ebx
	jmp 0x123456


_int_08_hand:			; Handler de INT 8 ( Timer tick)
	cli
	pushad
		mov eax, esp
		push eax
			call SaveESP
		pop eax
		call GetTemporaryESP
		mov esp, eax
		call GetNextProcess
		push eax
			call LoadESP
		pop ebx
		mov esp,eax
		;call _debug;
	popad
	
	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al
	sti
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

_int_09_hand:			; Handler de INT 9 (Teclado)
	cli
	pusha
	mov eax, 0			;
	mov [0x123450],eax
	in al, 60h
	push eax			; 
	call int_09                 
	pop eax
	mov al,20h			; Envio de EOI generico al PIC
	out	20h,al
	popa                            
	sti
	iret

beep:
	mov al, 182			; Prepare the speaker for the
	out 43h, al			; note.
	mov ax, 4560		; Frequency number (in decimal)
						;  for middle C.
	out 42h, al			; Output low byte.
	mov al, ah			; Output high byte.
	out 42h, al
	in al, 61h			; Turn on note (get value from
						; port 61h).
	or al, 00000011b	; Set bits 1 and 0.
	out 61h, al			; Send new value.
	mov bx, 25			; Pause for duration of note.
.pause1:
	mov cx, 65535
.pause2:
	dec cx
	jne .pause2
	dec bx
	jne .pause1
	in al, 61h			; Turn off note (get value from
						; port 61h).
	and al, 11111100b	; Reset bits 1 and 0.
	out 61h, al			; Send new value.
	retn

_movercursor:
	mov al, 0x0f
	mov dx, 0x3b4
	out dx,al
	mov al, 1
	mov dx, 0x3b5
	out dx,al
	retn

; Debug para el BOCHS, detiene la ejecución.
; Para continuar colocar en el BOCHSDBG: set $eax=0
;

_debug:
	push bp
	mov bp, sp
	push ax
vuelve:
	mov ax, 1
	cmp	ax, 0

	jne	vuelve
	pop	ax
	pop bp
	retn

_ponerss:
	mov ax, [0x10000]
	mov ss, ax
	retn


_ponercs:
	mov ax, [0x10000]
	push ax
	pop cs
	retn


_ponerds:
	mov ax, [0x10000]
	mov ds, ax
	retn

_ejecutar: 
	
	mov eax, [0x101800]
	mov  [$+5],eax
	nop
	nop
	nop
	nop
	nop
	nop
	retn

_execute: ; QUE FUNCION MALDITA QUE SOS!
	mov eax, esp
	mov ebx, [ss:eax+4]
	mov ecx, [ss:eax+8]
	mov edx, [ss:eax+12]
	mov esp, ebx ;
	push edx
	push ecx
	;call _ExecuteProcess; esto nunca retorna
	ret ;TODO: ESTA LINEA LA AGREGE YYO PQ NO TENOG IDEA PQ NUNCA REORNA :S
	
_debugBuenaOnda:
	push eax
		mov eax, 0fdh
		push eax
			call _mascaraPIC1
		pop eax
		mov ax,1
		mov [0x123450], ax
	otravez:
		mov ax, [0x123450]
		cmp ax ,0
		jne otravez
		mov eax, 0fch
		push eax
			call _mascaraPIC1
		pop eax
	pop eax

	retn
