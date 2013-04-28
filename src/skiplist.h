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
    struct skiplistNode *forward[];
} skiplistNode;

//extern skiplistNode slsearchNode(skiplist list, char *key);
extern int skiplistInit(skiplist *list, float p);
extern int skiplistAdd(skiplist *list, char *key, void *value);

#endif
