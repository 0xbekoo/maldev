.code

; /*
;
; PVOID GetAddress(
;	_In PVOID Address
; );
;
; */
GetAddress PROC
	; This function will return the address of the Address parameter.

	mov r10,rcx
	xor rdi,rdi
	mov r10,qword ptr [r10+rdi]

	mov rax,r10
	ret
GetAddress ENDP


; /*
; 
; BOOLEAN ChangePreviousMode(
;	_in_ int Mode
; );
;
; */
ChangePreviousMode PROC
	; In PreviousMode we have 0 for KernelMode and 1 for UserMode.
	; It means, we can only change the PreviousMode to 0 or 1.

	cmp rcx,0		; Check if the Mode is Kernel Mode (0)
	jz ChangePrevious

	cmp rcx,1		; Check if the Mode is User Mode (1)
	jz ChangePrevious

	; If the Mode is not 0 or 1, return FALSE
	xor rax,rax
	jmp Return 

ChangePrevious:
	push rdx 
	
	; We need to get the KPCR because the KPCR contains the PreviousMode
	mov rdx,qword ptr gs:[188h]

	; The 0x232 offset is point to the PreviousMode in the KPCR
	mov byte ptr [rdx+232h],cl
	pop rdx

	mov rax,1

Return:
	ret
ChangePreviousMode ENDP
END
