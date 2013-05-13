#ifndef _DLLIST_H
#define _DLLIST_H

#include <stdlib.h>

typedef struct dllistNode{
    void *value;
    struct dllistNode *prev, *next;
} dllistNode;

typedef struct dllist{
    unsigned long length;
    struct dllistNode *head, *tail;
    void *(*dup)(void *);
    void (*free)(void *);
} dllist;

extern dllist *dllistCreate(void *(*dupFunc)(void *), void (*freeFunc)(void *));
extern int dllistLeftPush(dllist *dl, void *value);
extern int dllistRightPush(dllist *dl, void *value);
extern dllistNode *dllistLeftPull(dllist *dl);
extern dllistNode *dllistRightPull(dllist *dl);
extern dllistNode *dllistIndex(dllist *dl, long index);
extern void dllistFilter(dllist *dl, int (*filter)(void *));
extern void dllistReverse(dllist *dl);
extern void dllistFree(dllist *dl);

#endif
