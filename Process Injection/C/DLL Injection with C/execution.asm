includelib ".\lib\user32.lib"
includelib ".\lib\kernel32.lib"

extern CreateRemoteThread:PROC 
extern WaitForSingleObject:PROC
extern GetLastError:PROC
extern CloseHandle:PROC 
extern ExitProcess:PROC 

.code 
    public _DllMainCRTStartup
    
_DllMainCRTStartup PROC 
    ; r8  => loadlibrary    address
    ; rdx => virtualALlocEx address
    ; rcx => hProcess       address
    mov [rbp-24h],rcx 

    ;   HANDLE CreateRemoteThread(
    ;       [in]  HANDLE                 hProcess,
    ;       [in]  LPSECURITY_ATTRIBUTES  lpThreadAttributes,
    ;       [in]  SIZE_T                 dwStackSize,
    ;       [in]  LPTHREAD_START_ROUTINE lpStartAddress,
    ;       [in]  LPVOID                 lpParameter,
    ;       [in]  DWORD                  dwCreationFlags,
    ;       [out] LPDWORD                lpThreadId
    ;   );
    mov r15,[rsp]

    mov rax,[rbp-24h] 
    mov qword ptr [rsp+30h],0       ; lpThreadAttributes,
    mov qword ptr [rsp+28h],0      ; dwStackSize
    mov qword ptr [rsp+20h],rdx     ; lpParameter
    mov r9,r8                       ; lpStartAddress
    mov r8d,0                       ; dwCreationFlags
    mov edx,0                       ; lpThreadId
    mov rcx,rax                     ; hProcess
    call CreateRemoteThread 

    cmp rax,0
    je _failure

    ;   DWORD WaitForSingleObject(
    ;       [in] HANDLE hHandle,
    ;       [in] DWORD  dwMilliseconds
    ;   );
    mov rcx,rax                     ; hHandle                 
    mov edx,0FFFFFFFFh              ; dwMilliseconds: 0FFFFFFFFh == INFINITE
    call WaitForSingleObject

    mov rax,1
    mov [rsp],r15
    ret 
_DllMainCRTStartup ENDP

_failure:
    ; Close Handle 
    mov rcx,[rbp-24h]
    call CloseHandle

    ; Exit Program
    call GetLastError
    mov rcx,rax 
    call ExitProcess
END