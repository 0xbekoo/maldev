includelib ".\lib\user32.lib"
includelib ".\lib\kernel32.lib"

extern GetModuleHandleW:PROC 
extern GetProcAddress:PROC
extern ExitProcess:PROC
extern WaitForSingleObject:PROC 

OBJECT_ATTRIBUTES STRUCT
    oLength                  QWORD ?
    RootDirectory            QWORD ?
    ObjectName               QWORD ?
    Attributes               QWORD ?
    SecurityDescriptor       QWORD ?
    SecurityQualityOfService QWORD ?
OBJECT_ATTRIBUTES ENDS

CLIENT_ID STRUCT
    UniqueProcess            DWORD ?
    UniqueThread             QWORD ?
CLIENT_ID ENDS

.data     
    PID                         equ 0        
    objAttr OBJECT_ATTRIBUTES   <> 
    CID     CLIENT_ID           <>

    hProcess                    QWORD 0
    hThread                     QWORD 0
    rBuffer                     QWORD 0
    wLoadLibrary                QWORD ?
    dllSize                     QWORD 0
    

    ntOpenStr                           db "NtOpenProcess",0
    AddrOfNtOpenProcess                 dw 0

    ntVirtualStr                        db "NtAllocateVirtualMemory",0
    AddrOfNtAllocateVirtualMemory       dw 0

    ntWriteStr                          db "NtWriteVirtualMemory",0
    AddrOfNtWriteVirtualMemory          dw 0

    ntCreateStr                 db "NtCreateThreadEx",0
    AddrOfNtCreateThreadEx            dw 0

    ntCloseString               db "NtClose",0 
    AddrOfNtClose                     dw 0

	payload DB 0DEH 0ADH 0BEH 0EFH ; 0xDEADBEEF
    payloadSize QWORD 0

.code

GetSSN PROC 
    mov r15,[rsp]
    
    mov rdx,rsi
    mov rcx,rax
    call GetProcAddress

    cmp rax,0 
    je _returnToCleanup

    mov rcx,rax 
    mov eax,1
    imul rax,rax,0
    movzx eax,byte ptr [rcx+rax+4]

    _returnToMain:
        mov [rsp],r15
        ret     

    _returnToCleanup:
        jmp CLEANUP
GetSSN ENDP 

CLEANUP PROC
    lea rsi,ntCloseString
    lea rdx,AddrOfNtClose
    mov rax,[rbp + 80h]
    call GetSSN 
    mov [rbp + 85h],rax 

    CleanuProcessHandle:
        mov rcx,hProcess 
        cmp rcx,0
        jne CleanupThreadHandle

        call rax 

    CleanuProcessHandle 
        mov rcx,hThread
        cmp rcx,0 
        jne ExitProgram 

        mov rax,[rbp + 85h]
        call rax 

    ExitProgram 
        xor rcx,rcx 
        call ExitProcess     
CLEANUP ENDP