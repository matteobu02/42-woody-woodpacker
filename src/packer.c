/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 12:01:59 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/14 02:33:36 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <sys/mman.h>
#include <stdlib.h>
//#include <elf.h>
//#include <stdint.h>

int pack(t_arg *file)
{
	(void)file;

	// TODO: copy original binary into new mapping

	// unmap memory used for file
	if (munmap((void *)file->base_ptr, file->bsize) == -1)
		return write_error(NULL);

	// TODO: add loader
	// TODO: encrypt .text section

	return 0;
}
