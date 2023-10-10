/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 13:49:18 by mbucci            #+#    #+#             */
/*   Updated: 2023/10/10 14:31:41 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <elf.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>

/* Import ASM Function */
extern void _rc4(void *, long, const char *, int);

/* Global Variables */
Elf64_Addr g_codeseg_addr = 0;
Elf64_Addr g_handler_addr = 0;
Elf64_Addr g_parasite_addr = 0;
Elf64_Addr g_decryptor_addr = 0;

Elf64_Off g_codeseg_off = 0;
Elf64_Off g_handler_off = 0;
Elf64_Off g_parasite_off = 0;
Elf64_Off g_decryptor_off = 0;

uint64_t g_codeseg_size = 0;
uint64_t g_handler_size = 0;
uint64_t g_parasite_size = 0;
uint64_t g_decryptor_size = 0;
uint64_t g_total_payload_size = 0;

/* CODE */
static int check_boundaries(const t_woody *context)
{
	Elf64_Ehdr *elf = (Elf64_Ehdr *)context->base;

	if ((elf->e_shoff + (sizeof(Elf64_Shdr) * elf->e_shnum) > context->size)
		|| (elf->e_phoff + (sizeof(Elf64_Phdr) * elf->e_phnum) > context->size)
		|| ((elf->e_phentsize * elf->e_phnum) > (sizeof(Elf64_Phdr) * elf->e_phnum))
		|| ((elf->e_shentsize * elf->e_shnum) > (sizeof(Elf64_Shdr) * elf->e_shnum))
		|| (elf->e_shoff + elf->e_shstrndx > context->size)
		|| (elf->e_ehsize > sizeof(Elf64_Ehdr))
	)
		return 1;

	Elf64_Shdr *sh_tab = (Elf64_Shdr *)((void*)elf + elf->e_shoff);
	Elf64_Shdr *strtab = sh_tab + elf->e_shstrndx;
	for (int i = 0; i < elf->e_shnum; ++i)
		if (strtab->sh_offset + sh_tab[i].sh_name > context->size)
			return 1;

	return 0;
}

static int check_exec(const void *ptr)
{
	Elf64_Ehdr *elf = (Elf64_Ehdr *)ptr;
	Elf64_Phdr *ph_tab = (Elf64_Phdr *)(ptr + elf->e_phoff);

	for (uint16_t i = 0; i < elf->e_phnum; ++i)
	{
		if (ph_tab[i].p_type == PT_INTERP)
			return 0;
	}

	return 1;
}

static Elf64_Off get_padding(const Elf64_Ehdr *elf)
{
	const void *ptr = (void *)elf;
	Elf64_Phdr *ph_tab = (Elf64_Phdr *)(ptr + elf->e_phoff);
	uint8_t found_code = 0;

	for (uint16_t i = 0; i < elf->e_phnum; ++i)
	{
		if (!found_code && ph_tab[i].p_type == PT_LOAD && ph_tab[i].p_flags == (PF_R | PF_X))
		{
			found_code = 1;

			g_codeseg_addr = ph_tab[i].p_vaddr;
			g_codeseg_off = ph_tab[i].p_offset;
			g_codeseg_size = ph_tab[i].p_filesz;

			g_parasite_off = g_codeseg_off + g_codeseg_size;
			g_parasite_addr = g_codeseg_addr + g_codeseg_size;

			g_decryptor_off = g_parasite_off + g_parasite_size;
			g_decryptor_addr = g_parasite_addr + g_parasite_size;

			g_handler_off = g_decryptor_off + g_decryptor_size;
			g_handler_addr = g_decryptor_addr + g_decryptor_size;

			ph_tab[i].p_filesz += g_total_payload_size;
			ph_tab[i].p_memsz += g_total_payload_size;
			ph_tab[i].p_flags |= PF_W;
		}

		if (found_code && ph_tab[i].p_type == PT_LOAD && ph_tab[i].p_flags == (PF_R | PF_W)) 
			return (ph_tab[i].p_offset - g_parasite_off);
	}

	return 0;
}

static void patch_section_offsets(const Elf64_Ehdr *elf)
{
	const void *ptr = (void *)elf;
	Elf64_Shdr *sh_tab = (Elf64_Shdr *)(ptr + elf->e_shoff);

	for (uint16_t i = 0; i < elf->e_shnum; ++i)
	{
		if (sh_tab[i].sh_offset + sh_tab[i].sh_size == g_parasite_off)
		{
			sh_tab[i].sh_size += g_total_payload_size;
			return ;
		}
	}
}

static void patch_payload_addr32(char *bytes, uint64_t size, int32_t target, int32_t repl)
{
	for (uint32_t i = 0; i < size; ++i)
	{
		int32_t chunk = *(int32_t *)(bytes + i);
		if (!(chunk ^ target))
		{
			*(int32_t *)(bytes + i) = repl;
			return;
		}
	}
}

static void patch_payload_addr64(char *bytes, uint64_t size, int64_t target, int64_t repl)
{
	for (uint64_t i = 0; i < size; ++i)
	{
		int64_t chunk = *(int64_t *)(bytes + i);
		if (!(chunk ^ target))
		{
			*(int64_t *)(bytes + i) = repl;
			return;
		}
	}
}

int woody(t_woody *context)
{
	if (check_boundaries(context))
		return write_error(CORRUPT_ERR);

	if (check_exec((void *)context->base))
		return write_error(EXEC_ERR);

	// BONUS: handle paramterized key
	// generate key
	if (!(context->key = generate_key()))
		return write_error(NULL);

	// get injection handler
	void *handler = read_file(HANDLER_PATH, &g_handler_size);
	if (!handler)
	{
		free_payloads(context->key, NULL, NULL, NULL);
		return 1;
	}

	// get parasite
	void *parasite = read_file(PARASITE_PATH, &g_parasite_size);
	if (!parasite)
	{
		free_payloads(context->key, handler, NULL, NULL);
		return 1;
	}

	// get decryptor
	void *decryptor = read_file(DECRYPTOR_PATH, &g_decryptor_size);
	if (!decryptor)
	{
		free_payloads(context->key, handler, parasite, NULL);
		return 1;
	}

	g_total_payload_size = g_parasite_size
						+ g_decryptor_size
						+ g_handler_size
						+ (sizeof(char) * KEY_LEN);

	// get padding
	Elf64_Off padding = get_padding(context->base);
	if (padding < (Elf64_Off)g_total_payload_size)
	{
		free_payloads(context->key, handler, parasite, decryptor);
		return write_error(PADD_ERR);
	}

	// create woody
	int woody_fd = -1;
	if ((woody_fd = open(PATCH, (O_CREAT | O_WRONLY | O_TRUNC), 0755)) == -1)
	{
		free_payloads(context->key, handler, parasite, decryptor);
		return write_error(NULL);
	}

	// time to print the key
	print_key(context->key);

	// save old entrypoint
	Elf64_Addr code_entry = context->base->e_entry;

	// set entrypoint to start of payload
	if (context->base->e_type == ET_EXEC)
		context->base->e_entry = g_handler_addr;
	else
		context->base->e_entry = g_handler_off;

	// patch code section header
	patch_section_offsets(context->base);

	// inject parasite
	void *inject_addr = (void *)context->base + g_parasite_off;
	ft_memmove(inject_addr, parasite, g_parasite_size);

	// inject decryptor
	inject_addr += g_parasite_size;
	ft_memmove(inject_addr, decryptor, g_decryptor_size);

	// inject handler
	inject_addr += g_decryptor_size;
	ft_memmove(inject_addr, handler, g_handler_size);

	// inject key
	inject_addr += g_handler_size;
	ft_memmove(inject_addr, context->key, KEY_LEN);

	// patch handler
	patch_payload_addr32((void *)context->base + g_handler_off, g_handler_size, 0x55555555, KEY_LEN); // key size
	patch_payload_addr64((void *)context->base + g_handler_off, g_handler_size, 0x4444444444444444, g_handler_off + g_handler_size); // offset to key
	patch_payload_addr64((void *)context->base + g_handler_off, g_handler_size, 0x1942194219421942, g_codeseg_size + g_parasite_size); // text size
	patch_payload_addr64((void *)context->base + g_handler_off, g_handler_size, 0x2222222222222222, g_codeseg_off); // text offset
	patch_payload_addr64((void *)context->base + g_handler_off, g_handler_size, 0x1919191919191919, g_decryptor_off); // offset to rc4

	if (context->base->e_type == ET_EXEC)
	{
		patch_payload_addr64((void *)context->base + g_handler_off, g_handler_size, 0x4242424242424242, g_parasite_addr); // offset to parasite
		patch_payload_addr64((void *)context->base + g_handler_off, g_handler_size, 0xAAAAAAAAAAAAAAAA, 0);
	}
	else
	{
		patch_payload_addr64((void *)context->base + g_handler_off, g_handler_size, 0x4242424242424242, g_parasite_off); // offset to parasite
		patch_payload_addr64((void *)context->base + g_handler_off, g_handler_size, 0xAAAAAAAAAAAAAAAA, 1);
	}

	patch_payload_addr64((void *)context->base + g_handler_off, g_handler_size, 0x3333333333333333, code_entry);

	// encrypt code segment
	_rc4((void *)context->base + g_codeseg_off, g_codeseg_size + g_parasite_size, context->key, KEY_LEN);

	free_payloads(context->key, handler, parasite, decryptor);

	// TODO: strip file

	// dump file into woody
	write(woody_fd, (void *)context->base, context->size);

	close(woody_fd);

	return 0;
}
