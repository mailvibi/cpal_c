#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "helper.h"

int main(int argc, char **argv)
{
  buf_t *key, *p, *r;
  char *ip = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
  key = alloc_buf(3);
  strncpy((char*)key->val, "ICE", 3);

  p = alloc_buf(strlen(ip));
  if (!p) {
    err("Memory Allocation");
    return -1;
  }
  strcpy((char*)p->val, ip);
  r = xor_repeated_key(p, key);
  if (!r) {
    return -1;
  }
  dump_buf("Input : ", 0, p);
  dump_buf("Repeated Key XOR : ", 0, r);
}