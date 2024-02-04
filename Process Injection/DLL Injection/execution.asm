includelib ".\lib\user32.lib"
includelib ".\lib\kernel32.lib"

extern CreateRemoteThread:PROC 
extern WaitForSingleObject:PROC

.code 
    public _DllMainCRTStartup
    
_DllMainCRTStartup PROC 
    ; r8  => loadlibrary    address
    ; rdx => virtualALlocEx address
    ; rcx => hProcess       address


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

    mov rax,rcx 
    mov qword ptr [rsp+30h],0       ; lpThreadAttributes,
    mov qword ptr [rsp+28h],0       ; dwStackSize
    mov qword ptr [rsp+20h],rdx     ; lpParameter
    mov r9,r8                       ; lpStartAddress
    mov r8d,0                       ; dwCreationFlags
    mov edx,0                       ; lpThreadId
    mov rcx,rax                     ; hProcess
    call CreateRemoteThread 

    ;   DWORD WaitForSingleObject(
    ;       [in] HANDLE hHandle,
    ;       [in] DWORD  dwMilliseconds
    ;   );
    mov rcx,rax                     ; hHandle                 
    mov edx,0FFFFFFFFh              ; dwMilliseconds: 0FFFFFFFFh == INFINITE
    call WaitForSingleObject

    test rax,rax 
    mov [rsp],r15
    ret 
_DllMainCRTStartup ENDP
END