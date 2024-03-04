includelib ".\lib\user32.lib"
includelib ".\lib\kernel32.lib"

extern GetModuleHandleW :PROC
extern GetLastError     :PROC
extern ExitProcess      :PROC

.code
    public getTargetModule

getTargetModule:
    ; rcx => ModuleName
    mov r15,[rsp]
    mov rax,rcx
    call GetModuleHandleW

    cmp rax, 0
    je _error

    mov [rsp],r15
    ret

_error:   
    call GetLastError 
    mov rcx,rax 
    
    mov [rsp],r15
    xor rax, rax  ; Return 0 on error
    ret

_DllMainCRTStartup PROC
    ; this function is as empty as my life lmao
    mov rax,0
_DllMainCRTStartup ENDP 
END