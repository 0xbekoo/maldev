.data 
extern NtOpenProcessSSN:DWORD
extern NtAllocateVirtualMemorySSN:DWORD
extern NtWriteVirtualMemorySSN:DWORD
extern NtCreateThreadExSSN:DWORD
extern NtWaitForSingleObjectSSN:DWORD
extern NtCloseSSN:DWORD

.code
NtOpenProcess proc 
	mov r10, rcx
	mov eax, NtOpenProcessSSN       
	syscall                         
	ret                             
NtOpenProcess endp

NtAllocateVirtualMemory proc    
	mov r10, rcx
	mov eax, NtAllocateVirtualMemorySSN      
	syscall                        
	ret                             
NtAllocateVirtualMemory endp

NtWriteVirtualMemory proc 
	mov r10, rcx
	mov eax, NtWriteVirtualMemorySSN      
	syscall                        
	ret                             
NtWriteVirtualMemory endp 

NtCreateThreadEx proc 
	mov r10, rcx
	mov eax, NtCreateThreadExSSN      
	syscall                        
	ret                             
NtCreateThreadEx endp 

NtWaitForSingleObject proc 
	mov r10, rcx
	mov eax, NtWaitForSingleObjectSSN      
	syscall                        
	ret                             
NtWaitForSingleObject endp 

NtClose proc 
	mov r10, rcx
	mov eax, NtCloseSSN      
	syscall                        
	ret                             
NtClose endp 
end