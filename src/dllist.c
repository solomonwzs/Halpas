#include "dllist.h"

dllist *dllistCreate(void *(*dupFunc)(void *), void (*freeFunc)(void *)){
    dllist *dl=malloc(sizeof(dllist));

    if (dl){
        dl->length=0;
        dl->dup=dupFunc;
        dl->free=freeFunc;
        dl->head=NULL;
        dl->tail=NULL;
    }
    return NULL;
}

int dllistLeftPush(dllist *dl, void *value){
    dllistNode *dln=malloc(sizeof(dllistNode));

    if (dln){
        dln->value=value;
        dln->prev=NULL;
        dln->next=dl->head;
        dl->head=dln;

        if (dl->length++==0){
            dl->tail=dln;
        }
        return 1;
    }
    else{
        return 0;
    }
}

dllistNode *dllistLeftPull(dllist *dl){
    dllistNode *dln=dl->head;

    if (dln){
        dl->head=dl->head->next;
        if (!(--dl->length)){
            dl->tail=NULL;
        }
    }
    return dln;
}

int dllistRightPush(dllist *dl, void *value){
    dllistNode *dln=malloc(sizeof(dllistNode));

    if (dln){
        dln->value=value;
        dln->prev=dl->tail;
        dln->next=NULL;
        dl->tail=dln;

        if (dl->length++==0){
            dl->head=dln;
        }
        return 1;
    }
    else{
        return 0;
    }
}

dllistNode *dllistRightPull(dllist *dl){
    dllistNode *dln=dl->tail;

    if (dln){
        dl->tail=dl->tail->prev;
        if (!(--dl->length)){
            dl->head=NULL;
        }
    }
    return dln;
}

dllistNode *dllistIndex(dllist *dl, long index){
    dllistNode *dln;

    if (index<0){
        index=(-index)-1;
        dln=dl->tail;
        while (dln && index--){
            dln=dln->prev;
        }
    }
    else{
        dln=dl->head;
        while (dln && index--){
            dln=dln->next;
        }
    }
    return dln;
}

void dllistFilter(dllist *dl, int (*filter)(void *)){
    dllistNode **p, *n;

    if (dl->head){
        if (dl->free){
            for (p=&dl->head; p; ){
                n=*p;
                if (filter(n->value)>0){
                    *p=n->next;
                    dl->length--;
                    dl->free(n->value);
                    free(n);
                }
                else{
                    p=&n->next;
                }
            }
        }
        else{
            for (p=&dl->head; p; ){
                n=*p;
                if (filter(n->value)>0){
                    *p=n->next;
                    dl->length--;
                    free(n);
                }
                else{
                    p=&n->next;
                }
            }
        }
    }
}

void dllistFree(dllist *dl){
}
