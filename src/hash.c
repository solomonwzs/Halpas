#include "hash.h"

#define _rotl32(_i_, _rl_) \
    (((_i_)<<(_rl_))|((_i_)>>(32-(_rl_))))

#define _rotl64(_i_, _rl_) \
    (((_i_)<<(_rl_))|((_i_)>>(64-(_rl_))))

#define _getblock32(_p_, _i_) \
    ((_p_)[(_i_)])

#define _getblock64(_p_, _i_) \
    ((_p_)[(_i_)])

#define _fmix32(_h_) do{\
    (_h_)^=(_h_)>>16; \
    (_h_)*=0x85ebca6b; \
    (_h_)^=(_h_)>>13; \
    (_h_)*=0xc2b2ae35; \
    (_h_)^=(_h_)>>16; \
} while(0)

#define _fmix64(_h_) do{\
    (_h_)^=(_h_)>>33; \
    (_h_)*=0xff51afd7ed558ccd; \
    (_h_)^=(_h_)>>33; \
    (_h_)*=0xc4ceb9fe1a85ec53; \
    (_h_)^=(_h_)>>33; \
} while(0)

void murmurHash_x86_32(const void *key, int len, uint32_t seed, void *out){
    const uint8_t *data=(const uint8_t *)key;
    const int nblocks=len/4;
    const uint32_t c1=0xcc9e2d51;
    const uint32_t c2=0x1b873593;
    const uint32_t n1=0xe6546b64;
    const uint32_t *blocks=(const uint32_t *)(data+nblocks*4);

    const uint8_t *tail=(const uint8_t *)(data+nblocks*4);

    int i;
    uint32_t h1=seed, k1;

    for (i=-nblocks; i; ++i){
        k1=_getblock32(blocks, i);

        k1*=c1;
        k1=_rotl32(k1, 15);
        k1*=c2;

        h1^=k1;
        h1=_rotl32(h1, 13);
        h1=h1*5+n1;
    }

    k1=0;
    switch(len&3){
        case 3: k1^=tail[2]<<16;
        case 2: k1^=tail[1]<<8;
        case 1: k1^=tail[0];
                k1*=c1; k1=_rotl32(k1, 15); k1*=c2; h1^=k1;
    }

    h1^=len;
    _fmix32(h1);
    *(uint32_t *)out=h1;
}

static const uint32_t n_x86_128[4]={0x561ccd1b, 0x0bcaa747, 0x96cd1c35, 0x32ac3b17};
static const uint32_t c_x86_128[4]={0x239b961b, 0xab0e9789, 0x38b34ae5, 0xa1e38b93};

void murmurHash_x86_128(const void *key, int len, uint32_t seed, void *out){
    const uint8_t *data=(const uint8_t *)key;
    const int nblocks=len/16;

    uint32_t h[4]={seed, seed, seed, seed};

    const uint32_t *blocks=(const uint32_t *)(data+nblocks*16);
    const uint8_t *tail=(const uint8_t *)(data+nblocks*16);

    int i, j, index;
    uint32_t k[4];

    for (i=-nblocks; i; ++i){
        for (j=0; j<4; ++j){
            k[j]=_getblock32(blocks, i*4+j);
        }
        for (j=0; j<4; ++j){
            index=(j+1)%4;
            k[j]*=c_x86_128[j]; k[j]=_rotl32(k[j], 15+j);
            k[j]*=c_x86_128[index]; h[j]^=k[j];
            h[j]=_rotl32(h[j], 19-i*2); h[j]+=h[index]; h[j]=h[j]*5+n_x86_128[j];
        }
    }

    k[0]=0; k[1]=0; k[2]=0; k[3]=0;
    switch(len&15){
        case 15: k[3]^=tail[14]<<16;
        case 14: k[3]^=tail[13]<<8;
        case 13: k[3]^=tail[12]<<0;
                 k[3]*=c_x86_128[3]; k[3]=_rotl32(k[3], 18);
                 k[3]*=c_x86_128[0]; h[3]^=k[3];

        case 12: k[2]^=tail[11]<<24;
        case 11: k[2]^=tail[10]<<16;
        case 10: k[2]^=tail[ 9]<<8;
        case  9: k[2]^=tail[ 8]<<0;
                 k[2]*=c_x86_128[2]; k[2]=_rotl32(k[2], 17);
                 k[2]*=c_x86_128[3]; h[2]^=k[2];

        case  8: k[1]^=tail[ 7]<<24;
        case  7: k[1]^=tail[ 6]<<16;
        case  6: k[1]^=tail[ 5]<<8;
        case  5: k[1]^=tail[ 4]<<0;
                 k[1]*=c_x86_128[1]; k[1]=_rotl32(k[1], 16);
                 k[1]*=c_x86_128[2]; h[1]^=k[1];

        case  4: k[0]^=tail[ 3]<<24;
        case  3: k[0]^=tail[ 2]<<16;
        case  2: k[0]^=tail[ 1]<<8;
        case  1: k[0]^=tail[ 0]<<0;
                 k[0]*=c_x86_128[0]; k[0]=_rotl32(k[0], 15);
                 k[0]*=c_x86_128[1]; h[0]^=k[0];
    }

    for (i=0; i<4; ++i) h[i]^=len;

    h[0]+=(h[1]+h[2]+h[3]);
    h[1]+=h[0];
    h[2]+=h[0];
    h[3]+=h[0];

    for (i=0; i<4; ++i) _fmix32(h[i]);

    h[0]+=(h[1]+h[2]+h[3]);
    h[1]+=h[0];
    h[2]+=h[0];
    h[3]+=h[0];

    for (i=0; i<4; ++i) ((uint32_t *)out)[i]=h[i];
}

static const uint64_t c1_x64_128=0x87c37b91114253d5llu;
static const uint64_t c2_x64_128=0x4cf5ad432745937fllu;
static const uint64_t n1_x64_128=0x52dce729;
static const uint64_t n2_x64_128=0x38495ab5;

void murmurHash_x64_128(const void *key, int len, uint32_t seed, void *out){
    const uint8_t *data=(const uint8_t *)key;
    const int nblocks=len/16;

    uint64_t h1=seed;
    uint64_t h2=seed;
    uint64_t k1, k2;

    const uint64_t *blocks=(const uint64_t *)data;
    const uint8_t *tail=(const uint8_t *)(data+nblocks*16);

    int i;

    for (i=0; i<nblocks; ++i){
        k1=_getblock64(blocks, i*2);
        k2=_getblock64(blocks, i+2+1);

        k1*=c1_x64_128; k1=_rotl64(k1, 31); k1*=c2_x64_128; h1^=k1;
        h1=_rotl64(h1, 27); h1+=h2; h1=h1*5+n1_x64_128;

        k2*=c2_x64_128; k2=_rotl64(k1, 33); k2*=c1_x64_128; h2^=k2;
        h2=_rotl64(h2, 31); h2+=h1; h2=h2*5+n2_x64_128;
    }

    k1=0; k2=0;
    switch(len&15){
        case 15: k2^=((uint64_t)tail[14])<<48;
        case 14: k2^=((uint64_t)tail[13])<<40;
        case 13: k2^=((uint64_t)tail[12])<<32;
        case 12: k2^=((uint64_t)tail[11])<<24;
        case 11: k2^=((uint64_t)tail[10])<<16;
        case 10: k2^=((uint64_t)tail[ 9])<<8;
        case  9: k2^=((uint64_t)tail[ 8])<<0;
                 k2*=c2_x64_128; k2=_rotl64(k2, 33); k2*=c1_x64_128; h2^=k2;
        case  8: k1^=((uint64_t)tail[ 7])<<56;
        case  7: k1^=((uint64_t)tail[ 6])<<48;
        case  6: k1^=((uint64_t)tail[ 5])<<40;
        case  5: k1^=((uint64_t)tail[ 4])<<32;
        case  4: k1^=((uint64_t)tail[ 3])<<24;
        case  3: k1^=((uint64_t)tail[ 2])<<16;
        case  2: k1^=((uint64_t)tail[ 1])<<8;
        case  1: k1^=((uint64_t)tail[ 0])<<0;
                 k1*=c1_x64_128; k1=_rotl64(k1, 31); k1*=c2_x64_128; h1^=k1;
    }

    h1^=len; h2^=len;
    h1+=h2; h2+=h1;
    _fmix64(h1); _fmix64(h2);
    h1+=h2; h2+=h1;

    ((uint64_t*)out)[0]=h1;
    ((uint64_t*)out)[1]=h2;
}
