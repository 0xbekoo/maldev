.code
NtOpenProcess proc 
		; eax => SSN Number
		mov r10,rcx
		syscall                         
		ret                             
NtOpenProcess endp

NtAllocateVirtualMemory proc 
		; eax => SSN Number
		mov r10,rcx
		syscall                         
		ret                             
NtAllocateVirtualMemory endp

NtWriteVirtualMemory proc 
		; eax => SSN Number
		mov r10,rcx
		syscall                         
		ret                             
NtWriteVirtualMemory endp

NtCreateThreadEx proc 
		; eax => SSN Number
		mov r10,rcx
		syscall                         
		ret                             
NtCreateThreadEx endp