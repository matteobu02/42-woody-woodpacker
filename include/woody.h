/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 02:00:30 by mbucci            #+#    #+#             */
/*   Updated: 2023/10/10 17:52:29 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/* INCLUDES */
#include <elf.h>
#include <stdint.h>
#include "../libft/include/libft.h"

/* CONSTANTS */
#define PATCH "woody"
#define HANDLER_PATH "./payloads/handler"
#define PARASITE_PATH "./payloads/parasite"
#define DECRYPTOR_PATH "./payloads/decryptor"
#define URANDOM "/dev/urandom"
#define DFLT_KEY_LEN 16

/* ERRORS */
#define USAGE_ERR "usage: ./woody_woodpacker <path/to/binary> <key>"
#define CORRUPT_ERR "file is corrupted"
#define ELF_ERR "not an elf file"
#define FORMAT_ERR "file format is not supported"
#define EXEC_ERR "not an executable"
#define PADD_ERR "can't inject payload: file too small"

typedef struct s_woody
{
	int				keyisparam;
	char 			*key;
	Elf64_Ehdr		*base;
	unsigned long	size;
}	t_woody;

/* woody.c */
int woody(t_woody *woody);

/* utils.c */
int write_error(const char *msg);
void *read_file(const char *path, uint64_t *fsize);
char *generate_key(uint64_t keysz);
void print_key(const char *key, uint64_t keysz);
void free_payloads(char *k, void *p1, void *p2, void *p3);
