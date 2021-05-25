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
#define MAX_SAMPLES (10)

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

static int compare_ham(const void *c1, const void *c2)
{
  const struct key_ham *k1, *k2;
  k1 = c1;
  k2 = c2;
  return (k1->hamming_distance > k2->hamming_distance) - (k2->hamming_distance > k1->hamming_distance);
}

int get_repeating_xor_key(buf_t *b, unsigned int keysize, unsigned char *xor_key)
{
  buf_t *t;
  unsigned int num_blks = 0;
  unsigned char isenc, key;
  int ret = 0;
  if (!b || !keysize || !xor_key) {
    err("Invalid Argument");
    return -1;
  }

  num_blks = b->size/keysize;
  t = alloc_buf(num_blks);
  for (int i = 0; i < keysize ; i++ ) {
    key = isenc = 0;
    for (int j = 0 ; j < num_blks; j++) {
      t->val[j] = b->val[j * keysize + i];
    }
  
    ret = get_single_key_xor_of_buf(t, &isenc, &key);
    if (ret) {
      err("Error in getting key for offset = %d", i);
      break;
    }
    dbg("keysize = %d, i = %d, key = %hhx[%hhd]", keysize, i, key, key);
    xor_key[i] = key;
  }
  free(t);
  return ret;
}

int get_most_probable_repeating_xor_key_size(buf_t *ip, unsigned int *keysize)
{
  /* this arry of struct is not really required - added as help for debugging */
  struct key_ham key_ham_pair[MAX_KEY_SZ - 2]; 
  for (int keysz = 2 ; keysz < MAX_KEY_SZ ; keysz++) {
    key_ham_pair[keysz - 2].keysz = keysz;
    key_ham_pair[keysz - 2].hamming_distance = 0;
    for (int k = 0, thd = 0 ; k < (MAX_SAMPLES * keysz) && ((k + keysz) < ip->size) ; k += keysz) {
      thd = ham_dist_arr(&(ip->val[k]), &(ip->val[k + keysz]), keysz, keysz);
      key_ham_pair[keysz - 2].hamming_distance += thd;
    }
    dbg("For keysize = %d - hd : %d, score = %d", keysz, key_ham_pair[keysz - 2].hamming_distance, key_ham_pair[keysz - 2].hamming_distance/ keysz);
    key_ham_pair[keysz - 2].hamming_distance /= keysz;
  }
  qsort(&key_ham_pair, MAX_KEY_SZ - 2, sizeof(struct key_ham), compare_ham);
//  for (int i = 0 ; i < (MAX_KEY_SZ - 2) ; i++)
//    dbg("map_keysz_ham[%d] {.keysz = %d, .hamming_distance = %d}", i, key_ham_pair[i].keysz, key_ham_pair[i].hamming_distance);
  *keysize = key_ham_pair[0].keysz;
  return 0;  
}

int main(int argc, char **argv)
{
  const char *filename = "./6.txt";
	buf_t *ip, *key;
	char *b64str;
  int ret = 0;
  unsigned int keysize = 0;

  ret = get_file_content(filename, &b64str);
  if (ret < 0) {
    err("error in reading content from file %s", filename);
    return ret;
  }
  ip = b64_decode(b64str);
  if (!ip) {
    err("failed to decode");
    goto err0;
  }
  ret = get_most_probable_repeating_xor_key_size(ip, &keysize);
  if (ret) {
    err("get_most_probable_repeating_xor_key_size failed");
    goto err1;
  }
  dbg("probable keysize = %d", keysize);
  key = alloc_buf(keysize);
  if (!key) {
    err("alloc_buf failed");
    goto err1;
  }
  ret = get_repeating_xor_key(ip, keysize, key->val);
  if (ret) {
    err("get_most_probable_repeating_xor_key_size failed");
    goto err2;
  }
  dump_buf("Key :", 0, key);
  buf_t *dec = xor_repeated_key(ip, key);
  info("Decrypted content : %s", dec->val);
  //dump_buf("converted hex ", 0, ip);

err2 :
  free(key);
err1 :
  free(ip);
err0 :
  free(b64str);
return ret;
}