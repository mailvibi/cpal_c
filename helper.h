#ifndef __HELPER_H__
#define __HELPER_H__

static int __debug__ = 1;

#include <stdio.h>


#define _prn(_x_, ...) do { fprintf(stderr, "["_x_"][%s@%d] : ", __FUNCTION__, __LINE__); \
  fprintf(stderr, __VA_ARGS__); \
  fprintf(stderr, "\n"); \
  } while (0)

#define err(...) _prn("ERR", __VA_ARGS__)
#define info(...) _prn("INFO", __VA_ARGS__)
#define dbg(...) do { if (__debug__) _prn("DBG", __VA_ARGS__); } while (0)

#define scoped_alloc(_ptr_, _size_) for(int _i_##__LINE__ = (_ptr_ = malloc(_size_), 0); _ptr_ && !_i_##__LINE__ ; _i_##__LINE__++, free(_ptr_), _ptr_ = NULL)
#define scoped_ptr(_ptr_) for (int _i_##__LINE__ = 0 ; !_i_##__LINE__ && (_ptr_); _i_##__LINE__++, free(_ptr_))

typedef struct hex {
  int size;
  unsigned char val[];
} buf_t;

//#define xor(_x_, _y_) _Generic((_y_), buf_t* : xor_eq_buf, unsigned char : xor_buf_with_key)

buf_t* alloc_buf(unsigned int sz);
buf_t* str_to_hex(char *str, unsigned int len);
char* b64_encode(buf_t *h);
buf_t* b64_decode(char *b64);
void dump(char *s, unsigned char group, unsigned char *b, unsigned int sz);
buf_t* xor_eq_buf(buf_t *h1, buf_t *h2);
static inline void dump_buf(char *s, unsigned char group, buf_t *h)
{
	dump(s, group, h->val, h->size);
}

buf_t* xor_buf_with_key(buf_t *h, unsigned char c);
buf_t* xor_repeated_key(buf_t *p, buf_t *k);
int ham_dist(buf_t *b1, buf_t *b2);
int ham_dist_arr(unsigned char *b1, unsigned char *b2, unsigned int b1sz, unsigned int b2sz);
int get_single_key_xor(char *ip, unsigned char *isenc, unsigned char *key);

#endif 