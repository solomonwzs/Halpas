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
    return dl;
}

int dllistLeftPush(dllist *dl, void *value){
    dllistNode *dln=malloc(sizeof(dllistNode));

    if (dln){
        dln->value=value;
        dln->prev=NULL;
        dln->next=dl->head;

        if (dl->head){
            dl->head->prev=dln;
        }
        else{
            dl->tail=dln;
        }
        dl->head=dln;
        dl->length++;

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

        if (dl->tail){
            dl->tail->next=dln;
        }
        else{
            dl->head=dln;
        }
        dl->tail=dln;
        dl->length++;

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
            for (p=&dl->head; *p; ){
                n=*p;
                if (filter(n->value)>0){
                    if (n->next){
                        n->next->prev=(*p)->prev;
                    }
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
            for (p=&dl->head; *p; ){
                n=*p;
                if (filter(n->value)>0){
                    if (n->next){
                        n->next->prev=(*p)->prev;
                    }
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
    dllistNode *dln=dl->head, *next;

    if (dl->free){
        while (dln){
            next=dln->next;
            dl->free(dln->value);
            free(dln);
            dln=next;
        }
    }
    else{
        while (dln){
            next=dln->next;
            free(dln);
            dln=next;
        }
    }
    free(dl);
}

void dllistReverse(dllist *dl){
    dllistNode *dln=dl->head, *next;

    while (dln){
        next=dln->next;
        dln->next=dln->prev;
        dln->prev=next;
        dln=next;
    }
    next=dl->head;
    dl->head=dl->tail;
    dl->tail=next;
}
