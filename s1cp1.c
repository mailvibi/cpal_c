#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"

int main(void)
{
	char *ip = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
	buf_t *h;
	char *b64str;

	printf("Input  = %s\n", ip);
	h = str_to_hex(ip, strlen(ip));
	if (!h)
		return -1;
	dump_buf("In Hex : ", 0, h);
	b64str = b64_encode(h);
	if (b64str)
		printf("%s\n", b64str);
	free(b64str);
	free(h);
}