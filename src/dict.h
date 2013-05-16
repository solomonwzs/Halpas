#ifndef _DICT_H
#define _DICT_H

#include <stdint.h>
#include <stdlib.h>

typedef struct dictEntry{
    void *key;
    union {
        void *val;
        uint64_t ui;
        int64_t si;
    } v;
    struct dictEntry *next;
} dictEntry;

typedef struct dictFunc{
    void (*hashfunc)(void *privdata, const void *key);
    void (*keyfreefunc)(void *privdata, void *key);
    void (*valfreefunc)(void *privdata, void *val);
    void *(*keydupfunc)(void *privdata, void *key);
    void *(*valdupfunc)(void *privdata, void *val);
} dictFunc;

typedef struct dictHashTable{
    unsigned long size;
    unsigned long used;
    dictEntry **table;
} dictHashTable;

typedef struct dict{
    void *privdata;
    struct dictFunc *func;
    dictHashTable ht[2];
} dict;

extern dict *dictCreate(dictFunc *dictFunc, void *privdata);
extern void dictFree(dict *d);

#endif
