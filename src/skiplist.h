#ifndef _SKIP_LIST_H
#define _SKIP_LIST_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SKIPLIST_OPT_ERR 0
#define SKIPLIST_OPT_OK 1

#define SKIPLIST_NODE_TYPE_UNKNOWN 0x00
#define SKIPLIST_NODE_TYPE_POINT 0x01
#define SKIPLIST_NODE_TYPE_UINT 0x02
#define SKIPLIST_NODE_TYPE_INT 0x03
#define SKIPLIST_NODE_TYPE_FLOAT 0x04

#define skiplistFreeKey(_sl_, _node_) do{\
    if ((_sl_)->func->keyfreefunc) \
        (_sl_)->func->keyfreefunc((_sl_)->privdata, (_node_)->key);\
} while(0)

#define skiplistFreeVal(_sl_, _node_) do{\
    if ((_sl_)->func->valfreefunc && (_node_)->type==SKIPLIST_NODE_TYPE_POINT) \
        (_sl_)->func->valfreefunc((_sl_)->privdata, (_node_)->value.point);\
} while(0)

#define skiplistKeyCompare(_sl_, _key1_, _key2_) \
    (((_sl_)->func->keycmpfunc)? \
        (_sl_)->func->keycmpfunc((_sl_)->privdata, _key1_, _key2_): \
        (_key1_)!=(_key2_))

#define skiplistSetFloat(_node_, _f_) do{\
    (_node_)->type=SKIPLIST_NODE_TYPE_FLOAT; \
    (_node_)->value.f=(_f_); \
} while(0)

#define skiplistSetUnsigedInteger(_node_, _ui_) do{\
    (_node_)->type=SKIPLIST_NODE_TYPE_UINT; \
    (_node_)->value.ui=(_ui_); \
} while(0)

#define skiplistSetSigedInteger(_node_, _si_) do{\
    (_node_)->type=SKIPLIST_NODE_TYPE_INT; \
    (_node_)->value.si=(_si_); \
} while(0)

#define skiplistSetPoint(_sl_, _node_, _p_) do{\
    (_node_)->type=SKIPLIST_NODE_TYPE_POINT; \
    if ((_sl_)->func->valdupfunc) \
        (_node_)->value.point=(_sl_)->func->valdupfunc((_sl_)->privdata, _p_); \
    else \
        (_node_)->value.point=(_p_);\
} while(0)

typedef struct skiplist{
    struct skiplistNode *header;
    void *privdata;
    unsigned long length;
    unsigned level;
    float p;
    struct skiplistFunc *func;
} skiplist;

typedef struct skiplistNode{
    uint8_t type;
    void *key;
    union{
        void *point;
        uint64_t ui;
        int64_t si;
        double f;
    } value;
    struct skiplistNode *forward[];
} skiplistNode;

typedef struct skiplistFunc{
    int (*keycmpfunc)(void *privdata, const void *key1, const void *key2);
    void (*keyfreefunc)(void *privdata, void *key);
    void (*valfreefunc)(void *privdata, void *val);
    void *(*keydupfunc)(void *privdata, void *key);
    void *(*valdupfunc)(void *privdata, void *val);
}skiplistFunc;

extern skiplist *skiplistCreate(float p, skiplistFunc *func, void *privdata);
extern void skiplistFree(skiplist *sl);
extern skiplistNode *skiplistAddRaw(skiplist *sl, void *key);
extern skiplistNode *skiplistFind(skiplist *sl, void *key);
extern int skiplistDelete(skiplist *sl, void *key, int freeval);
extern skiplistNode *skiplistSearch(skiplist *sl, void *key);

#endif
