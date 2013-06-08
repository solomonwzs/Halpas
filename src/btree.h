#ifndef _BTREE_H
#define _BTREE_H

#include <stdio.h>
#include "base.h"

#define BTREE_OPT_ERR 0
#define BTREE_OPT_OK 1

typedef struct btreeKP{
    void *key;
    entryFunc value;
} btreeKP;

typedef struct btreeNode{
    struct btreeNode *parent;
    unsigned int keyNum;
} btreeNode;

typedef struct bt_setsEntry{
    entryValue value;
    struct bt_setsEntry *next, *prev;
    struct bt_setsNode *child;
} bt_setsEntry;

typedef struct bt_setsNode{
    unsigned int size;
    struct bt_setsNode *parent;
    struct bt_setsEntry *entry;
    struct bt_setsNode *lastChild;
} bt_setsNode;

typedef struct bt_sets{
    unsigned long size;
    unsigned int keyNum, height;
    void *privdata;
    entryFunc *func;
    struct bt_setsNode *root;
} bt_sets;

extern bt_sets *bt_setsCreate(unsigned int keyNum, entryFunc *func,
        void *privdata);
extern bt_setsEntry *bt_setsAdd(bt_sets *bts, entryValue ev);
extern void bt_setsFree(bt_sets *bts);

#endif
