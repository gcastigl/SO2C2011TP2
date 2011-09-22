global __read
global __write
global __cpuspeed

SECTION .text

__read:
    	mov ecx, [esp+8]
	mov eax,3
	mov ebx, [esp+4]
	mov edx, [esp+12]
	int 80h
	ret

__write:
    	mov ecx, [esp+8]
	mov eax,4
	mov ebx, [esp+4]
	mov edx, [esp+12]
	int 80h
	ret

__cpuspeed:
	 mov ebx, [esp+4]
	 mov eax,5
	 int 80h
	 ret