/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 01:51:33 by mbucci            #+#    #+#             */
/*   Updated: 2023/09/26 21:05:48 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>

// BONUS: write function to decide what arch to handle

char *generate_key(void)
{
	int fd = open(URANDOM, O_RDONLY);
	if (fd == -1)
		return NULL;

	char *ret = (char *)malloc(sizeof(char) * KEY_LEN);

	if (ret)
	{
		ft_bzero(ret, KEY_LEN);
		read(fd, ret, KEY_LEN);
	}

	return ret;
}

static int check_arg(const char *filename)
{
	// open file
	int fd;
	if ((fd = open(filename, O_RDONLY)) == -1)
		return write_error(NULL);

	// get file size in bytes with lseek
	long fsize = lseek(fd, 0, SEEK_END);
	if (fsize == -1)
	{
		close(fd);
		return write_error(NULL);
	}

	// load file into memory
	Elf64_Ehdr *base_ptr = (Elf64_Ehdr *)mmap(NULL, fsize, PROT_WRITE, MAP_PRIVATE, fd, 0);

	// close fd cuz we don't need it anymore
	close(fd);

	// check if mapping failed
	if (base_ptr == MAP_FAILED)
		return write_error(NULL);

	t_woody context = {NULL, base_ptr, fsize};
	int err = 1;

	// check architecture
	const unsigned char *f_ident = base_ptr->e_ident;
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
	else if (base_ptr->e_type != ET_EXEC && base_ptr->e_type != ET_DYN)
		write_error(EXEC_ERR);

	// do the thing
	else if (!woody(&context))
		err = 0;

	// unmap file
	if (munmap((void *)base_ptr, fsize) == -1)
		return write_error(NULL);

	return err;
}

int main(int ac, char **av)
{
	if (ac != 2)
		return write_error(USAGE_ERR);

	// BONUS: handle parameter key

	return check_arg(av[1]);
}
