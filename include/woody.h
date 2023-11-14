/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <mbucci@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 13:06:21 by mbucci            #+#    #+#             */
/*   Updated: 2023/11/07 15:04:21 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/* INCLUDES */
#include <stdint.h>
#include "../libft/include/libft.h"

/* CONSTANTS */
#define PATCH "woody"
#define DFLT_KEY_LEN 16
#define URANDOM "/dev/urandom"

/* PAYLOADS */
#define HANDLER_ELF64_PATH "./payloads/handler_elf64.bin"
#define PARASITE_ELF64_PATH "./payloads/parasite_elf64.bin"
#define DECRYPTOR_ELF64_PATH "./payloads/rc4_elf64.bin"

#define HANDLER_ELF32_PATH "./payloads/handler_elf32.bin"
#define PARASITE_ELF32_PATH "./payloads/parasite_elf32.bin"
#define DECRYPTOR_ELF32_PATH "./payloads/rc4_elf32.bin"

/* ERRORS */
#define USAGE_ERR "usage: ./woody_woodpacker <path/to/binary> <key>"
#define KEYGEN_ERR "could not generate key"
#define LOAD_ERR "no loadable segment found"
#define PADD_ERR "can't inject payload: padding too small"
#define CORRUPT_ERR "file is corrupted"
#define ELF_ERR "not an elf file"
#define FORMAT_ERR "file format is not supported"
#define ELFEXEC_ERR "not an elf executable"

typedef struct s_woody
{
	const char	*param_name;
	int8_t		keyisparam;
	char 		*key;
	void		*base;
	uint64_t	size;
}	t_woody;

/* elf64.c */
int woody_elf64(t_woody *woody);

/* elf32.c */
int woody_elf32(t_woody *woody);

/* utils.c */
int write_error(const char *filename, const char *custmsg);
void *read_file(const char *path, uint64_t *fsize);
char *generate_key(uint32_t keysz);
void print_key(const char *key, uint32_t keysz);
void patch_payload_addr32(char *bytes, uint64_t size, int32_t target, int32_t repl);
void patch_payload_addr64(char *bytes, uint64_t size, int64_t target, int64_t repl);
void free_payloads(char *k, void *p1, void *p2, void *p3);
