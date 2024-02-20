; ///  **********************************************************************  /// 
;
;
; @author:        @x1nerama (aka: bekoo)
; @website:       x1nerama.github.io
; @release-date:  02-20-2024
; @last-update:   02-20-2024 - 5.20 PM
;
;
; ///  **********************************************************************  /// 

includelib ".\lib\user32.lib"
includelib ".\lib\kernel32.lib"

extern GetModuleHandleW:PROC 
extern GetProcAddress:PROC 
extern OpenProcess:PROC 
extern VirtualAllocEx:PROC 
extern WriteProcessMemory:PROC
extern CreateRemoteThread:PROC 
extern WaitForSingleObject:PROC
extern GetCurrentProcessId:PROC
extern CloseHandle:PROC  
extern ExitProcess:PROC 
extern GetLastError:PROC 
extern WriteConsoleA:PROC 
extern GetStdHandle:PROC 

.data
    ; You need to enter it separating the characters as shown below. Otherwise you will not be able to execute the DLL.  
    ; Remember that this is a DWORD variable! If you try to convert the DB you will not be able to execute the dll in any way.  
    ; For example, if your dll is in C:, you need to set the variable as follows:
    ; dllPath DW 'C',':','\','b','e','k','o','o','.','d','l','l',0  ; L"C:\bekoo.dll"
    dllPath   DW    'p','a','t','h','\','t','o','\','d','l','l',0
    
    hKernel32 DW    'k','e','r','n','e','l','3','2',0   
    loadLibraryWStr db "LoadLibraryW",0
    hProcess        QWORD ? 
    hThread         QWORD ?
    wLoadLibrary    QWORD ?
    rBuffer         QWORD ? 
    STDOUTVARIABLE  QWORD ?
    STDOUT equ -11

   ; => *** Messages *** <=
    openProcessSuc db "[+] Opened Target Process!",10
    openProcessErr db "[-] Failed to open target Process!",10

    getModuleHandleSuc db "[*] got handle to kernel32.dll!",10
    getModuleHandleErr db "[-] Failed to get handle to kernel32.dll!",10

    loadLibraryWSuc db "[*] Got address to LoadLibraryW",10
    loadLibraryWErr db "[-] Failed to get address to LoadLibraryW",10

    virtualAllocSuc db "[+] Allocated memory address for Dll Path!",10
    virtualAllocErr db "[-] Failed to allocated memory address for Dll Path!",10

    writeProcessMemSuc db "[+] Wrote DLL Path in allocated memory address!",10
    writeProcessMemErr db "[-] Failed to write DLL Path in memory address!",10

    createRemoteThreadSuc db "[*] Created Thread. now waiting for finish.",10
    createRemoteThreadErr db "[-] Failed to create Thread.",10

    waitForSingleObjectSuc db "[+] DLL is executed! Good Job Hacker :>",10

.code 

_checkRaxValue PROC 
    cmp rax,0 
    je _printError

    xor rsi,rsi 
    xor rdi,rdi 
    ret
_checkRaxValue ENDP

_printError PROC 
    call _printMessage
    jmp _CLEAN
_printError ENDP

_printMessage PROC 
    ; rsi => Message 
    ; rdi => MessageLen 

    mov rcx, STDOUT
    call GetStdHandle
    mov qword ptr [STDOUTVariable], rax
    mov rcx,qword ptr [STDOUTVariable]
    mov rdx,rsi  
    mov r8,rdi
    mov r9,0
    call WriteConsoleA
    ret 
_printMessage ENDP 

mainCRTStartup PROC 
    push rbp 
    mov rbp,rsp 
    sub rsp,60h 

    call GetCurrentProcessId
    mov [rsp-15h],rax 

    ;* HANDLE OpenProcess(
    ;*   [in] DWORD dwDesiredAccess,
    ;*   [in] BOOL  bInheritHandle,
    ;*   [in] DWORD dwProcessId
    ;* );    
    mov r8,[rsp-15h]    ; dwProcessId
    mov edx,0           ; bInheritHandle
    mov ecx,1FFFFFh     ; dwDesiredAccess => 1FFFFFh: PROCESS_ALL_ACCESS
    call OpenProcess 
    mov hProcess,rax 
    
    ; Check rax value 
    lea rsi,openProcessErr
    mov rdi,sizeof openProcessErr
    call _checkRaxValue

    ; if no error, print success message (OpenProcess)
    lea rsi,openProcessSuc
    mov rdi,sizeof openProcessSuc
    call _printMessage 

    ;* HMODULE GetModuleHandleW(
    ;*   [in, optional] LPCWSTR lpModuleName
    ;* );
    lea rax,hKernel32   ; L"kernel32"
    mov rcx,rax         ; lpModuleName
    call GetModuleHandleW

    ; Check rax value     
    mov qword ptr [rsp-38h],rax 
    lea rsi,getModuleHandleErr
    mov rdi,sizeof getModuleHandleErr
    call _checkRaxValue

    ; if no error, print success message (GetModuleHandleW)    
    lea rsi,getModuleHandleSuc
    mov rdi,sizeof getModuleHandleSuc
    call _printMessage 

    ;* FARPROC GetProcAddress(
    ;*   [in] HMODULE hModule,
    ;*   [in] LPCSTR  lpProcName
    ;* );
    lea rsi,wLoadLibrary    ; wLoadLibrary Address
    lea rdi,loadLibraryWStr ; "LoadLibraryW"
    mov rsi,rdi 
    mov rdx,rsi             ; lpProcName
    mov rcx,[rsp-38h]       ; hModule 
    call GetProcAddress
    mov wLoadLibrary,rax 

    ; Check rax value     
    lea rsi,loadLibraryWErr
    mov rdi,sizeof loadLibraryWErr
    call _checkRaxValue

    ; if no error, print success message (LoadLibraryW)    
    lea rsi,loadLibraryWSuc
    mov rdi,sizeof loadLibraryWSuc
    call _printMessage 

    ;* LPVOID VirtualAllocEx(
    ;*   [in]           HANDLE hProcess,
    ;*   [in, optional] LPVOID lpAddress,
    ;*   [in]           SIZE_T dwSize,
    ;*   [in]           DWORD  flAllocationType,
    ;*   [in]           DWORD  flProtect
    ;* );    
    mov rax,hProcess         
    mov dword ptr [rsp+20h],40h     ; flProtect         => 40h:   PAGE_EXECUTE_READWRITE
    mov r9d,3000h                   ; flAllocationType  => 3000h: (MEM_COMMIT: 0x00001000 | MEM_RESERVE: 0x00002000)
    mov r8d,sizeof dllPath          ; dwSize
    mov edx,0                       ; lpAddress
    mov rcx,rax                     ; hProcess
    call VirtualAllocEx
    mov rBuffer,rax 

    ; check rax value    
    lea rsi,virtualAllocErr
    mov rdi,sizeof virtualAllocErr
    call _checkRaxValue

    ; if no error, print success message (VirtualAllocEx)  
    lea rsi,virtualAllocSuc
    mov rdi,sizeof virtualAllocSuc
    call _printMessage 

    ;* BOOL WriteProcessMemory(
    ;*   [in]  HANDLE  hProcess,
    ;*   [in]  LPVOID  lpBaseAddress,
    ;*   [in]  LPCVOID lpBuffer,
    ;*   [in]  SIZE_T  nSize,
    ;*   [out] SIZE_T  *lpNumberOfBytesWritten
    ;* );
    mov rax,hProcess  
    mov rdx,rBuffer
    lea rcx,dllPath
    mov qword ptr [rsp+20h],0   ; *lpNumberOfBytesWritten
    mov r9d,sizeof dllPath      ; nSize
    mov r8,rcx                  ; lpBuffer
    mov rcx,rax                 ; hProcess
    call WriteProcessMemory

    ; check rax value    
    lea rsi,writeProcessMemErr
    mov rdi,sizeof writeProcessMemErr
    call _checkRaxValue

    ; if no error, print success message (WriteProcessMemory)  
    lea rsi,writeProcessMemSuc
    mov rdi,sizeof writeProcessMemSuc
    call _printMessage 

    ;* HANDLE CreateRemoteThread(
    ;*   [in]  HANDLE                 hProcess,
    ;*   [in]  LPSECURITY_ATTRIBUTES  lpThreadAttributes,
    ;*   [in]  SIZE_T                 dwStackSize,
    ;*   [in]  LPTHREAD_START_ROUTINE lpStartAddress,
    ;*   [in]  LPVOID                 lpParameter,
    ;*   [in]  DWORD                  dwCreationFlags,
    ;*   [out] LPDWORD                lpThreadId
    ;* );
    mov rdx,rBuffer 
    mov rax,hProcess 
    mov r8,wLoadLibrary 
    mov qword ptr [rsp+30h],0   ; lpThreadId
    mov qword ptr [rsp+28h],0   ; dwCreationFlags
    mov qword ptr [rsp+20h],rdx ; lpParameter
    mov r9,r8                   ; lpStartAddress
    mov r8d,0                   ; dwStackSize
    mov edx,0                   ; lpThreadAttributes
    mov rcx,rax                 ; hProcess
    call CreateRemoteThread
    mov hThread,rax 

    ; check rax value    
    lea rsi,createRemoteThreadErr
    mov rdi,sizeof createRemoteThreadErr
    call _checkRaxValue

    ; if no error, print success message (CreateRemoteThread)    
    lea rsi,createRemoteThreadSuc
    mov rdi,sizeof createRemoteThreadSuc
    call _printMessage 

    ;* DWORD WaitForSingleObject(
    ;*   [in] HANDLE hHandle,
    ;*   [in] DWORD  dwMilliseconds
    ;* );
    mov rcx,rax                     ; hHandle                 
    mov edx,0FFFFFFFFh              ; dwMilliseconds => 0FFFFFFFFh: INFINITE
    call WaitForSingleObject

    ; print success message (WaitForSingleObject)    
    lea rsi,waitForSingleObjectSuc
    mov rdi,sizeof waitForSingleObjectSuc
    call _printMessage 

    jmp _CLEAN 
mainCRTStartup ENDP 

_CLEAN PROC 
    checkHandleProcess:
        mov rax,hProcess 
        ; if (hProcess == 0)    
        cmp rax,0
        je checkHandleThread

        mov rcx,rax 
        call CloseHandle     
    checkHandleThread:
        mov rax,hThread 
        cmp rax,0
        ; if (hThread == 0)    
        je _returnExit 

        mov rcx,rax 
        call CloseHandle
    
    _returnExit:
        jmp _exit    
_CLEAN ENDP 

_exit PROC 
    call ExitProcess 
_exit ENDP 
END 