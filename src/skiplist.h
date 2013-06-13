#ifndef _SKIP_LIST_H
#define _SKIP_LIST_H

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "base.h"

#define SKIPLIST_OPT_ERR 0
#define SKIPLIST_OPT_OK 1

#define skiplistNodeType(_n_) (_n_)->value.type
#define skiplistNodeUInt(_n_) (_n_)->value.val.ui
#define skiplistNodeInt(_n_) (_n_)->value.val.si
#define skiplistNodeFloat(_n_) (_n_)->value.val.f
#define skiplistNodePoint(_n_) (_n_)->value.val.point

#define skiplistFreeKey(_sl_, _node_) do{\
    if ((_sl_)->func->keyfreefunc) \
        (_sl_)->func->keyfreefunc((_sl_)->privdata, (_node_)->key);\
} while(0)

#define skiplistFreeVal(_sl_, _node_) do{\
    if ((_sl_)->func->valfreefunc && (_node_)->value.type==ENTRY_TYPE_POINT) \
        (_sl_)->func->valfreefunc((_sl_)->privdata, \
                (_node_)->value.val.point);\
} while(0)

#define skiplistKeyCompare(_sl_, _key1_, _key2_) \
    (((_sl_)->func->keycmpfunc)? \
        (_sl_)->func->keycmpfunc((_sl_)->privdata, _key1_, _key2_): \
        (_key1_)!=(_key2_))

#define skiplistSetFloat(_node_, _f_) do{\
    (_node_)->value.type=ENTRY_TYPE_FLOAT; \
    (_node_)->value.val.f=(_f_); \
} while(0)

#define skiplistSetUnsigedInteger(_node_, _ui_) do{\
    (_node_)->value.type=ENTRY_TYPE_UINT; \
    (_node_)->value.val.ui=(_ui_); \
} while(0)

#define skiplistSetSigedInteger(_node_, _si_) do{\
    (_node_)->type=ENTRY_TYPE_INT; \
    (_node_)->value.val.si=(_si_); \
} while(0)

#define skiplistSetPoint(_sl_, _node_, _p_) do{\
    (_node_)->value.type=ENTRY_TYPE_POINT; \
    if ((_sl_)->func->valdupfunc) \
        (_node_)->value.val.point=\
            (_sl_)->func->valdupfunc((_sl_)->privdata, _p_); \
    else \
        (_node_)->value.val.point=(_p_);\
} while(0)

typedef struct skiplist{
    struct skiplistNode *header;
    void *privdata;
    unsigned long length;
    unsigned level;
    float p;
    struct entryFunc *func;
} skiplist;

typedef struct skiplistNode{
    void *key;
    entryValue value;
    struct skiplistNode *forward[];
} skiplistNode;

extern skiplist *skiplistCreate(float p, entryFunc *func, void *privdata);
extern void skiplistFree(skiplist *sl);
extern skiplistNode *skiplistAddRaw(skiplist *sl, void *key);
extern skiplistNode *skiplistFind(skiplist *sl, void *key);
extern int skiplistDelete(skiplist *sl, void *key, const int freeval);
extern skiplistNode *skiplistSearch(skiplist *sl, void *key);

#endif
