/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 02:00:30 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/15 15:42:51 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <elf.h>
#include "../libft/include/libft.h"

#define USAGE_ERR "supply a 64bit elf biary please"
#define STRIP_ERR "file is stripped"
#define CORRUPT_ERR "file corrupted"
#define ELF_ERR "not an elf file"
#define FORMAT_ERR "file format is not supported"
#define EXEC_ERR "not an executable"

typedef struct s_packed
{
	const int	fd;
	Elf64_Ehdr	elfhdr;
	Elf64_Phdr	*phdr;
	Elf64_Shdr	*shdr;
}	t_packed;

/* packer.c */
int pack(t_packed *file);

/* parser.c */
int check_arg(const char *filename, t_packed *patch);

/* utils.c */
int write_error(const char *msg);
int is_stripped(const Elf64_Ehdr *ptr);
