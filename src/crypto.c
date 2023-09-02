/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   crypto.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 21:17:34 by mbucci            #+#    #+#             */
/*   Updated: 2023/09/02 01:43:18 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

char *generate_key(void)
{
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd != -1)
		return NULL;

	char *ret = (char *)malloc(sizeof(char) * 257);

	if (ret)
		read(fd, ret, 256);

	return ret;
}

void rc4(char *bytes, int size, const char *key)
{
	char state[256] = {0};
	for (int i = 0; i < 256; ++i)
		state[i] = i;

	int j = 0;
	for (int i = 0; i < 256; ++i)
	{
		j = (j + state[i] + key[i % 256]) % 256;
		ft_swap(&state[i], &state[j]);
	}

	j = 0;
	int k = 0;
	for (int i = 0; i < size; ++i)
	{
		k = (k + 1) % 256;
		j = (j + state[k]) % 256;
		ft_swap(&state[k], &state[j]);

		int t = (state[k] + state[j]) % 256;
		bytes[i] = bytes[i] ^ state[t];
	}
}
