GLOBAL copy_page_physical
GLOBAL read_esp
GLOBAL read_eip
GLOBAL read_cs
GLOBAL read_ss
GLOBAL read_ds
GLOBAL read_ebp
GLOBAL loadStackFrame
GLOBAL switchProcess
EXTERN getNextProcess

read_eip:
    mov eax, [esp]
    ret

read_esp:
    mov eax, esp
    ret

read_cs:
    mov eax, cs;
    ret

read_ss:
    mov eax, ss;
    ret

read_ds:
    mov eax, ds;
    ret
    
read_ebp:
    mov eax, ebp;
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
    pushf
    push esp
    call getNextProcess
    pop esp
    mov esp, eax		; cambia el stack pointer
    popf
    popa
    sti
    ret
    

loadStackFrame:	
    mov ebx, ebp
    mov ebp, esp

    mov eax, [ebp + 8] ; Stack start
    mov esp, eax ; new stack

    mov eax, [ebp + 16] ; argv
    push eax
    mov eax, [ebp + 12] ; argc
    push eax
    mov eax, [ebp + 20] ; return func
    push eax	
    mov eax, [ebp + 4] ; main func, will return here
    push eax
    pusha
    pushf
    mov eax, esp
    mov esp, ebp
    mov ebp, ebx
    ret
