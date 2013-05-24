#ifndef _DICT_H
#define _DICT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define _DICT_DEBUG

#define DICT_OPT_ERR 0
#define DICT_OPT_OK 1

#define DICT_ENTRY_TYPE_UNKNOWN 0x00
#define DICT_ENTRY_TYPE_POINT 0x01
#define DICT_ENTRY_TYPE_UINT 0x02
#define DICT_ENTRY_TYPE_INT 0x03
#define DICT_ENTRY_TYPE_FLOAT 0x04

#define dictEntryType(_entry_) \
    ((_entry_)->type)

#define dictFreeKey(_d_, _entry_) do{\
    if ((_d_)->func->keyfreefunc) \
        (_d_)->func->keyfreefunc((_d_)->privdata, (_entry_)->key);\
} while(0)

#define dictSetKey(_d_, _entry_, _key_) do{\
    if ((_d_)->func->keydupfunc) \
        (_entry_)->key=(_d_)->func->keydupfunc((_d_)->privdata, _key_); \
    else \
        (_entry_)->key=(_key_);\
} while(0)

#define dictFreeVal(_d_, _entry_) do{\
    if ((_d_)->func->valfreefunc && (_entry_)->type==DICT_ENTRY_TYPE_POINT) \
        (_d_)->func->valfreefunc((_d_)->privdata, (_entry_)->value.point);\
} while(0)

#define dictSetFloat(_entry_, _f_) do{\
    (_entry_)->type=DICT_ENTRY_FLOAT; \
    (_entry_)->value.f=(_f_); \
} while(0)

#define dictSetUnsigedInteger(_entry_, _ui_) do{\
    (_entry_)->type=DICT_ENTRY_TYPE_UINT; \
    (_entry_)->value.ui=(_ui_); \
} while(0)

#define dictSetSigedInteger(_entry_, _si_) do{\
    (_entry_)->type=DICT_ENTRY_TYPE_INT; \
    (_entry_)->value.si=(_si_); \
} while(0)

#define dictSetPoint(_d_, _entry_, _p_) do{\
    (_entry_)->type=DICT_ENTRY_TYPE_POINT; \
    if ((_d_)->func->valdupfunc) \
        (_entry_)->value.point=(_d_)->func->valdupfunc((_d_)->privdata, _p_); \
    else \
        (_entry_)->value.point=(_p_);\
} while(0)

#define dictKeyCompare(_d_, _key1_, _key2_) \
    (((_d_)->func->keycmpfunc)? \
        (_d_)->func->keycmpfunc((_d_)->privdata, _key1_, _key2_): \
        (_key1_)!=(_key2_))

#define dictRehashing(_d_) \
    ((_d_)->rehashing!=0)

typedef struct dictEntry{
    uint8_t type;
    void *key;
    union {
        void *point;
        uint64_t ui;
        int64_t si;
        double f;
    } value;
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
    struct dictFunc *func;
    dictHashTable ht[2];
} dict;

extern dict *dictCreate(dictFunc *dictfunc, void *privdata, unsigned long size);
extern void dictFree(dict *d);
extern dictEntry *dictAddRaw(dict *d, void *key);
extern dictEntry *dictFind(dict *d, const void *key);
extern int dictDelete(dict *d, const void *key, int freeval);

#endif
