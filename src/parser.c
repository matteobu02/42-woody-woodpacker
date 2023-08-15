/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 13:49:18 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/15 15:51:45 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int copy_arg(t_packed *patch, const Elf64_Ehdr *ptr, unsigned long fsize)
{
	(void)patch;
	//const Elf64_Shdr *sect_tab = (Elf64_Shdr *)ptr + ptr->e_shoff;
	if ((ptr->e_shoff + (sizeof(Elf64_Shdr) * ptr->e_shnum) > fsize)
		|| (ptr->e_phoff + (sizeof(Elf64_Phdr) * ptr->e_phnum) > fsize)
	)
		return write_error(CORRUPT_ERR);

	// check if file is stripped
	if (is_stripped(ptr))
		return write_error(STRIP_ERR);

	return 0;
}

int check_arg(const char *filename, t_packed *patch)
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
	const Elf64_Ehdr *base_ptr = (const Elf64_Ehdr *)mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, fd, 0);

	// close fd cuz we don't need it anymore
	close(fd);

	// check if mapping failed
	if (base_ptr == MAP_FAILED)
		return write_error(NULL);

	int err = 1;
	// check architecture
	const unsigned char *f_ident = base_ptr->e_ident;
	if (f_ident[EI_MAG0] != ELFMAG0
		|| f_ident[EI_MAG1] != ELFMAG1
		|| f_ident[EI_MAG2] != ELFMAG2
		|| f_ident[EI_MAG3] != ELFMAG3
	)
		write_error(ELF_ERR);

	else if (base_ptr->e_shoff + (sizeof(Elf64_Shdr) * base_ptr->e_shnum) > (unsigned)fsize)
		write_error(CORRUPT_ERR);

	// check file class
	// TODO: change for bonus
	else if (f_ident[EI_CLASS] != ELFCLASS64)
		write_error(FORMAT_ERR);

	// check file type
	else if (base_ptr->e_type != ET_EXEC && base_ptr->e_type != ET_DYN)
		write_error(EXEC_ERR);

	// copy file to local struct
	else if (!copy_arg(patch, base_ptr, fsize))
		err = 0;

	// unmap file
	if (munmap((void *)base_ptr, fsize) == -1)
		return write_error(NULL);

	return err;
}
