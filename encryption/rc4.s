BITS 64

global _rc4

section .text

; void rc4(char *bytes, int64_t length, const char *key, int keysize);
_rc4:
	; check args
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

	sub rsp, 8 + 8 + 8 + 4 + 4 + 4 + 256
	mov [rsp], rdi				; bytes
	mov [rsp + 8], rsi			; length
	mov [rsp + 16], rdx			; key
	mov DWORD [rsp + 24], ecx	; keysize

	mov rcx, -1
	lea r8, [rsp + 36]			; r8 = state

.state_init_loop:
	inc rcx
	cmp rcx, 256
	jnl .ready_state_mix
	
	mov BYTE [r8 + rcx], cl

	jmp .state_init_loop

.ready_state_mix:
	mov rcx, -1
	mov r9, [rsp + 16]			; r9 = key
	mov DWORD [rsp + 28], 0

.state_mixing_loop:
	inc rcx
	cmp rcx, 256
	jnl .ready_encrypt

	; j = (j + state[i] + key[i % keysize]) % 256;
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

	; ft_swap(&state[i], &state[j])
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
	mov DWORD [rsp + 28], 0			; j
	mov DWORD [rsp + 32], 0			; k
	xor rax, rax

.encrypt:
	inc rcx
	cmp rcx, [rsp + 8]
	jnl .restore_stack

	; k = (k + 1) % 256
	mov eax, DWORD [rsp + 32]
	inc rax
	push rcx
	mov rcx, 256
	xor rdx, rdx
	div rcx
	mov DWORD [rsp + 32 + 8], edx

	; j = (j + state[k]) % 256
	mov eax, DWORD [rsp + 28 + 8]
	add al, BYTE [r8 + rdx]
	xor rdx, rdx
	div rcx
	mov DWORD [rsp + 28 + 8], edx
	mov eax, DWORD [rsp + 32 + 8]

	; ft_swap((char *)&state[k], (char *)&state[j]);
	push rax
	push rdx
	lea rsi, [r8 + rdx]
	lea rdi, [r8 + rax]
	call _ft_swap
	pop rdx
	pop rax

	; int t = (state[k] + state[j]) % 256;
	mov al, BYTE [r8 + rax]
	add al, BYTE [r8 + rdx]
	mov rcx, 256
	xor rdx, rdx
	div rcx
	pop rcx
	xor rax, rax

	; bytes[i] = bytes[i] ^ state[t];
	mov al, BYTE [r10 + rcx]
	xor al, BYTE [r8 + rdx]
	mov BYTE [r10 + rcx], al

	jmp .encrypt

.restore_stack:
	add rsp, 8 + 8 + 8 + 4 + 4 + 4 + 256
	pop rbp

.endfunc:
	ret

; void ft_swap(char *s1, char *s2);
_ft_swap:
	mov al, BYTE [rdi]
	mov dl, BYTE [rsi]

	mov BYTE [rsi], al
	mov BYTE [rdi], dl

	ret
