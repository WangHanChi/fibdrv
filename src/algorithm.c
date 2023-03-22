// #include <linux/slab.h>
// #include <linux/string.h>
// #include <linux/types.h>

#include "../inc/algorithm.h"


long long original_iter(long long k)
{
    /* FIXME: C99 variable-length array (VLA) is not allowed in Linux kernel. */
    long long f[k + 2];

    f[0] = 0;
    f[1] = 1;

    for (int i = 2; i <= k; i++) {
        f[i] = f[i - 1] + f[i - 2];
    }

    return f[k];
}

inline uint64_t fdoubling_rec(uint32_t target)
{
    if (target <= 2)
        return !!target;

    // fib(2n) = fib(n) * (2 * fib(n+1) − fib(n))
    // fib(2n+1) = fib(n) * fib(n) + fib(n+1) * fib(n+1)
    uint64_t n = fdoubling_rec(target >> 1);
    uint64_t n1 = fdoubling_rec((target >> 1) + 1);

    // check 2n or 2n+1
    if (target & 1)
        return n * n + n1 * n1;
    return n * ((n1 << 1) - n);
}

inline uint64_t fdoubling_iter(uint64_t target)
{
    if (target <= 2)
        return !!target;

    // find first 1
    uint8_t count = 63 - __builtin_clzll(target);
    uint64_t fib_n0 = 1, fib_n1 = 1;

    for (uint64_t i = count, fib_2n0, fib_2n1, mask; i-- > 0;) {
        fib_2n0 = fib_n0 * ((fib_n1 << 1) - fib_n0);
        fib_2n1 = fib_n0 * fib_n0 + fib_n1 * fib_n1;

        mask = -!!(target & (1UL << i));
        fib_n0 = (fib_2n0 & ~mask) + (fib_2n1 & mask);
        fib_n1 = (fib_2n0 & mask) + fib_2n1;
    }
    return fib_n0;
}

/* calc n-th Fibonacci number and save into dest */
void bn_fib(bn *dest, unsigned int n)
{
    bn_resize(dest, 1);
    if (n < 2) {  // Fib(0) = 0, Fib(1) = 1
        dest->number[0] = n;
        return;
    }

    bn *a = bn_alloc(1);
    bn *b = bn_alloc(1);
    dest->number[0] = 1;

    for (unsigned int i = 1; i < n; i++) {
        bn_swap(b, dest);
        bn_add(a, b, dest);
        bn_swap(a, b);
    }
    bn_free(a);
    bn_free(b);
}

void bn_fib_fdoubling(bn *dest, unsigned int n)
{
    bn_resize(dest, 1);
    if (n < 2) {  // Fib(0) = 0, Fib(1) = 1
        dest->number[0] = n;
        return;
    }

    bn *f1 = dest;        /* F(k) */
    bn *f2 = bn_alloc(1); /* F(k+1) */
    f1->number[0] = 0;
    f2->number[0] = 1;
    bn *k1 = bn_alloc(1);
    bn *k2 = bn_alloc(1);

    for (unsigned int i = 1U << (31 - __builtin_clz(n)); i; i >>= 1) {
        /* F(2k) = F(k) * [ 2 * F(k+1) – F(k) ] */
        bn_cpy(k1, f2);
        bn_lshift2(k1, 1);
        bn_sub(k1, f1, k1);
        bn_mult(k1, f1, k1);
        /* F(2k+1) = F(k)^2 + F(k+1)^2 */
        bn_mult(f1, f1, f1);
        bn_mult(f2, f2, f2);
        bn_cpy(k2, f1);
        bn_add(k2, f2, k2);
        if (n & i) {
            bn_cpy(f1, k2);
            bn_cpy(f2, k1);
            bn_add(f2, k2, f2);
        } else {
            bn_cpy(f1, k1);
            bn_cpy(f2, k2);
        }
    }
    bn_free(f2);
    bn_free(k1);
    bn_free(k2);
}

void bn_fib_fdoubling_v1(bn *dest, unsigned int n)
{
    bn_resize(dest, 1);
    if (n < 2) {  // Fib(0) = 0, Fib(1) = 1
        dest->number[0] = n;
        return;
    }

    bn *f1 = dest;        /* F(k) */
    bn *f2 = bn_alloc(1); /* F(k+1) */
    f1->number[0] = 0;
    f2->number[0] = 1;
    bn *k1 = bn_alloc(1);
    bn *k2 = bn_alloc(1);

    for (unsigned int i = 1U << (31 - __builtin_clz(n)); i; i >>= 1) {
        /* F(2k) = F(k) * [ 2 * F(k+1) – F(k) ] */
        /* F(2k+1) = F(k)^2 + F(k+1)^2 */
        bn_lshift3(f2, 1, k1);  // k1 = 2 * F(k+1)
        bn_sub(k1, f1, k1);     // k1 = 2 * F(k+1) – F(k)
        bn_mult(k1, f1, k2);    // k2 = k1 * f1 = F(2k)
        bn_mult(f1, f1, k1);    // k1 = F(k)^2
        bn_swap(f1, k2);        // f1 <-> k2, f1 = F(2k) now
        bn_mult(f2, f2, k2);    // k2 = F(k+1)^2
        bn_add(k1, k2, f2);     // f2 = f1^2 + f2^2 = F(2k+1) now
        if (n & i) {
            bn_swap(f1, f2);     // f1 = F(2k+1)
            bn_add(f1, f2, f2);  // f2 = F(2k+2)
        }
    }
    bn_free(f2);
    bn_free(k1);
    bn_free(k2);
}
