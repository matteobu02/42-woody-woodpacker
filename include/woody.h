/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 02:00:30 by mbucci            #+#    #+#             */
/*   Updated: 2023/09/02 01:40:31 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <elf.h>
#include <stdint.h>
#include "../libft/include/libft.h"

#define USAGE_ERR "supply a 64bit elf biary please"
#define STRIPPED_ERR "file is stripped"
#define CORRUPT_ERR "file is corrupted"
#define ELF_ERR "not an elf file"
#define FORMAT_ERR "file format is not supported"
#define EXEC_ERR "not an executable"

typedef struct s_packed
{
	int		fd;
	char 	*key;
}	t_packed;

/* elf64.c */
int woody(const void *ptr, unsigned long fsize);

/* crypto.c */
void encrypt_sects(t_packed *file);

/* utils.c */
int write_error(const char *msg);
int is_stripped(const Elf64_Ehdr *ptr, unsigned long fsize);
