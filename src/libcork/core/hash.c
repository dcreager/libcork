/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2011, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include "libcork/core/hash.h"
#include "libcork/core/types.h"

/*
 * We currently use MurmurHash3 [1], which is public domain, as our hash
 * implementation.
 *
 * [1] http://code.google.com/p/smhasher/
 */

#define ROTL32(a,b) (((a) << ((b) & 0x1f)) | ((a) >> (32 - ((b) & 0x1f))))

static inline uint32_t fmix(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

cork_hash_t
cork_hash_buffer(cork_hash_t seed, const void *src, size_t len)
{
    const uint8_t  *data = (const uint8_t *) src;
    const int  nblocks = len / 4;

    uint32_t  h1 = seed;

    uint32_t  c1 = 0xcc9e2d51;
    uint32_t  c2 = 0x1b873593;

    //----------
    // body

    const uint32_t  *blocks = (const uint32_t *) (data + nblocks*4);
    int  i;

    for (i = -nblocks; i != 0; i++) {
        uint32_t  k1 = blocks[i];

        k1 *= c1;
        k1 = ROTL32(k1,15);
        k1 *= c2;

        h1 ^= k1;
        h1 = ROTL32(h1,13);
        h1 = h1*5+0xe6546b64;
    }

    //----------
    // tail

    const uint8_t  *tail = (const uint8_t *) (data + nblocks*4);

    uint32_t  k1 = 0;

    switch (len & 3)
    {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
                k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
    };

    //----------
    // finalization

    h1 ^= len;

    h1 = fmix(h1);
    return h1;
}
