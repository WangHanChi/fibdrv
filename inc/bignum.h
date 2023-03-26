#ifndef __BIGNUM_H__
#define __BIGNUM_H__
#include <linux/types.h>
// #include <stddef.h>

/* number[size - 1] = msb, number[0] = lsb */
typedef struct _bn {
    unsigned int *number;
    unsigned int size;
    unsigned int capacity; /* total allocated length, size <= capacity */
    int sign;
} bn;

/* define */
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#ifndef SWAP
#define SWAP(x, y)           \
    do {                     \
        typeof(x) __tmp = x; \
        x = y;               \
        y = __tmp;           \
    } while (0)
#endif

#ifndef DIV_ROUNDUP
#define DIV_ROUNDUP(x, len) (((x) + (len) -1) / (len))
#endif


#define INIT_ALLOC_SIZE 8
#define ALLOC_CHUNK_SIZE 8

/* function */

char *bn_to_string(bn src);
void bn_add(const bn *a, const bn *b, bn *c);
void bn_sub(const bn *a, const bn *b, bn *c);
void bn_do_add(const bn *a, const bn *b, bn *c);
void bn_do_sub(const bn *a, const bn *b, bn *c);
void bn_mult(const bn *a, const bn *b, bn *c);
void bn_mult_add(bn *c, int offset, unsigned long long int x);
void bn_lshift2(bn *src, size_t shift);
void bn_lshift3(const bn *src, size_t shift, bn *dest);
void bn_swap(bn *a, bn *b);
int bn_cmp(const bn *a, const bn *b);
int bn_resize(bn *src, size_t size);
int bn_clz(const bn *src);
int bn_msb(const bn *src);
bn *bn_alloc(size_t size);
int bn_free(bn *a);

/* function for fast doubling */
int bn_cpy(bn *dest, bn *src);

#endif  // __BIGNUM_H__