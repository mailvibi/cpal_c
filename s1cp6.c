#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "helper.h"

#define MAX_KEY_SZ (40)
#define MAX_SAMPLES (4)

int get_file_content(const char *filename, char **b)
{
  int ret, fd;
  struct stat s;
  if (!filename || !b ) {
    err("Invalid Arguments");
    return -1;
  }
  ret = stat(filename, &s);
  if (ret < 0) {
    err("unable to find the size of file %s", filename);
    return -1;
  }
  *b = malloc(s.st_size + 1);
  if (!*b) {
    err("unable to allocate memory %ld for contents of file %s", s.st_size, filename);
    return -1;
  }
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    err("unable to open file %s", filename);
    return -1;
  }
  ret = read(fd, *b, s.st_size);
  if (ret != s.st_size) {
    err("unable to copy %ld bytes from file %s. copied only %d bytes", s.st_size, filename, ret);
    return -1;
  }
  (*b)[s.st_size] = '\0';
  return 0;
}

struct key_ham {
  unsigned int keysz;
  unsigned int hamming_distance;
};

static int compare_ham(struct key_ham *k1, struct key_ham *k2)
{
  return (k1->hamming_distance > k2->hamming_distance) - (k2->hamming_distance > k1->hamming_distance);
}

int get_repeating_xor_key(buf_t *b, unsigned int keysize, unsigned char *xor_key)
{
  buf_t *t;
  unsigned int num_blks = 0;
  if (!b || !keysize || !xor_key) {
    err("Invalid Argument");
    return -1;
  }

  num_blks = b->size/keysize;
  t = alloc_buf(num_blks);
  for (int i = 0; i < keysize ; i++ ) {
    for (int j = 0 ; j < num_blks; j++) {
      t->val[j] = b->val[j * keysize + i];
    }
    
  }
  free(t);
  return 0;
}

int main(int argc, char **argv)
{
  const char *filename = "./6.txt";
  buf_t *filecontent;
	buf_t *h, *ip;
	char *b64str;
  int fd, ret;
  struct stat stat;
  const int start_key_len = 0, last_key_len = 0;
  struct key_ham key_ham_pair[MAX_KEY_SZ];
  
  ret = get_file_content(filename, &b64str);
  if (ret < 0) {
    err("error in reading content from file %s", filename);
    return ret;
  }
  ip = b64_decode(b64str);
  //dump_buf("converted hex ", 0, ip);

  memset(&key_ham_pair, 0xFF, sizeof(key_ham_pair));
  for (int keysz = 2 ; keysz < MAX_KEY_SZ ; keysz++) {
    key_ham_pair[keysz - 2].keysz = keysz;
    for (int k = 0, thd = 0 ; k < (MAX_SAMPLES * keysz) && ((k + keysz) < ip->size) ; k += keysz) {
      thd = ham_dist_arr(&(ip->val[k]), &(ip->val[k + keysz]), keysz, keysz);
      //dbg("key = %d - thd : %d", keysz, thd);
      key_ham_pair[keysz - 2].hamming_distance += thd;
    }
    key_ham_pair[keysz - 2].hamming_distance /= MAX_SAMPLES;
    key_ham_pair[keysz - 2].hamming_distance /= keysz;
  }
  for (int i = 0 ; i < MAX_KEY_SZ - 2 ; i++)
    dbg("map_keysz_ham[%d][%d] = %d", i, key_ham_pair[i].keysz, key_ham_pair[i].hamming_distance);
  qsort(&key_ham_pair, MAX_KEY_SZ, sizeof(struct key_ham), compare_ham);
  dbg("++++++++++++++++++++++++++++++++++++");
  for (int i = 0 ; i < MAX_KEY_SZ ; i++)
    dbg("map_keysz_ham[%d][%d] = %d", i, key_ham_pair[i].keysz, key_ham_pair[i].hamming_distance);
  for (int i = 0; ; i++) {
    if (key_ham_pair[i].hamming_distance != key_ham_pair[0].hamming_distance) {
      dbg("key_ham_pair[%d].hamming_distance == %d != key_ham_pair[0].hamming_distance == %d", i, key_ham_pair[i].hamming_distance, key_ham_pair[0].hamming_distance);
      break;
    }
    unsigned int blks = ip->size / key_ham_pair[i].keysz;
    buf_t *tmp = NULL;
    tmp = alloc_buf(ip->size);
    for (int j = 0, k = 0; j < blks ; j++) {
      
    }
  }
#if 0
	//printf("Input  = %s\n", ip);
	h = str_to_hex(ip, strlen(ip));
	if (!h)
		return -1;
	dump_buf("In Hex : ", 0, h->val, h->size);
	b64str = b64_encode(h);
	if (b64str)
		printf("%s\n", b64str);
  buf_t *r = b64_decode(b64str);
	dump_buf("decoded : ", 0, r->val, r->size);
	free(b64str);
	free(h);
	free(r);

  

  int hd;
  char *i1 = "this is a test";
  char *i2 = "wokka wokka!!!";
  h = alloc_buf(strlen(i1));
  strcpy(h->val, i1);
  r = alloc_buf(strlen(i2));
  strcpy(r->val, i2);
  hd = ham_dist(h, r);
  dbg("hamming distance b/w \"%s\" & \"%s\" = %d", i1, i2, hd);

#endif

}