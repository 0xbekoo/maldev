.code

My_pNtOpenProcess PROC
	mov r10,rcx
	syscall
	ret
My_pNtOpenProcess ENDP

My_pNtAllocateVirtualMemory PROC
	mov r10,rcx
	syscall
	ret
My_pNtAllocateVirtualMemory ENDP

My_pNtWriteVirtualMemory PROC
	mov r10,rcx
	syscall 
	ret
My_pNtWriteVirtualMemory ENDP

My_pNtProtectVirtualMemory PROC
	mov r10,rcx
	syscall
	ret
My_pNtProtectVirtualMemory ENDP

My_pNtCreateThreadEx PROC
	mov r10,rcx
	syscall 
	ret
My_pNtCreateThreadEx ENDP

My_pNtWaitForSingleObject PROC
	mov r10,rcx
	syscall 
	ret
My_pNtWaitForSingleObject ENDP