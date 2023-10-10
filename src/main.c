/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 01:51:33 by mbucci            #+#    #+#             */
/*   Updated: 2023/10/10 17:46:08 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>

// BONUS: write function to decide what arch to handle

static int check_arg(const char *filename, const char *key)
{
	t_woody context = {(key != NULL), NULL, NULL, 0};

	if (key && !(context.key = ft_strdup(key)))
		return write_error(NULL);

	int err = 1;

	if (!(context.base = (Elf64_Ehdr *)read_file(filename, &context.size)))
		return err;

	// check architecture
	const unsigned char *f_ident = context.base->e_ident;
	if (f_ident[EI_MAG0] != ELFMAG0
		|| f_ident[EI_MAG1] != ELFMAG1
		|| f_ident[EI_MAG2] != ELFMAG2
		|| f_ident[EI_MAG3] != ELFMAG3
	)
		write_error(ELF_ERR);

	// check file class
	// BONUS: adapt for 32 bits
	else if (f_ident[EI_CLASS] != ELFCLASS64)
		write_error(FORMAT_ERR);

	// check file type
	else if (context.base->e_type != ET_EXEC && context.base->e_type != ET_DYN)
		write_error(EXEC_ERR);

	// do the thing
	else if (!woody(&context))
		err = 0;

	free((void *)context.base);

	return err;
}

int main(int ac, char **av)
{
	if (ac != 2 && ac != 3)
		return write_error(USAGE_ERR);

	return check_arg(av[1], ( (ac == 3 && ft_strlen(av[2])) ? av[2] : NULL) );
}
