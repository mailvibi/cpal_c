#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "helper.h"

int read_b64str_from_file(const char *filename, char **b)
{
  int ret;
  FILE *f;
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

  f = fopen(filename, "rb");
  if (!f) {
    err("unable to open file %s", filename);
    free(*b);
    return -1;
  }

  char *t = *b;
  while(s.st_size--) {
    int i = fgetc(f);
    if (i == EOF)
      break;
    if (i == '\n' || i == '\r')
      continue;
    *t++ = i;
  }
  *t = '\0';
  return 0;
}


buf_t* aes128_ecb_decrypt(const unsigned char *ctxt, const unsigned int ctxt_size, const unsigned char *key)
{
  EVP_CIPHER_CTX *ctx;
  buf_t *b = NULL, *r = NULL;
  int ret, len = 0;

  ctx = EVP_CIPHER_CTX_new();
  if(!ctx) {
    err("error initializing evp context");
    return NULL;
  }

  ret = EVP_DecryptInit(ctx, EVP_aes_128_ecb(), key, NULL);
  if(ret != 1) {
    err("initializing evp decrypt failed");
    goto free_evp_and_return;
  }
  b = alloc_buf(ctxt_size);
  if (!b) {
    err("allocation buffer of size %d", ctxt_size);
    goto free_evp_and_return;
  }
  
  ret = EVP_DecryptUpdate(ctx, b->val, &len, ctxt, ctxt_size);
  if(ret != 1) {
    err("updating evp decrypt failed");
    goto free_buf_and_return;;
  }
  dbg("len after decrypt update = %d", len);
  b->size = len;

  ret = EVP_DecryptFinal(ctx, b->val + len, &len);
  if(ret != 1) {
    err("finalizing evp decrypt failed : %s", ERR_error_string(ERR_get_error(), NULL));
    goto free_buf_and_return;;
  }
  b->size += len;
  dbg("len after decrypt update final = %d", len);
  return b;
free_buf_and_return:
  free(b);
free_evp_and_return:
  EVP_CIPHER_CTX_free(ctx);
  return r;
}

buf_t* aes128_ecb_decrypt_buf(const buf_t *b, const unsigned char *key)
{
  return aes128_ecb_decrypt(b->val, b->size, key);
}

int main (void)
{
  char *b64str;
  char *filename = "7.txt";
  buf_t *ip, *op;
  int r;
  const unsigned char key[] = {"YELLOW SUBMARINE"};
  int len, ret = -1;
  int ciphertext_len;

  r = read_b64str_from_file(filename, &b64str);
  if (r) {
    err("getting content from file %s failed", filename);
    return -1;
  }
  dbg("read file with size %lu -> %s", strlen(b64str), b64str);
  r = -1;
  ip = b64_decode(b64str);
  free(b64str);
  if (!ip) {
    err("failed to decode");
    goto _exit;
  }
  dbg("decoded buffer size = %d", ip->size);
  op = aes128_ecb_decrypt_buf(ip, key);
  free(ip);
  if (!op) {
    err("error in decrypting ");
    goto _exit;
  }

  info("decrypted text -> %s", op->val);
_exit:
  return r;
}