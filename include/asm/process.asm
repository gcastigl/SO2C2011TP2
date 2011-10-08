GLOBAL _buildStackFrame

_buildStackFrame:
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