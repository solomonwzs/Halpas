#ifndef _HASH_H
#define _HASH_H

#include <stdint.h>
#include <stdlib.h>

extern void murmurHash_x86_32(const void *key, int len, uint32_t seed,
        void *out);
extern void murmurHash_x86_128(const void *key, int len, uint32_t seed,
        void *out);
extern void murmurHash_x64_128(const void *key, int len, uint32_t seed,
        void *out);

#endif
