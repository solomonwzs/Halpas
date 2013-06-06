#ifndef _DICT_H
#define _DICT_H

#include <stdio.h>
#include "base.h"

#define _DICT_DEBUG

#define DICT_OPT_ERR 0
#define DICT_OPT_OK 1

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
    if ((_d_)->func->valfreefunc && (_entry_)->value.type==ENTRY_TYPE_POINT) \
        (_d_)->func->valfreefunc((_d_)->privdata, (_entry_)->value.val.point);\
} while(0)

#define dictSetFloat(_entry_, _f_) do{\
    (_entry_)->value.type=ENTRY_FLOAT; \
    (_entry_)->value.val.f=(_f_); \
} while(0)

#define dictSetUnsigedInteger(_entry_, _ui_) do{\
    (_entry_)->value.type=ENTRY_TYPE_UINT; \
    (_entry_)->value.val.ui=(_ui_); \
} while(0)

#define dictSetSigedInteger(_entry_, _si_) do{\
    (_entry_)->value.type=ENTRY_TYPE_INT; \
    (_entry_)->value.val.si=(_si_); \
} while(0)

#define dictSetPoint(_d_, _entry_, _p_) do{\
    (_entry_)->value.type=ENTRY_TYPE_POINT; \
    if ((_d_)->func->valdupfunc) \
        (_entry_)->value.val.point=\
            (_d_)->func->valdupfunc((_d_)->privdata, _p_); \
    else \
        (_entry_)->value.val.point=(_p_);\
} while(0)

#define dictKeyCompare(_d_, _key1_, _key2_) \
    (((_d_)->func->keycmpfunc)? \
        (_d_)->func->keycmpfunc((_d_)->privdata, _key1_, _key2_): \
        (_key1_)!=(_key2_))

#define dictRehashing(_d_) ((_d_)->rehashing!=0)

#define dictEntryType(_entry_) (_entry_)->value.type
#define dictEntryUInt(_entry_) (_entry_)->value.val.ui
#define dictEntryInt(_entry_) (_entry_)->value.val.si
#define dictEntryFloat(_entry_) (_entry_)->value.val.f
#define dictEntryPoint(_entry_) (_entry_)->value.val.point

typedef struct dictEntry{
    void *key;
    entryValue value;
    struct dictEntry *next;
} dictEntry;

typedef struct dictHashTable{
    unsigned long size;
    unsigned long used;
    dictEntry **table;
} dictHashTable;

typedef struct dict{
    uint8_t rehashing;
    void *privdata;
    struct entryFunc *func;
    dictHashTable ht[2];
} dict;

extern dict *dictCreate(entryFunc *dictfunc, void *privdata, unsigned long size);
extern void dictFree(dict *d);
extern dictEntry *dictAddRaw(dict *d, void *key);
extern dictEntry *dictFind(dict *d, const void *key);
extern int dictDelete(dict *d, const void *key, int freeval);

#endif
