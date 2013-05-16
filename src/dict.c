#include "dict.h"

#define _dictFreeKey(d, entry) \
    if ((d)->type->keyfreefunc) \
        (d)->type->keyfreefunc((d)->privdata, (entry)->key)

#define _dictFreeVal(d, entry) \
    if ((d)->type->keyvalfunc) \
        (d)->type->keyvalfunc((d)->privdata, (entry)->v.val)

static void _dictHashTableInit(dictHashTable *ht, unsigned long size){
    ht->size=size;
    ht->used=0;

    if (size==0){
        ht->table=NULL;
    }
    else{
        int i;
        ht->table=malloc(sizeof(dictEntry *)*size);
        for (i=0; i<size; ++i){
            ht->table[i]=NULL;
        }
    }
}

dict *dictCreate(dictFunc *dictfunc, void *privdata){
    dict *d=malloc(sizeof(dict));

    d->func=dictfunc;
    d->privdata=privdata;

    _dictHashTableInit(&d->ht[0], 0);
    _dictHashTableInit(&d->ht[1], 0);

    return d;
}

void dictClearHashTable(dict *d, int index){
    unsigned long i;
}
