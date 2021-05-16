#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"

int main(void)
{
	char *hstr1 = "1c0111001f010100061a024b53535009181c", *hstr2 = "686974207468652062756c6c277320657965";
	buf_t *h1, *h2, *r;
	h1 = str_to_hex(hstr1, strlen(hstr1));
	h2 = str_to_hex(hstr2, strlen(hstr2));
	r = xor_eq_buf(h1, h2);
	dump_buf("Result : ", 0, r);
	
	free(h1);
	free(h2);
	free(r);
}