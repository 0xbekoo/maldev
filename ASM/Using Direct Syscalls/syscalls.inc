; ╔═══════════════════════════════════════════════════════════╗
; ║                                                           ║
; ║                    ⚠️  WARNING!  ⚠️                      ║
; ║                                                           ║
; ║  The SSN numbers prepared for each NT Function will be    ║
; ║  passed into eax by the GetSSN function before coming to  ║
; ║  the corresponding function. See main.asm for details.    ║
; ║                                                           ║
; ╚═══════════════════════════════════════════════════════════╝

.code

NtOpenProcess PROC
	mov r10,rcx
	syscall
	ret
NtOpenProcess ENDP

NtAllocateVirtualMemory PROC
	mov r10,rcx
	syscall
	ret
NtAllocateVirtualMemory ENDP

NtWriteVirtualMemory PROC
	mov r10,rcx
	syscall 
	ret
NtWriteVirtualMemory ENDP

NtCreateThreadEx PROC
	mov r10,rcx
	syscall 
	ret
NtCreateThreadEx ENDP

NtWaitForSingleObject PROC
	mov r10,rcx
	syscall 
	ret
NtWaitForSingleObject ENDP

NtClose PROC
	mov r10,rcx
	syscall 
	ret
NtClose ENDP 