/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbucci <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/12 01:51:33 by mbucci            #+#    #+#             */
/*   Updated: 2023/08/12 01:58:46 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int main(int ac, char **av)
{
	if (ac < 2)
	{
		// TODO: tell user to supply one or more argument
		return 1;
	}

	for (int i = 1; i < ac; ++i)
	{
		// TODO: if (check file existance/correctness)
		// TODO: pack av[i]
		;
	}

	return 0;
}
