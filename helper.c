#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "helper.h"

void dump(char *s, unsigned char group, unsigned char *b, unsigned int sz)
{
  if (!b || !sz) {
    err("Invalid Arguments");
    return;
  }
  unsigned int i;
  printf("%s\n", s);
  for (i = 0 ; i < sz ; i++) {
    if (group && !(i%group)) {
      printf("\n");
    }
    printf("%02hhx", b[i]);
  }
  printf("\n");
}

static unsigned char to_hex(char c)
{
  if (c > '9')
    c += 9;
  return c & 0xf;
}

buf_t* alloc_buf(unsigned int sz)
{
  buf_t *h = NULL;
  h = malloc(sizeof(*h) + sz);
  if (!h) {
    err("mem alloc failed");
    return NULL;
  }
  //h->val = ((unsigned char *)h) + sizeof(*h);
  h->size = sz;
  return h;
}

buf_t* str_to_hex(char *s, unsigned int sz)
{
  int o = 0;
  buf_t *h;
  unsigned char *t;
  char *tmp;
  if (!s || !sz) {
    err("Invalid Argument");
    return NULL;
  }
  h = alloc_buf((sz + 1)/2);
  t = h->val;
  tmp = s;
  if (sz & 1) {
    t[0] = to_hex(*tmp);
    tmp++;
    t++;
    sz--;
  }
  while(sz && sz - 1) {
//	dbg("t = %p, tmp = %p, sz = %u\n", t, tmp, sz);
    *t = (to_hex(*tmp)  << 4 ) | to_hex(*(tmp + 1));
    t++;
    tmp += 2;
    sz -= 2;
  }
  return h;
}

static const char b64encode_map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char* b64_encode(buf_t *h)
{
	char *d = NULL, *t = NULL;
  unsigned char *v;
	int cnt = ((h->size + 2)/3) * 4;
	int i;

	t = d = malloc(cnt + 1);
	if (!d) {
		err("memory allocation failed\n");
		return NULL;
	}

	for (i = 0, v = h->val ; i < h->size - 2 ; i += 3) {
		*d++ = b64encode_map[v[i] >> 2];
		*d++ = b64encode_map[((v[i] & 0x3) << 4) | ((v[i + 1] >> 4) & 0xf)];
		*d++ = b64encode_map[((v[i + 1] & 0xf) << 2) | ((v[i + 2] >> 6) & 0x3)];
		*d++ = b64encode_map[v[i + 2] & 0x3f];
	}
	if (i < h->size) {
		*d++ = b64encode_map[v[i] >> 2];
		*d++ = b64encode_map[(v[i] & 0x3) << 4];
		if ((i + 1) < h->size) {
			*(d - 1) = b64encode_map[((v[i] & 0x3) << 4) | ((v[i + 1] >> 4) & 0xf)];
			*d++ = b64encode_map[(v[i + 1] & 0xf) << 2];
		} else 
			*d++ = '=';
		*d++ = '=';
	}
	*d = '\0';
	return t;
}

buf_t* b64_decode(char *s)
{
  buf_t *r;
  int i, j, slen, dlen;
  unsigned char b64_rev_map[256] = {0};

  if (!s) {
    err("Invalid argument");
    return NULL;
  }
  slen = strlen(s);
  if(!slen) {
    err("Invalid string to decode");
    return NULL;
  }

  if (slen % 4) {
    err("Input string len not multiple of 4");
    return NULL;
  }

  for (i = 0 ; i < sizeof(b64encode_map); i++) {
    b64_rev_map[b64encode_map[i]] = i;
  }
  dlen = (slen / 4) * 3;

  r = alloc_buf(dlen);
  if (!r) {
    err("buffer allocation failure");
    return NULL;
  }

  for (j = i = 0 ; i < slen ; i+= 4) {
    r->val[j++] = (b64_rev_map[s[i]] << 2) | ((b64_rev_map[s[i + 1]] >> 4) & 3);
    r->val[j++] = (b64_rev_map[s[i + 1]] << 4) | (b64_rev_map[s[i + 2]] >> 2);
    r->val[j++] = ((b64_rev_map[s[i + 2]] & 3) << 6) | b64_rev_map[s[i + 3]];
  }
  return r;
}

buf_t* xor_eq_buf(buf_t *h1, buf_t *h2)
{
	buf_t *r;
	if (!h1 || !h2 || h1->size != h2->size) {
		err("Invalid Arguments");
		return NULL;
	}
	
	r = alloc_buf(h1->size);
	for (int i = 0 ; i < h1->size ; i++) {
		r->val[i] = h1->val[i] ^ h2->val[i];
	}
	return r;
}

buf_t* xor_buf_with_key(buf_t *h, unsigned char c)
{
	buf_t *r;
	if (!h || !h->size) {
		err("Invalid Arguments");
		return NULL;
	}
	
	r = alloc_buf(h->size);
	for (int i = 0 ; i < h->size ; i++) {
		r->val[i] = h->val[i] ^ c;
	}
	return r;
}

buf_t* xor_repeated_key(buf_t *p, buf_t *k)
{
  int i;
  buf_t *r;
  if (!p || !p->size || !k || !k->size) {
    err("Invalid Arguments");
    return NULL;
  }

  r = alloc_buf(p->size);
  if (!r) {
    err("memory allocation failed");
    return NULL;
  }
  for (i = 0; i < p->size ; i++) {
    r->val[i] = p->val[i] ^ k->val[i%k->size];
  }
  return r;
}

static int _ham_dist_arr(unsigned char *b1, unsigned char *b2, unsigned int sz)
{
  int hd = 0;
  unsigned char t = 0;
  for (int i = 0 ; i < sz ; i++) {
    t = b1[i] ^ b2[i];
    while(t) {
      if (t & 1)
        hd++;
      t >>= 1;
    }
  }
  return hd;
}

int ham_dist_arr(unsigned char *b1, unsigned char *b2, unsigned int b1sz, unsigned int b2sz)
{
  if (!b1 || !b2 || !b1sz || (b1sz != b2sz)) {
    err("Invalid arguments");
    return -1;
  }
  return _ham_dist_arr(b1, b2, b1sz);
}


int ham_dist(buf_t *b1, buf_t *b2)
{
  int i, hd = 0;
  if (!b1 || !b2 || !b1->size || (b1->size != b2->size)) {
    err("Invalid arguments");
    return -1;
  }
  return _ham_dist_arr(b1->val, b2->val, b1->size);
}


int cal_score(buf_t *b, double *s)
{
  int i;
  double t_aplha = 0;
  if (!b || !b->size || !s) {
    err("Invalid Arguments");
    return -1;
  }
  for (i = 0 ; i < b->size ; i++) {
    if (isalpha(b->val[i]) || isspace(b->val[i]))
      t_aplha++;
  }
  *s = t_aplha / b->size;
  return 0;
}


int bruteforce_xor_key(buf_t *b, unsigned char *key)
{
  double score;
  const double threshold = .8;
  for (unsigned char i = 0 ; i < 0xFF ; i++) {
    score = 0;
    buf_t *t = xor_buf_with_key(b, i);
    scoped_ptr(t) {
      cal_score(b, &score);
      if (score >= threshold) {
        info("Possible Decryption [score = %f, key = %u]: %s", score, i, t->val);
        *key = i;
      }
    }
  }
  return 0;
}


static int _get_single_key_xor_of_buf(buf_t *b, unsigned char *isenc, unsigned char *key)
{
  const char *freq_map = "ETAOIN SHRDLUetaoinshrdlu";
  unsigned char freq[256] = {0};
  int tmp_max_freq = 0, max_freq_indx = 0;
  int i;
  unsigned char tmpkey;
  double score;
  const double threshold_score = 0.8;
  buf_t *d;

  for (i = 0 ; i < b->size ; i++) {
    freq[b->val[i]]++;
    if (freq[b->val[i]] > tmp_max_freq) {
      tmp_max_freq = freq[b->val[i]];
      max_freq_indx = i;
    }
  }

  for (tmpkey = 0, i = 0 ; i < strlen(freq_map) ; i++) {
      tmpkey = freq_map[i] ^ b->val[max_freq_indx];
      d = xor_buf_with_key(b, tmpkey);
      scoped_ptr(d) { 
        cal_score(d, &score);
        if (score >= threshold_score) {
          //dump_buf("Possible Decryption : ", 0, d->val, d->size);
          //info("Possible Decryption [score = %f, key = %u]: %s", score, tmpkey, d->val);
          *isenc = 1;
          *key = tmpkey;
        } else {
//         dbg("Cannot decrypt - key = %hhx, score = %f", tmpkey, score);
        }
      }
  }
  return 0;
}

int get_single_key_xor_of_buf(buf_t *b, unsigned char *isenc, unsigned char *key)
{
  if (!b || !isenc || !key) {
    err("Invalid argument");
    return -1;
  }
  return _get_single_key_xor_of_buf(b, isenc, key);
}

int get_single_key_xor(char *ip, unsigned char *isenc, unsigned char *key)
{
  buf_t *h;
  if (!ip || !isenc || !key) {
    err("Invalid argument");
    return -1;
  }

  *isenc = 0;
	h = str_to_hex(ip, strlen(ip));
	if (!h)
		return -1;
	//dump_buf("In Hex : ", 0, h->val, h->size);
  return _get_single_key_xor_of_buf(h, isenc, key);
}
