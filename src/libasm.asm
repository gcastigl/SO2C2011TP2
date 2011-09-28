GLOBAL  _read_msw,_lidt
GLOBAL  _int_08_hand
GLOBAL	_int_09_hand
GLOBAL _int_80_hand
GLOBAL  _mascaraPIC1,_mascaraPIC2,_Cli,_Sti
GLOBAL  _debug
GLOBAL	_outb
GLOBAL	_inb
GLOBAL _reset
GLOBAL _cpuIdTest
GLOBAL _rdtscTest
GLOBAL _rdmsrTest
GLOBAL _SysCall
GLOBAL _tscGetCpuSpeed
GLOBAL _msrGetCpuSpeed
GLOBAL _getTTCounter
GLOBAL _initTTCounter
GLOBAL _newStack

EXTERN  getNextProcess
EXTERN	int_09
EXTERN	int_80

SECTION .data
SEG_BIOS_DATA_AREA	equ	40h
OFFSET_TICK_COUNT	equ 6Ch
INTERVAL_IN_TICKS	equ 10

SECTION .bss
ttcounter 	resd 1
low			resd 1
high		resd 1

SECTION .text

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

_Cli:
	cli	; limpia flag de interrupciones
	ret

_Sti:

	sti	; habilita interrupciones por flag
	ret

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


_lidt:				; Carga el IDTR
	push	ebp
	mov		ebp, esp
	push	ebx
	mov		ebx, [ss: ebp + 6] ; ds:bx = puntero a IDTR
	rol		ebx,16
	lidt	[ds: ebx]          ; carga IDTR
	pop		ebx
	pop		ebp
	retn


_newStack:
    push ebp
    mov ebp, esp

            mov eax, [ebp + 12] ; Pointer to the bottom of the new stack.
            mov esp, eax        ; We switch to the new stack

            mov eax, [ebp + 16] ; This is the char* line argument
            push eax            ; Pushed so it can be read.

            mov eax, [ebp + 20] ; Function to clean-up after task is done.
            push eax            ; To the new stack it goes!

            mov eax, 512        ; New flags register, to be iret'd
            push eax            ; There we go.                       

            push cs             ; Code segment.

            mov eax, [ebp + 8]  ; Function for the new task to run.
            push eax            ; To the new stack it goes as well!

            pushad              ; Trash. Won't matter.
            mov eax, esp        ; Return the moved stack pointer.

    mov esp, ebp
    pop ebp
    ret

_int_08_hand:				; Handler de INT 8 ( Timer tick)
	cli
	pushad

; Increase timerTick counter	
	push	eax
	mov		eax, [ttcounter]
	inc		eax
	mov		[ttcounter], eax
	pop		eax
; Switch process
	mov		ebx, esp
	push	ebx
	call	getNextProcess
	pop		ebx
	mov		esp, ebx
; Finished switching process
	mov		al,20h			; Envio de EOI generico al PIC
	out		20h,al

	popad
	sti
	
	iret

_int_09_hand:				; Handler de INT 9 ( Teclado )
	cli
	push	ds
	push	es
	pushad
	call	int_09
	mov		al,20h			; Envio de EOI generico al PIC
	out		20h,al
	popad
	pop		es
	pop		ds
	sti
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

_reset:
.wait1:
	in		al, 64h
	test	al, 02h
	jne		.wait1
	mov		al, 0FEh
	out		64h, al
	ret

; Returns 1 if cpuid function is present.
_cpuIdTest:
	pushfd ; get
	pop eax
	mov ecx, eax ; save
	xor eax, 0x200000 ; flip
	push eax ; set
	popfd
	pushfd ; and test
	pop eax
	xor eax, ecx ; mask changed bits
	shr eax, 21 ; move bit 21 to bit 0
	and eax, 1 ; and mask others
	push ecx
	popfd ; restore original flags
	ret

_rdtscTest:
	push ebp
	mov ebp, esp
	mov eax, 1
	cpuid
	mov eax, 0
	sub dx, 10h 
	mov ax, dx ; if eax != 0, rdtsc is supported
	mov esp, ebp
	pop ebp
	ret
	
_rdmsrTest:
	push ebp
	mov ebp, esp
	mov eax, 1
	cpuid
	mov eax, 0
	sub dx, 20h 
	mov ax, dx ; if eax != 0, rdmsr is supported
	mov esp, ebp
	pop ebp
	ret

_tscGetCpuSpeed:
	push ebp
	mov ebp, esp
	
	mov ebx, [ttcounter]
.wait_irq0:
	cmp  ebx, [ttcounter]
	jz	.wait_irq0
	push ebx
	cpuid
	rdtsc                   ; read time stamp counter
	mov [low], eax
	mov	[high], edx
	pop ebx
	add	ebx, INTERVAL_IN_TICKS + 1             ; Set time delay value ticks.

.wait_for_elapsed_ticks:
	cmp	ebx, [ttcounter] ; Have we hit the delay?
	jnz	.wait_for_elapsed_ticks
	mov eax, 0
	push ebx
	cpuid
	rdtsc
	sub eax, [low]  ; Calculate TSC
	sbb edx, [high]
	pop ebx
	; f(total_ticks_per_Second) =  (1 / total_ticks_per_Second) * 1,000,000
	; This adjusts for MHz.
	; so for this: f(100) = (1/100) * 1,000,000 = 10000
	; we use 18.2, so 1/18.2 * 1000000 = 54945
	mov ebx, 54945 * INTERVAL_IN_TICKS
    div ebx
	; ax contains measured speed in MHz
.end:
	mov esp, ebp
	pop ebp
	ret

_msrGetCpuSpeed:
	push ebp
	mov ebp, esp
	
	mov ebx, [ttcounter]
.wait_irq0:
	cmp  ebx, [ttcounter]
	jz	.wait_irq0
	push ebx
	cpuid
	mov ecx, 10h
	rdmsr
	mov [low], eax
	mov	[high], edx
	pop ebx
	add	ebx, INTERVAL_IN_TICKS + 1             ; Set time delay value ticks.

.wait_for_elapsed_ticks:
	cmp	ebx, [ttcounter] ; Have we hit the delay?
	jnz	.wait_for_elapsed_ticks
	mov eax, 0
	push ebx
	cpuid
	mov ecx, 10h
	rdmsr
	sub eax, [low]  ; Calculate TSC
	sbb edx, [high]
	pop ebx
	; f(total_ticks_per_Second) =  (1 / total_ticks_per_Second) * 1,000,000
	; This adjusts for MHz.
	; so for this: f(100) = (1/100) * 1,000,000 = 10000
	; we use 18.2, so 1/18.2 * 1000000 = 54945
	mov ebx, 54945 * INTERVAL_IN_TICKS
    div ebx
	; ax contains measured speed in MHz
.end:
	mov esp, ebp
	pop ebp
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


