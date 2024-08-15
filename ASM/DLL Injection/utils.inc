; ╔═══════════════════════════════════════════════════════════╗
; ║                                                           ║
; ║                    ⚠️  WARNING!  ⚠️                      ║
; ║                                                           ║
; ║    Do not change the names or values of variables and     ║
; ║    structures here unless necessary. If you do, edit      ║
; ║    main.asm                                               ║
; ║                                                           ║
; ╚═══════════════════════════════════════════════════════════╝

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
	Kernel32Str		        db "kernel32.dll",0
	LoadLibraryStr	        db "LoadLibraryA",0

	HandleProcess           QWORD ? 
	HandleThread			QWORD ?
	RemoteBuffer			QWORD ?
	LoadLibraryAddress      QWORD ?
	Kernel32Address		    QWORD ?

.code

CheckFunctionResult PROC
	mov r15,[rsp]
	cmp rcx,0
	jz ExitProgram

	cmp rdx,0
	je ReturnToMain
	mov [rdx],rcx

ReturnToMain:
	mov [rsp],r15
	ret
CheckFunctionResult ENDP

ExitProgram PROC
	mov rax,[HandleProcess]
	cmp rax,0
	je Exit
	mov rcx,rax
	call CloseHandle

	mov rax,[HandleThread]
	cmp rax,0
	je ExitProgram
	mov rcx,rax
	call CloseHandle

	Exit:
	xor rcx,rcx
	call ExitProcess
ExitProgram ENDP