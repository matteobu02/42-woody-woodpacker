BITS 32

global _rc4

section .text

; void rc4(char *bytes, uint32_t length, const char *key, uint32_t keysize);
_rc4:
	; check args
	cmp edi, 0
	je .endfunc
	cmp esi, 0
	jng .endfunc
	cmp edx, 0
	je .endfunc
	cmp ecx, 0
	jng .endfunc

	push ebp
	mov ebp, esp

	sub esp, 8 + 8 + 8 + 4 + 4 + 4 + 256
	mov [esp], edi				; bytes
	mov [esp + 8], esi			; length
	mov [esp + 16], edx			; key
	mov DWORD [esp + 24], ecx	; keysize

	mov ecx, -1
	lea r8d, DWORD [esp + 36]			; r8d = state

.state_init_loop:
	inc ecx
	cmp ecx, 256
	jnl .ready_state_mix

	mov BYTE [r8d + ecx], cl

	jmp .state_init_loop

.ready_state_mix:
	mov ecx, -1
	mov r9d, DWORD [esp + 16]			; r9d = key
	mov DWORD [esp + 28], 0

.state_mixing_loop:
	inc ecx
	cmp ecx, 256
	jnl .ready_encrypt

	; j = (j + state[i] + key[i % keysize]) % 256;
	mov eax, DWORD [esp + 28]
	add al, DWORD [r8d + ecx]
	mov DWORD [esp + 28], eax

	push ecx
	mov eax, ecx
	mov ecx, DWORD [esp + 24 + 8]
	xor edx, edx
	div ecx

	xor eax, eax
	mov eax, DWORD [esp + 28 + 8]
	add al, BYTE [r9d + edx]

	mov ecx, 256
	xor edx, edx
	div ecx
	mov DWORD [esp + 28 + 8], edx

	; ft_swap(&state[i], &state[j])
	pop ecx
	push ecx
	lea esi, DWORD [r8d + edx]
	lea edi, DWORD [r8d + ecx]
	call _ft_swap
	pop ecx

	jmp .state_mixing_loop

.ready_encrypt:
	mov ecx, -1
	mov r10d, DWORD [esp]
	mov DWORD [esp + 28], 0			; j
	mov DWORD [esp + 32], 0			; k
	xor eax, eax

.encrypt:
	inc ecx
	cmp ecx, [esp + 8]
	jnl .restore_stack

	; k = (k + 1) % 256
	mov eax, DWORD [esp + 32]
	inc eax
	push ecx
	mov ecx, 256
	xor edx, edx
	div ecx
	mov DWORD [esp + 32 + 8], edx

	; j = (j + state[k]) % 256
	mov eax, DWORD [esp + 28 + 8]
	add al, BYTE [r8d + edx]
	xor edx, edx
	div ecx
	mov DWORD [esp + 28 + 8], edx
	mov eax, DWORD [esp + 32 + 8]

	; ft_swap((char *)&state[k], (char *)&state[j]);
	push eax
	push edx
	lea esi, DWORD [r8d + edx]
	lea edi, DWORD [r8d + eax]
	call _ft_swap
	pop edx
	pop eax

	; int t = (state[k] + state[j]) % 256;
	mov al, BYTE [r8d + eax]
	add al, BYTE [r8d + edx]
	mov ecx, 256
	xor edx, edx
	div ecx
	pop ecx
	xor eax, eax

	; bytes[i] = bytes[i] ^ state[t];
	mov al, BYTE [r10d + ecx]
	xor al, BYTE [r8d + edx]
	mov BYTE [r10d + ecx], al

	jmp .encrypt

.restore_stack:
	add esp, 8 + 8 + 8 + 4 + 4 + 4 + 256
	pop ebp

.endfunc:
	ret

; void ft_swap(char *s1, char *s2);
_ft_swap:
	mov al, BYTE [edi]
	mov dl, BYTE [esi]

	mov BYTE [esi], al
	mov BYTE [edi], dl

	ret
