/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 02:00:30 by mbucci            #+#    #+#             */
/*   Updated: 2023/09/02 02:38:37 by mbucci           ###   ########.fr       */
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

typedef struct s_woody
{
	int				fd;
	char 			*key;
	const void		*base;
	unsigned long	size;
}	t_woody;

/* elf64.c */
int woody(t_woody *woody);

/* crypto.c */
char *generate_key(void);
void rc4(char *bytes, int size, const char *key);

/* utils.c */
int write_error(const char *msg);
