/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 12:04:44 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/15 15:35:00 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <elf.h>

int write_error(const char *msg)
{
	if (!msg)
		perror("Error");
	else
		fprintf(stderr, "Error: %s\n", msg);

	return 1;
}

int is_stripped(const Elf64_Ehdr *ptr)
{
	(void)ptr;
	return 0;
}
