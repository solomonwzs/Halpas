#ifndef _SKIP_LIST_H
#define _SKIP_LIST_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct skiplist{
    struct skiplistNode *header;
    unsigned long length;
    unsigned level;
    float p;
    void (*free)(void *);
} skiplist;

typedef struct skiplistNode{
    char *key;
    void *value;
    struct skiplistNode *forward[];
} skiplistNode;

//extern skiplistNode slsearchNode(skiplist list, char *key);
extern skiplist *skiplistCreate(float p, void (*freeFunc)(void *));
extern int skiplistInsert(skiplist *sl, char *key, void *value);
extern int skiplistUpdate(skiplist *sl, char *key, void *value);
extern int skiplistWrite(skiplist *sl, char *key, void *value);
extern void skiplistFree(skiplist *sl);
extern skiplistNode *skiplistSearch(skiplist *sl, char *key);
extern void skiplistDelete(skiplist *sl, char *key);

#endif
