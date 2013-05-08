#ifndef _SKIP_LIST_H
#define _SKIP_LIST_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SKILLIST_MAXLEVEL 32

typedef struct skiplist{
    struct skiplistNode *header;
    unsigned long length;
    unsigned level;
    float p;
} skiplist;

typedef struct skiplistNode{
    char *key;
    void *value;
    void (*freeNode)(void *);
    struct skiplistNode *forward[];
} skiplistNode;

//extern skiplistNode slsearchNode(skiplist list, char *key);
extern skiplist *skiplistInit(float p);
extern skiplistNode *skiplistInsert(skiplist *sl, char *key, void *value,
        void (*freeNode)(void *));
extern void skiplistFree(skiplist *sl);
extern void *skiplistSearch(skiplist *sl, char *key);

#endif
