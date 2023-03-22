#ifndef __ALGORITHM_H__
#define __ALGORITHM_H__

#include "bignum.h"
// #include <stdint.h>


enum algorithm {
    _original_iter = 0,
    _fdoubling_rec,
    _fdoubling_iter,
    _bn_fib,
    _bn_fib_fdoubling,
    _bn_fib_fdoubling_v1,
    _bn_fib_fdoubling_v2
};

long long original_iter(long long k);
inline uint64_t fdoubling_rec(uint32_t target);
inline uint64_t fdoubling_iter(uint64_t target);
void bn_fib(bn *dest, unsigned int n);
void bn_fib_fdoubling(bn *dest, unsigned int n);
void bn_fib_fdoubling_v1(bn *dest, unsigned int n);

#endif /* __ALGORITHM_H__ */