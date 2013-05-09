#ifndef _DLLIST_H
#define _DLLIST_H

typedef struct dllistNode{
    void *value;
    struct dllistNode *prev, *next;
} dllistNode;

typedef struct dllist{
    int length;
    struct dllistNode *header, *tail;
} dllist;

#endif
