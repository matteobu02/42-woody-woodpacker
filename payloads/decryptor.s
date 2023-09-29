BITS 64

global _start

section .text

start:
	push rax
	push rcx
	push rdx
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11

	mov ecx, 0x42194219			; key size
	mov rdx, 0x1942194219421942	; key ptr
	mov rsi, 0x1919191919191919	; text size
	mov rdi, 0x4242424242424242	; text ptr
	call _rc4

	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rdx
	pop rcx
	pop rax

	mov rbx, 0x1919191919424242	; parasite ptr
	jmp rbx

_rc4:
	cmp rdi, 0 
	je .endfunc
	cmp rsi, 0 
	jng .endfunc
	cmp rdx, 0 
	je .endfunc
	cmp ecx, 0 
	jng .endfunc
	push rbp
	mov rbp, rsp
	sub rsp, 292
	mov [rsp], rdi
	mov [rsp + 8], rsi
	mov [rsp + 16], rdx
	mov DWORD [rsp + 24], ecx
	mov edx, 256
	xor rsi, rsi
	lea rdi, [rsp + 36]
	call _ft_memset
	mov rcx, -1
	lea r8, [rsp + 36]
.state_init_loop:
	inc rcx
	cmp rcx, 256
	jnl .ready_state_mix
	mov BYTE [r8 + rcx], cl
	jmp .state_init_loop
.ready_state_mix:
	mov rcx, -1
	mov r9, [rsp + 16]
	mov DWORD [rsp + 28], 0
.state_mixing_loop:
	inc rcx
	cmp rcx, 256
	jnl .ready_encrypt
	mov eax, DWORD [rsp + 28]
	add al, [r8 + rcx]
	mov DWORD [rsp + 28], eax
	push rcx
	mov rax, rcx
	mov ecx, DWORD [rsp + 24 + 8]
	xor rdx, rdx
	div rcx
	xor rax, rax
	mov eax, DWORD [rsp + 28 + 8]
	add al, BYTE [r9 + rdx]
	mov rcx, 256
	xor rdx, rdx
	div rcx
	mov DWORD [rsp + 28 + 8], edx
	pop rcx
	push rcx
	lea rsi, [r8 + rdx]
	lea rdi, [r8 + rcx]
	call _ft_swap
	pop rcx
	jmp .state_mixing_loop
.ready_encrypt:
	mov rcx, -1
	mov r10, [rsp]
	mov DWORD [rsp + 28], 0
	mov DWORD [rsp + 32], 0
	xor rax, rax
.encrypt:
	inc rcx
	cmp rcx, [rsp + 8]
	jnl .restore_stack
	mov eax, DWORD [rsp + 32]
	inc rax
	push rcx
	mov rcx, 256
	xor rdx, rdx
	div rcx
	mov DWORD [rsp + 32 + 8], edx
	mov eax, DWORD [rsp + 28 + 8]
	add al, BYTE [r8 + rdx]
	xor rdx, rdx
	div rcx
	mov DWORD [rsp + 28 + 8], edx
	mov eax, DWORD [rsp + 32 + 8]
	push rax
	push rdx
	lea rsi, [r8 + rdx]
	lea rdi, [r8 + rax]
	call _ft_swap
	pop rdx
	pop rax
	mov al, BYTE [r8 + rax]
	add al, BYTE [r8 + rdx]
	mov rcx, 256
	xor rdx, rdx
	div rcx
	pop rcx
	xor rax, rax
	mov al, BYTE [r10 + rcx]
	xor al, BYTE [r8 + rdx]
	mov BYTE [r10 + rcx], al
	jmp .encrypt
.restore_stack:
	add rsp, 292
	pop rbp
.endfunc:
	ret

_ft_swap:
	mov al, BYTE [rdi]
	mov dl, BYTE [rsi]
	mov BYTE [rsi], al
	mov BYTE [rdi], dl
	ret

_ft_memset:
	mov rcx, -1
.loop:
	inc rcx
	cmp ecx, edx
	jnl .endfunc
	mov BYTE [rdi + rcx], sil
	jmp .loop
.endfunc:
	ret
