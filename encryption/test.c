#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY "FS3L23KVFGTSW"
#define TEXT "this is my text"

extern	void _rc4_32(void *bytes, long length, const char *key, int keysize);

void ft_swap(char *s1, char *s2)
{
	char tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
}

void rc4(char *bytes, long length, const char *key, int keysz)
{
	unsigned char state[256] = {0};
	for (int i = 0; i < 256; ++i)
		state[i] = i;

	int j = 0;
	for (int i = 0; i < 256; ++i)
	{
		j = (j + state[i] + key[i % keysz]) & 255;
		printf("j[%d]: %d\n", i, j);
		ft_swap((char *)&state[i], (char *)&state[j]);
	}

	j = 0;
	int k = 0;
	for (int i = 0; i < length; ++i)
	{
		k = (k + 1) % 256;
		j = (j + state[k]) % 256;

		ft_swap((char *)&state[k], (char *)&state[j]);

		int t = (state[k] + state[j]) % 256;

		bytes[i] = bytes[i] ^ state[t];
	}
}

void printtext(char *bytes, long size)
{
	for (int i = 0; i < size; ++i)
		printf("\\%2hhx", bytes[i]);
	printf("\n");
}

int main(void)
{
	char *msg1 = strdup(TEXT);
	char *msg2 = strdup(TEXT);

	if (!msg1 || !msg2)
		return 1;

	long length = strlen(TEXT);
	printf("key: %s\n", KEY);
	printf("original: %s\n\n", msg1);

	rc4(msg1, length, KEY, strlen(KEY));
	_rc4_32(msg2, length, KEY, strlen(KEY));

	//printf("c:   ");
	//printtext(msg1, length);

	//printf("asm: ");
	//printtext(msg2, length);

	//rc4(msg1, length, KEY, strlen(KEY));
	//_rc4_32(msg2, length, KEY, strlen(KEY));

	//printf("\ndecrypted: %s\n", msg2);
	//printf("decryption: %s\n", (!memcmp(msg1, msg2, length) ? "OK" : "KO"));

	free(msg1);
	free(msg2);

	return 0;
}
