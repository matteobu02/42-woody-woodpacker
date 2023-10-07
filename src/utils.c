/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 12:04:44 by mbucci            #+#    #+#             */
/*   Updated: 2023/10/07 18:32:38 by mbucci           ###   ########.fr       */
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

char *generate_key(void)
{
	int fd = open(URANDOM, O_RDONLY);
	if (fd == -1)
		return NULL;

	char *ret = (char *)malloc(sizeof(char) * KEY_LEN);

	if (ret)
	{
		ft_bzero(ret, KEY_LEN);
		read(fd, ret, KEY_LEN);
	}

	return ret;
}
