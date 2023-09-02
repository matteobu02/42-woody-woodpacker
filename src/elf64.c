/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf64.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 13:49:18 by mbucci            #+#    #+#             */
/*   Updated: 2023/09/02 01:57:32 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <elf.h>
#include <stdlib.h>

#include <stdio.h>

static Elf64_Shdr *get_section(const Elf64_Ehdr *elf, const char *name)
{
	Elf64_Shdr *sh_tab = (Elf64_Shdr *)((void*)elf + elf->e_shoff);
	Elf64_Shdr *strtab = (sh_tab + elf->e_shstrndx);

	for (int i = 0; i < elf->e_shnum; ++i)
	{
		char *curr = (char *)((void*)elf + strtab->sh_offset + sh_tab[i].sh_name);
		if (curr && !ft_strncmp(curr, name, ft_strlen(name)))
			return &sh_tab[i];
	}
	return NULL;
}

int woody(const void *ptr, unsigned long fsize)
{
	const Elf64_Ehdr *elf = (const Elf64_Ehdr *)ptr;
	if ((elf->e_shoff + (sizeof(Elf64_Shdr) * elf->e_shnum) > fsize)
		|| (elf->e_phoff + (sizeof(Elf64_Phdr) * elf->e_phnum) > fsize)
	)
		return write_error(CORRUPT_ERR);

	if (!get_section(elf, ".shstrtab"))
		return write_error(STRIPPED_ERR);

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
	// BONUS: remove symbols -> compression

	return 0;
}
