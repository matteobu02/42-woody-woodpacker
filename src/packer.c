/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 12:01:59 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/14 17:48:03 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <sys/mman.h>
#include <stdlib.h>
//#include <elf.h>
//#include <stdint.h>

int pack(t_packed *file)
{
	(void)file;
	// TODO: locate .text section
	// TODO: add loader
	// TODO: encrypt .text section

	return 0;
}
