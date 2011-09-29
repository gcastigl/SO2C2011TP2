GLOBAL  _read_msw,
GLOBAL  _mascaraPIC1,_mascaraPIC2,_cli,_sti
GLOBAL  _debug

GLOBAL	_outb
GLOBAL	_inb
GLOBAL _port_in
GLOBAL _port_out
GLOBAL _portw_in
GLOBAL _portw_out

GLOBAL _reset

GLOBAL _getTTCounter
GLOBAL _initTTCounter
GLOBAL ttcounter

GLOBAL _SysCall
GLOBAL _gdt_flush
GLOBAL _idt_flush

SECTION .data
SEG_BIOS_DATA_AREA	equ	40h

common ttcounter 4

SECTION .text

_idt_flush:
	mov eax, [esp + 4]
	lidt [eax]
	ret

_gdt_flush:
	mov eax, [esp + 4]
	lgdt [eax]
	
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush
.flush:
	ret

_initTTCounter:
	push ebp
	mov ebp, esp
	push ax
	mov eax, 0
	mov [ttcounter], eax
	pop ax
	mov esp, ebp
	pop ebp
	ret

_getTTCounter:
	push ebp
	mov ebp, esp
	mov eax, [ttcounter]
	mov esp, ebp
	pop ebp
	ret

_cli:
	cli	; limpia flag de interrupciones
	ret

_sti:

	sti	; habilita interrupciones por flag
	ret

; FIXME _outb and _inb should be removed and use port_in and port_out instaed
_outb:
	push	ebp
	mov		ebp, esp
	mov		edx, [ss:ebp+8] ;Grab data
	mov		eax, [ss:ebp+12] ;Grab port
	out		dx, ax
	pop		ebp
	retn

_inb:
	push	ebp
	mov		ebp, esp
	mov		dx, [ss:ebp+8] ;Grab port
	in		ax, dx
	mov		esp, ebp
	pop		ebp
	retn

_mascaraPIC1:			; Escribe mascara del PIC 1
	push	ebp
	mov		ebp, esp
	mov		ax, [ss:ebp+8]  ; ax = mascara de 16 bits
	out		21h,al
	pop		ebp
	retn

_mascaraPIC2:			; Escribe mascara del PIC 2
	push	ebp
	mov		ebp, esp
	mov		ax, [ss:ebp+8]  ; ax = mascara de 16 bits
	out		0A1h,al
	pop		ebp
	retn

_read_msw:
	smsw	ax		; Obtiene la Machine Status Word
	retn

;=================================================================
;					PORT_IN
;=================================================================
_port_in:
	push ebp
	mov ebp, esp
	push dx

	mov eax, 0
	mov dx, [ebp+8]
	in al, dx

	pop dx
	leave
	ret


;=================================================================
;					PORT_OUT
;=================================================================

_port_out:
	push ebp
	mov ebp, esp
	pusha

	mov dx, [ebp+8]
	mov ax, [ebp+12]
	out dx, al

	popa
	leave
	ret


;=================================================================
;					PORTW_IN
;=================================================================
_portw_in:
	push ebp
	mov ebp, esp
	push dx

	mov eax, 0
	mov dx, [ebp+8]
	in ax, dx

	pop dx
	leave
	ret


;=================================================================
;					PORTW_OUT
;=================================================================

_portw_out:
	push ebp
	mov ebp, esp
	pusha

	mov dx, [ebp+8]
	mov ax, [ebp+12]
	out dx, ax

	popa
	leave
	ret

;=================================================================
;					INTERRUPT HANDLERS
;=================================================================

_SysCall:
	push ebp
	mov ebp, esp
	pusha

	mov eax, [ebp + 8] ; Syscall number
	mov ebx, [ebp + 12]; file descriptor
	mov ecx, [ebp + 16]; buffer
	mov edx, [ebp + 20]; count
	
	int 80h

	popa
	mov esp, ebp
	pop ebp
	ret

;=================================================================
;					OTHERS
;=================================================================

_reset:
.wait1:
	in		al, 64h
	test	al, 02h
	jne		.wait1
	mov		al, 0FEh
	out		64h, al
	ret

; Debug para el BOCHS, detiene la ejecución
; Para continuar colocar en el BOCHSDBG: set $eax=0


_debug:
	push    bp
	mov     bp, sp
	push	ax
vuelve:
	mov     ax, 1
	cmp	ax, 0
	jne	vuelve
	pop	ax
	pop     bp
	retn


