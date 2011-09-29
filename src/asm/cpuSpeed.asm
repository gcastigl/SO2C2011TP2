GLOBAL _cpuIdTest
GLOBAL _rdtscTest
GLOBAL _rdmsrTest
GLOBAL _tscGetCpuSpeed
GLOBAL _msrGetCpuSpeed

SECTION .data
INTERVAL_IN_TICKS	equ 10
OFFSET_TICK_COUNT	equ 6Ch

common ttcounter 4

SECTION .bss
low			resd 1
high		resd 1

SECTION .text
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
