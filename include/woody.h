/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 02:00:30 by mbucci            #+#    #+#             */
/*   Updated: 2023/09/29 15:54:41 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/* INCLUDES */
#include <elf.h>
#include <stdint.h>
#include "../libft/include/libft.h"

/* CONSTANTS */
#define PATCH "woody"
#define PAYLOAD_SO_PATH "./payloads/parasite_so"
#define PAYLOAD_EXEC_PATH "./payloads/parasite_exec"
#define PAYLOAD_RC4_PATH "./payloads/decryptor"
#define URANDOM "/dev/urandom"
#define KEY_LEN 16

/* ERRORS */
#define USAGE_ERR "supply a 64bit elf biary please"
#define STRIPPED_ERR "file is stripped"
#define CORRUPT_ERR "file is corrupted"
#define ELF_ERR "not an elf file"
#define FORMAT_ERR "file format is not supported"
#define EXEC_ERR "not an executable"
#define PADD_ERR "can't inject payload: file too small"

typedef struct s_woody
{
	char 			*key;
	Elf64_Ehdr		*base;
	unsigned long	size;
}	t_woody;

/* woody.c */
int woody(t_woody *woody);

/* utils.c */
int write_error(const char *msg);
void *read_file(const char *path, uint64_t *fsize);
char *generate_key(void);
