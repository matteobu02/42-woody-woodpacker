/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 13:49:18 by mbucci            #+#    #+#             */
/*   Updated: 2023/09/02 15:22:53 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <elf.h>
#include <stdlib.h>
#include <fcntl.h>

#include <stdio.h>

//static Elf64_Shdr *get_section(const t_woody *context, const char *name)
//{
//	Elf64_Ehdr *elf = (Elf64_Ehdr *)context->base;
//	Elf64_Shdr *sh_tab = (Elf64_Shdr *)((void*)elf + elf->e_shoff);
//	Elf64_Shdr *strtab = sh_tab + elf->e_shstrndx;
//
//	for (int i = 0; i < elf->e_shnum; ++i)
//	{
//		char *curr = (char *)((void*)elf + strtab->sh_offset + sh_tab[i].sh_name);
//		if (curr && !ft_strncmp(curr, name, ft_strlen(name)))
//			return &sh_tab[i];
//	}
//	return NULL;
//}

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

static void copy_elf_header(const t_woody *context)
{
	Elf64_Ehdr cpy;
	ft_memmove(&cpy, context->base, context->base->e_ehsize);

	// TODO: recalculate entry

	write(context->fd, &cpy, sizeof(Elf64_Ehdr));
}

static void copy_program_headers(const t_woody *context)
{
	uint16_t phnum = context->base->e_phnum;
	Elf64_Phdr cpy[phnum];
	Elf64_Phdr *ph_tab = (Elf64_Phdr *)((void *)context->base + context->base->e_phoff);

	for (uint16_t i = 0; i < phnum; ++i)
		ft_memmove(&(cpy[i]), &ph_tab[i], sizeof(Elf64_Phdr));
}

static void copy_section_headers(const t_woody *context)
{
	(void)context;
	// TODO: copy until .text section
	// TODO: encrypt and copy .text section
	// TODO: copy rest
	// BONUS: remove .symtab, .strtab, .debug* -> compression/stripping

	//Elf64_Shdr *sh_tab = (Elf64_Shdr *)(ptr + elf->e_shoff);
	//for (uint16_t i = 0; i < elf->e_shnum; ++i)
	//	ft_memmove(&(patch->shdr[i]), &sh_tab[i], sizeof(Elf64_Shdr));
}

int woody(t_woody *context)
{
	if (check_boundaries(context))
		return write_error(CORRUPT_ERR);

	// create woody
	if ((context->fd = open("woody", (O_CREAT | O_WRONLY | O_TRUNC), 0755)) == -1)
		return write_error(NULL);

	copy_elf_header(context);
	copy_program_headers(context);
	copy_section_headers(context);

	return 0;
}
