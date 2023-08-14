/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 02:00:30 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/14 02:31:16 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <elf.h>

typedef struct s_arg
{
	const char			*name;
	long				bsize;
	const Elf64_Ehdr	*base_ptr;
	int					arch;
}	t_arg;

int check_arg(t_arg *file);

/* packer.c */
int pack(t_arg *file);

/* utils.c */
int write_error(const char *msg);
