extern ExitProcess:PROC 
extern GetModuleHandleA:PROC 
extern LoadLibraryA:PROC
extern GetProcAddress:PROC 
extern OpenProcess:PROC 
extern VirtualAllocEx:PROC 
extern WriteProcessMemory:PROC
extern CreateRemoteThread:PROC 
extern WaitForSingleObject:PROC
extern CloseHandle:PROC 


.data
	Kernel32Str	db "kernel32.dll",0
	LoadLibraryStr db "LoadLibraryA",0

	HandleProcess QWORD ? 
	HandleThread QWORD ?
	RemoteBuffer QWORD ?
	LoadLibraryAddress QWORD ?
	Kernel32Address	QWORD ?

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
;	This function closes the handles and exits the program.
;
;	* Parameters: 
;	    * rcx - Thread's handle  (QWORD)
;	    * rdx - Process's handle (QWORD)
; */
ExitProgram PROC
    sub rsp,38h

    ; Save Params
    mov r13,rcx
    mov r14,rdx

CheckHandleThread:
    cmp r13,0
    jz CheckHandleProcess

    jmp CloseThreadHandle

CheckHandleProcess:
    cmp r14,0
    jz ExitTheProgram
    
    jmp CloseProcessHandle

CloseThreadHandle:
    mov rcx,r13
    call CloseHandle

    jmp CheckHandleProcess

CloseProcessHandle:
    mov rcx,r14
    call CloseHandle

ExitTheProgram:
    xor rcx,rcx
    call ExitProcess
ExitProgram ENDP
