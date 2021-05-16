#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define scoped_alloc(_ptr_, _size_) for(int _i_##__LINE__ = (_ptr_ = malloc(_size_), 0); _ptr_ && !_i_##__LINE__ ; _i_##__LINE__++, free(_ptr_), _ptr_ = NULL)

static void _s_alloc(char **p, size_t s)
{
  *p = malloc(s);
  return ;
}

int main(void)
{
  unsigned char *k = NULL;

  scoped_alloc(k, 10) {
    if (k == NULL) {
      printf("k is null\n");
    } else {
      printf("k is not null\n");
    }
  }
  if (k == NULL) {
    printf("k is null\n");
  } else {
    printf("k is not null\n");
  }
}