/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 01:51:33 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/14 02:33:04 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/mman.h>

int check_arg(t_arg *arg)
{
	// open file
	int fd;
	if ((fd = open(arg->name, O_RDONLY)) == -1)
		return write_error(NULL);

	// get file size in bytes with lseek
	arg->bsize = lseek(fd, 0, SEEK_END);
	if (arg->bsize == -1)
		return write_error(NULL);

	// load file into memory
	arg->base_ptr = (const Elf64_Ehdr *)mmap(NULL, arg->bsize, PROT_READ, MAP_PRIVATE, fd, 0);

	// close fd cuz we don't need it anymore
	close(fd);

	// check if mapping failed
	if (arg->base_ptr == MAP_FAILED)
		return write_error(NULL);


	// check architecture
	const unsigned char *f_ident = arg->base_ptr->e_ident;
	if (!(f_ident[EI_MAG0] == ELFMAG0
		&& f_ident[EI_MAG1] == ELFMAG1
		&& f_ident[EI_MAG2] == ELFMAG2
		&& f_ident[EI_MAG3] == ELFMAG3)
	)
		return write_error("not an elf file");

	// check/get file architecture
	if (f_ident[EI_CLASS] == ELFCLASSNONE)
		return write_error("file format not supported");
	arg->arch = f_ident[EI_CLASS] == ELFCLASS64 ? 64 : 32;

	return 0;
}

int main(int ac, char **av)
{
	if (ac != 2)
		return write_error("supply an elf biary please");

	t_arg arg;
	arg.name = av[1];

	if (!check_arg(&arg))
		return pack(&arg);

	return 1;
}
