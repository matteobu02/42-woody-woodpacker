/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 02:00:30 by mbucci            #+#    #+#             */
/*   Updated: 2023/11/06 13:13:38 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/* INCLUDES */
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
#define LOAD_ERR "no loadable segment found"
#define PADD_ERR "can't inject payload: padding too small"
#define CORRUPT_ERR "file is corrupted"
#define ELF_ERR "not an elf file"
#define FORMAT_ERR "file format is not supported"
#define EXEC_ERR "not an elf executable"

typedef struct s_woody
{
	int			keyisparam;
	char 		*key;
	void		*base;
	uint64_t	size;
}	t_woody;

/* elf64.c */
int woody_elf64(t_woody *woody);

/* utils.c */
int write_error(const char *msg);
void *read_file(const char *path, uint64_t *fsize);
char *generate_key(uint32_t keysz);
void print_key(const char *key, uint32_t keysz);
void patch_payload_addr32(char *bytes, uint64_t size, int32_t target, int32_t repl);
void patch_payload_addr64(char *bytes, uint64_t size, int64_t target, int64_t repl);
void free_payloads(char *k, void *p1, void *p2, void *p3);
