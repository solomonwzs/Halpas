#ifndef _DICT_H
#define _DICT_H

#include <stdint.h>
#include <stdlib.h>

typedef struct dictEntry{a
    void *key;
    union {
        void *val;
        uint64_t ui;
        int64_t si;
    } v;
    struct dictEntry *next;
} dictEntry;

typedef struct dictFunc{
} dictFunc;

typedef struct dict{
} dict;

#endif
