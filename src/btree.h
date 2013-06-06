#ifndef _BTREE_H
#define _BTREE_H

#include <stdio.h>
#include "base.h"

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
    struct bt_setsEntry *next;
    struct bt_setsNode *child;
} bt_setsEntry;

typedef struct bt_setsNode{
    unsigned int size;
    struct setsNode *parent;
    struct bt_setsEntry *lastEntry;
    struct bt_setsEntry *entry;
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

#endif
