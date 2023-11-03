void ft_swap(char *s1, char *s2)
{
	//*s1 ^= *s2;
	//*s2 ^= *s1;
	//*s1 ^= *s2;
	char t = *s1;
	*s1 = *s2;
	*s2 = t;
}
