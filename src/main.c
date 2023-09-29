/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 01:51:33 by mbucci            #+#    #+#             */
/*   Updated: 2023/09/29 15:53:53 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>

// BONUS: write function to decide what arch to handle

static int check_arg(const char *filename)
{
	//// open file
	//int fd;
	//if ((fd = open(filename, O_RDONLY)) == -1)
	//	return write_error(NULL);

	//// get file size in bytes with lseek
	//long fsize = lseek(fd, 0, SEEK_END);
	//if (fsize == -1)
	//{
	//	close(fd);
	//	return write_error(NULL);
	//}

	//// load file into memory
	//Elf64_Ehdr *base_ptr = (Elf64_Ehdr *)mmap(NULL, fsize, PROT_WRITE, MAP_PRIVATE, fd, 0);

	//// close fd cuz we don't need it anymore
	//close(fd);

	//// check if mapping failed
	//if (base_ptr == MAP_FAILED)
	//	return write_error(NULL);

	t_woody context = {NULL, NULL, 0};
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

	// unmap file
	//if (munmap((void *)context.base, fsize) == -1)
	//	return write_error(NULL);
	free((void *)context.base);

	return err;
}

int main(int ac, char **av)
{
	if (ac != 2)
		return write_error(USAGE_ERR);

	// BONUS: handle parameter key

	return check_arg(av[1]);
}
