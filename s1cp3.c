#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "helper.h"

struct key_score {
  unsigned char key;
  double score;
};


int main(void)
{
	char *ip = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
  char *freq_map = "ETAOIN SHRDLUetaoinshrdlu";
  struct key_score keyscores[strlen(freq_map)];
  double base_score = .90;
	buf_t *h, *d;
	char *b64str;
  char freq[256] = {0};
  int tmp_max_freq = 0, max_freq_indx = 0;
  int i;
  unsigned char tmpkey;

	//printf("Input  = %s\n", ip);
	h = str_to_hex(ip, strlen(ip));
	if (!h)
		return -1;
	//dump_buf("In Hex : ", 0, h->val, h->size);

  for (i = 0 ; i < h->size ; i++) {
    freq[h->val[i]]++;
    if (freq[h->val[i]] > tmp_max_freq) {
      tmp_max_freq = freq[h->val[i]];
      max_freq_indx = i;
    }
  }
  for (tmpkey = 0, i = 0 ; i < strlen(freq_map) ; i++) {
      tmpkey = freq_map[i] ^ h->val[max_freq_indx];
      d = xor_buf_with_key(h, tmpkey);
      keyscores[i].key = tmpkey;
      cal_score(d, &keyscores[i].score);
      if (keyscores[i].score > .80) {
        dump_buf("Possible Decryption : ", 0, d);
        info("Possible Decryption : %s", d->val);
      } else
        dbg("Cannot decrypt - key = %hhx, score = %f", tmpkey, keyscores[i].score);
      free(d);
  }

}
  