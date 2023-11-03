BITS 32

global _rc4_32

section .text

; void rc4(char *bytes, uint32_t length, const char *key, uint32_t keysize);
_rc4_32:
	push ebp
	mov ebp, esp

	; check args
	cmp DWORD [ebp + 8], 0		; bytes
	je .endfunc
	cmp DWORD [ebp + 12], 0		; length
	jng .endfunc
	cmp DWORD [ebp + 16], 0		; key
	je .endfunc
	cmp DWORD [ebp + 20], 0		; keysize
	jng .endfunc

	push ebx
	sub esp, 4 + 4 + 256

	mov DWORD [esp], 0		; j = 0
	mov DWORD [esp + 4], 0		; k = 0

	mov ecx, -1			; ecx = i = -1
	lea ebx, [esp + 8]		; ebx = state

.state_init_loop:
	inc ecx				; ++i
	cmp ecx, 256
	jnl .ready_state_mix

	mov BYTE [ebx + ecx], cl	; state[i] = i

	jmp .state_init_loop

.ready_state_mix:
	mov ecx, -1			; i = -1

.state_mixing_loop:
	inc ecx				; ++i
	cmp ecx, 256
	jnl .restore_stack

	;
	; j = (key[i % keysize] + j + state[i]) & 255
	;

	; i % keysize
	push ecx
	mov eax, ecx			; eax = i
	mov ecx, DWORD [ebp + 20]	; ecx = keysize
	xor edx, edx
	div ecx				; edx = i % keysize
	pop ecx

	; j += key[i % keysize]
	mov ebx, DWORD [ebp + 16]	; ebx = key
	mov eax, DWORD [esp]		; eax = j
	add al, BYTE [ebx + edx]	; j += key[i % keysize]

	; j += state[i]
	lea ebx, [esp + 8]		; ebx = state
	add al, BYTE [ebx + ecx]	; j += state[i]

	and eax, 255			; j &= 255
	mov DWORD [esp], eax		; save j

	;
	; ft_swap(&state[i], &state[j])
	;

	push ecx
	lea esi, [ebx + edx]
	push esi
	lea esi, [ebx + ecx]
	push esi
	call _ft_swap
	add esp, 8
	pop ecx

	jmp .state_mixing_loop

.restore_stack:
	add esp, 4 + 4 + 256
	pop ebx

.endfunc:
	pop ebp
	ret

; void ft_swap(char *s1, char *s2);
_ft_swap:
	mov edx, DWORD [esp + 4]
	mov ecx, DWORD [esp + 8]
	movzx eax, BYTE [edx]

	xor al, BYTE [ecx]
	mov BYTE [edx], al
	xor al, BYTE [ecx]
	mov BYTE [ecx], al
	xor BYTE [edx], al

	ret
