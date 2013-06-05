#ifndef _BTREE_H
#define _BTREE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BT_ENTRY_TYPE_UNKNOWN 0x00
#define BT_ENTRY_TYPE_POINT 0x01
#define BT_ENTRY_TYPE_UINT 0x02
#define BT_ENTRY_TYPE_INT 0x03
#define BT_ENTRY_TYPE_FLOAT 0x04

typedef struct btreeKP{
    void *key;
    union{
        void *point;
        uint64_t ui;
        int64_t si;
        double f;
    } value;
} btreeKP;

typedef struct btreeNode{
    struct btreeNode *parent;
    unsigned int keyNum;
} btreeNode;

typedef struct bt_setsEntry{
    int type;
    union{
        void *point;
        uint64_t ui;
        int64_t si;
        double f;
    } value;
    struct bt_setsEntry *next;
    struct bt_setsNode *child;
} bt_setsEntry;

typedef struct bt_setsNode{
    struct setsNode *parent;
} bt_setsNode;

typedef struct bt_sets{
    unsigned long size;
    unsigned int keyNum;
    struct bt_setsNode *root;
} bt_sets;

extern bt_sets *bt_setsCreate(unsigned int keyNum);

#endif
