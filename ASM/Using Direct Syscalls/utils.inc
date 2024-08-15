; ╔═══════════════════════════════════════════════════════════╗
; ║                                                           ║
; ║                    ⚠️  WARNING!  ⚠️                      ║
; ║                                                           ║
; ║    Do not change the names or values of variables and     ║
; ║    structures here unless necessary. If you do, edit      ║
; ║    main.asm                                               ║
; ║                                                           ║
; ╚═══════════════════════════════════════════════════════════╝

extern GetModuleHandleA:PROC
extern GetProcAddress:PROC
extern ExitProcess:PROC
extern CloseHandle:PROC
extern CreateRemoteThread:PROC

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
    ObjAttr OBJECT_ATTRIBUTES   <> 
    CID     CLIENT_ID           <>

	
	HandleProcess			QWORD ?
	HandleThread			QWORD ?
	RemoteBuffer			QWORD ?
	LoadLibraryAddress		QWORD ?
	NTDLLAddress			QWORD ?
	KERNEL32Address			QWORD ?

	; /* DLL's */
	NTDLLString				db "ntdll.dll",0
	KERNEL32String			db "kernel32.dll",0

	; /* Functions */ 
	LoadLibraryString		db "LoadLibraryA",0
	NtOpenProcessString		db "NtOpenProcess",0
	NtAllocateVirtualString db "NtAllocateVirtualMemory",0
	NtWriteVirtualString	db "NtWriteVirtualMemory",0
	NtCreateThreadString	db "NtCreateThreadEx",0
	NtWaitString			db "NtWaitForSingleObject",0
	NtCloseString			db "NtClose",0

.code

CheckFunctionResult PROC
	mov r15,[rsp]
	cmp rcx,0
	jz ExitProgram

	cmp rdx,0
	jz ReturnMain
	mov [rdx],rcx

ReturnMain:
	mov [rsp],r15
	ret
CheckFunctionResult ENDP

ExitProgram PROC
	lea rdx,[NtCloseString]
	mov rcx,NTDLLAddress
	call GetSSN
	mov r15,rax 

	mov rcx,[HandleProcess]
	cmp rcx,0
	je Exit
	call NtClose

	mov rcx,[HandleThread]
	cmp rcx,0
	je Exit 
	mov rax,r15 
	call NtClose
Exit:
	xor rcx,rcx
	call ExitProcess
ExitProgram ENDP