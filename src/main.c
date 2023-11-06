/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 01:51:33 by mbucci            #+#    #+#             */
/*   Updated: 2023/11/06 13:11:09 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>

static int check_arg(const char *filename, const char *key)
{
	t_woody context = {(key != NULL), NULL, NULL, 0};

	if (key && !(context.key = ft_strdup(key)))
		return write_error(NULL);

	int err = 1;

	if (!(context.base = read_file(filename, &context.size)))
		return err;

	// check architecture
	Elf64_Ehdr *elf = (Elf64_Ehdr *)context.base;
	const unsigned char *f_ident = elf->e_ident;
	if (f_ident[EI_MAG0] != ELFMAG0
		|| f_ident[EI_MAG1] != ELFMAG1
		|| f_ident[EI_MAG2] != ELFMAG2
		|| f_ident[EI_MAG3] != ELFMAG3
	)
		write_error(ELF_ERR);

	// check file class
	else if (f_ident[EI_CLASS] != ELFCLASS64 && f_ident[EI_CLASS] != ELFCLASS32)
		write_error(FORMAT_ERR);

	// check file type
	else if (elf->e_type != ET_EXEC && elf->e_type != ET_DYN)
		write_error(EXEC_ERR);

	else // do the thing
	{
		if (f_ident[EI_CLASS] == ELFCLASS64)
			err = woody_elf64(&context);
		//else if (f_ident[EI_CLASS] == ELFCLASS32)
		//	err = woody_elf32(&context);
	}

	free(context.base);

	return err;
}

int main(int ac, char **av)
{
	if (ac != 2 && ac != 3)
		return write_error(USAGE_ERR);

	return check_arg(av[1], ( (ac == 3 && ft_strlen(av[2])) ? av[2] : NULL) );
}
