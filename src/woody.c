/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 13:49:18 by mbucci            #+#    #+#             */
/*   Updated: 2023/09/29 12:11:04 by mbucci           ###   ########.fr       */
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
Elf64_Addr g_parasite_addr = 0;
Elf64_Addr g_decryptor_addr = 0;

Elf64_Off g_parasite_off = 0;
Elf64_Off g_decryptor_off = 0;

int64_t g_parasite_size = 0;
int64_t g_decryptor_size = 0;
int64_t g_total_payload_size = 0;

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
	Elf64_Phdr *phtab = (Elf64_Phdr *)(ptr + elf->e_phoff);

	for (uint16_t i = 0; i < elf->e_phnum; ++i)
	{
		if (phtab[i].p_type == PT_INTERP)
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

			g_parasite_off = ph_tab[i].p_offset + ph_tab[i].p_filesz;
			g_parasite_addr = ph_tab[i].p_vaddr + ph_tab[i].p_filesz;

			g_decryptor_off = g_parasite_off + g_parasite_size;
			g_decryptor_addr = g_parasite_addr + g_parasite_size;

			ph_tab[i].p_filesz += g_total_payload_size;
			ph_tab[i].p_memsz += g_total_payload_size;
		}

		if (found_code && ph_tab[i].p_type == PT_LOAD && ph_tab[i].p_flags == (PF_R | PF_W)) 
			return (ph_tab[i].p_offset - g_parasite_off);
	}

	return 0;
}

static void adjust_section_offsets(const Elf64_Ehdr *elf)
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

//static void adjust_payload_addr32(char *bytes, int32_t target, int32_t repl)
//{
//	for (int i = 0; i < g_parasite_size; ++i)
//	{
//		int32_t chunk = *(int32_t *)(bytes + i);
//		if (!(chunk ^ target))
//		{
//			*(int32_t *)(bytes + i) = repl;
//			return;
//		}
//	}
//}

static void adjust_payload_addr64(char *bytes, int64_t target, int64_t repl)
{
	for (int i = 0; i < g_parasite_size; ++i)
	{
		int64_t chunk = *(int64_t *)(bytes + i);
		if (!(chunk ^ target))
		{
			*(int64_t *)(bytes + i) = repl;
			return;
		}
	}
}

void print_key(const char *key)
{
	for (int i = 0; i < KEY_LEN; ++i)
		dprintf(STDOUT_FILENO, "%02hhX", (int)key[i]);

	dprintf(STDOUT_FILENO, "\n");
}

static void *get_payload(const char *path, int64_t *fsize)
{
	int payloadfd = -1;
	if ((payloadfd = open(path, O_RDONLY)) == -1)
		return NULL;

	*fsize = lseek(payloadfd, 0, SEEK_END);
	if (*fsize == -1)
	{
		close(payloadfd);
		return NULL;
	}

	void *payload = malloc(*fsize);
	if (payload)
	{
		lseek(payloadfd, 0, SEEK_SET);
		int readbytes = read(payloadfd, payload, *fsize);
		if (readbytes < 1)
		{
			free(payload);
			payload = NULL;
		}

		close(payloadfd);
	}

	return payload;
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

	// choose parasite depending on elf type
	const char *payload_path = NULL;
	if (context->base->e_type == ET_DYN)
		payload_path = PAYLOAD_SO_PATH;
	else
		payload_path = PAYLOAD_EXEC_PATH;

	// get parasite
	void *parasite = get_payload(payload_path, &g_parasite_size);
	if (!parasite)
	{
		free(context->key);
		return write_error(NULL);
	}

	// get decryptor
	void *decryptor = get_payload(PAYLOAD_RC4_PATH, &g_decryptor_size);
	if (!decryptor)
	{
		free(parasite);
		free(context->key);
		return write_error(NULL);
	}
	g_total_payload_size = g_parasite_size + (sizeof(char) * KEY_LEN) + g_decryptor_size;

	// get padding
	Elf64_Off padding = get_padding(context->base);
	if (padding < (Elf64_Off)g_total_payload_size)
	{
		free(parasite);
		free(decryptor);
		free(context->key);
		return write_error(PADD_ERR);
	}

	// save old entrypoint
	Elf64_Addr code_entry = context->base->e_entry;

	// set entrypoint to start of payload
	if (context->base->e_type == ET_EXEC)
		context->base->e_entry = g_parasite_addr;//g_decryptor_addr;
	else
		context->base->e_entry = g_parasite_off;//g_decryptor_off;

	// patch parasite
	adjust_payload_addr64(parasite, 0x4242424242424242, code_entry);

	// patch decryptor
	//Elf64_Addr keyaddr = g_decryptor_addr - KEY_LEN;

	//printf("keyaddr: %ld\n", keyaddr);
	//printf("text addr: %ld\n", g_codeseg_addr);
	//printf("text len: %ld\n", keyaddr - g_codeseg_addr);

	//adjust_payload_addr32(decryptor, 0x42194219, KEY_LEN);
	//adjust_payload_addr64(decryptor, 0x1942194219421942, keyaddr);
	//adjust_payload_addr64(decryptor, 0x1919191919191919, keyaddr - g_codeseg_addr);
	//adjust_payload_addr64(decryptor, 0x4242424242424242, g_codeseg_addr);
	//adjust_payload_addr64(decryptor, 0x1919191919424242, g_parasite_addr);

	// patch code section header
	adjust_section_offsets(context->base);

	// inject parasite
	void *inject_addr = (void *)context->base + g_parasite_off;
	ft_memmove(inject_addr, parasite, g_parasite_size);

	// inject key
	inject_addr += g_parasite_size;
	ft_memmove(inject_addr, context->key, KEY_LEN);

	// inject decryptor
	inject_addr += KEY_LEN;
	ft_memmove(inject_addr, decryptor, g_decryptor_size);

	free(parasite);
	free(decryptor);

	// encrypt code segment
	//_rc4((void *)context->base + 0, 0, context->key, KEY_LEN);

	// create woody
	int woody_fd = -1;
	if ((woody_fd = open(PATCH, (O_CREAT | O_WRONLY | O_TRUNC), 0755)) == -1)
	{
		free(context->key);
		return write_error(NULL);
	}

	// time to print the key
	print_key(context->key);

	// TODO: strip file
	// dump file into woody
	write(woody_fd, (void *)context->base, context->size);

	close(woody_fd);
	free(context->key);

	return 0;
}
