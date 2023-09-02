/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 13:49:18 by mbucci            #+#    #+#             */
/*   Updated: 2023/09/02 14:31:07 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <elf.h>
#include <stdlib.h>
#include <fcntl.h>

#include <stdio.h>

static int check_boundaries(const t_woody *context)
{
	Elf64_Ehdr *elf = (Elf64_Ehdr *)context->base;

	if (   (elf->e_shoff + (sizeof(Elf64_Shdr) * elf->e_shnum) > context->size)
		|| (elf->e_phoff + (sizeof(Elf64_Phdr) * elf->e_phnum) > context->size)
		|| ((elf->e_phentsize * elf->e_phnum) > (sizeof(Elf64_Phdr) * elf->e_phnum))
		|| ((elf->e_shentsize * elf->e_shnum) > (sizeof(Elf64_Shdr) * elf->e_shnum))
		|| (elf->e_shoff + elf->e_shstrndx > context->size)
	)
		return 1;

	Elf64_Shdr *sh_tab = (Elf64_Shdr *)((void*)elf + elf->e_shoff);
	Elf64_Shdr *strtab = sh_tab + elf->e_shstrndx;
	for (int i = 0; i < elf->e_shnum; ++i)
		if (strtab->sh_offset + sh_tab[i].sh_name > context->size)
			return 1;

	return 0;
}

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

int woody(t_woody *woody)
{
	//const Elf64_Ehdr *elf = (const Elf64_Ehdr *)woody->base;

	if (check_boundaries(woody))
		return write_error(CORRUPT_ERR);

	// create woody
	if ((woody->fd = open("woody", (O_CREAT | O_WRONLY | O_TRUNC), 0755)) == -1)
		return write_error(NULL);

	// TODO: copy elf header

	// TODO: copy program headers
	//Elf64_Phdr *ph_tab = (Elf64_Phdr *)(ptr + elf->e_phoff);
	//for (uint16_t i = 0; i < elf->e_phnum; ++i)
	//	ft_memmove(&(patch->phdr[i]), &ph_tab[i], sizeof(Elf64_Phdr));

	// TODO: copy until .text section
	//Elf64_Shdr *sh_tab = (Elf64_Shdr *)(ptr + elf->e_shoff);
	//for (uint16_t i = 0; i < elf->e_shnum; ++i)
	//	ft_memmove(&(patch->shdr[i]), &sh_tab[i], sizeof(Elf64_Shdr));

	// TODO: encrypt and copy .text section
	// TODO: copy rest
	// BONUS: remove .symtab, .strtab, .debug* -> compression/stripping

	return 0;
}
