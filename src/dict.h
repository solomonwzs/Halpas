#ifndef _DICT_H
#define _DICT_H

#include <stdint.h>
#include <stdlib.h>

#define DICT_OPT_OK 0
#define DICT_OPT_ERR 1

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
    unsigned long (*hashfunc)(void *privdata, const void *key);
    int (*keycmpfunc)(void *privdata, const void *key1, const void *key2);
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
    uint8_t rehashing;
    void *privdata;
    struct dictFunc func;
    dictHashTable ht[2];
} dict;

extern dict *dictCreate(dictFunc dictfunc, void *privdata, unsigned long size);
extern void dictFree(dict *d);
extern dictEntry *dictAddRaw(dict *d, void *key);
extern dictEntry *dictFind(dict *d, const void *key);
extern int dictDelete(dict *d, const void *key, int freeval);

#endif
