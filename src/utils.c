/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 12:04:44 by mbucci            #+#    #+#             */
/*   Updated: 2023/10/10 17:53:07 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <fcntl.h>
#include "woody.h"

int write_error(const char *msg)
{
	if (!msg)
		perror("Error");
	else
		fprintf(stderr, "Error: %s\n", msg);

	return 1;
}

void *read_file(const char *path, uint64_t *fsize)
{
	int fd = -1;
	if ((fd = open(path, O_RDONLY)) == -1)
	{
		write_error(NULL);
		return NULL;
	}

	int64_t bytes;
	bytes = lseek(fd, 0, SEEK_END);
	if (bytes == -1)
	{
		write_error(NULL);
		close(fd);
		return NULL;
	}

	*fsize = bytes;
	void *file = malloc(sizeof(char) * (*fsize));
	if (file)
	{
		lseek(fd, 0, SEEK_SET);
		bytes = read(fd, file, *fsize);
		if (bytes == -1)
		{
			write_error(NULL);
			free(file);
			file = NULL;
		}
	}
	else
		write_error(NULL);

	close(fd);

	return file;
}

char *generate_key(uint64_t keysz)
{
	int fd = open(URANDOM, O_RDONLY);
	if (fd == -1)
		return NULL;

	char *ret = (char *)malloc(sizeof(char) * keysz);

	if (ret)
	{
		ft_bzero(ret, keysz);
		read(fd, ret, keysz);
	}

	return ret;
}

void print_key(const char *key, uint64_t keysz)
{
	for (uint64_t i = 0; i < keysz; ++i)
		dprintf(STDOUT_FILENO, "%02hhX", (int)key[i]);

	dprintf(STDOUT_FILENO, "\n");
}

void free_payloads(char *k, void *p1, void *p2, void *p3)
{
	free(k);
	free(p1);
	free(p2);
	free(p3);
}
