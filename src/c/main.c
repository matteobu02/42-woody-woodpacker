/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <mbucci@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:38:28 by mbucci            #+#    #+#             */
/*   Updated: 2023/11/07 13:04:37 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>

static int check_arg(const char *filename, const char *key)
{
	t_woody context = {filename, (key != NULL), NULL, NULL, 0};

	if (key && !(context.key = ft_strdup(key)))
		return write_error(KEYGEN_ERR, NULL);

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
		write_error(filename, ELF_ERR);

	// check file class
	else if (f_ident[EI_CLASS] != ELFCLASS64 && f_ident[EI_CLASS] != ELFCLASS32)
		write_error(filename, FORMAT_ERR);

	// check file type
	else if (elf->e_type != ET_EXEC && elf->e_type != ET_DYN)
		write_error(filename, ELFEXEC_ERR);

	else // do the thing
	{
		if (f_ident[EI_CLASS] == ELFCLASS64)
			err = woody_elf64(&context);
		else if (f_ident[EI_CLASS] == ELFCLASS32)
			err = woody_elf32(&context);

		// TODO: handle macho
	}

	free(context.base);

	return err;
}

int main(int ac, char **av)
{
	if (ac != 2 && ac != 3)
		return write_error(NULL, USAGE_ERR);

	return check_arg(av[1], ( (ac == 3 && ft_strlen(av[2])) ? av[2] : NULL) );
}
