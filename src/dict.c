#include "dict.h"

#define _EXPAND_RATIO 1

#define _dictFreeKey(_d_, _entry_) do{\
    if ((_d_)->func.keyfreefunc) \
        (_d_)->func.keyfreefunc((_d_)->privdata, (_entry_)->key);\
} while(0)

#define _dictSetKey(_d_, _entry_, _key_) do{\
    if ((_d_)->func.keydupfunc) \
        (_entry_)->key=(_d_)->func.keydupfunc((_d_)->privdata, _key_); \
    else \
        (_entry_)->key=(_key_);\
} while(0)

#define _dictFreeVal(_d_, _entry_) do{\
    if ((_d_)->func.valfreefunc) \
        (_d_)->func.valfreefunc((_d_)->privdata, (_entry_)->v.val);\
} while(0)

#define _dictSetUnsigedInteger(_entry_, _ui_) \
    (_entry_)->v.ui=(_ui_)

#define _dictSetSigedInteger(_entry_, _si_) \
    (_entry_)->v.si=(_si_)

#define _dictSetVal(_d_, _entry_, _val_) do{\
    if ((_d_)->func.valdupfunc) \
        (_entry_)->v.val=(_d_)->func.valdupfunc((_d_)->privdata, _val_); \
    else \
        (_entry_)->v.val=(_val_);\
} while(0)

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

static void _dictFreeHashTable(dict *d, int index){
    unsigned long i;
    dictHashTable *dht=&(d->ht[index]);
    dictEntry *de, *next;

    for (i=0; i<dht->size && dht->used>0; ++i){
        if ((de=dht->table[i])!=NULL){
            while (de){
                next=de->next;
                _dictFreeKey(d, de);
                _dictFreeVal(d, de);
                free(de);
                --dht->used;
                de=next;
            }
        }
    }
    free(dht->table);
    dht->table=NULL;
}

static void _dictRehashStep(dict *d){
    dictHashTable *dht1=&d->ht[1];

    if (dht1->used==0){
        d->rehashing=0;
    }
    else{
        dictEntry *de, *next;
        unsigned long i, hashIndex;
        dictHashTable *dht0=&d->ht[0];

        for (i=0; i<dht1->size && dht1->table[i]==NULL; ++i);

        de=dht1->table[i];
        dht1->table[i]=NULL;
        while (de){
            next=de->next;

            hashIndex=d->func.hashfunc(d->privdata, de->key)%dht0->size;
            de->next=dht0->table[hashIndex];
            dht0->table[hashIndex]=de;

            ++dht0->used;
            --dht1->used;

            de=next;
        }

        if (dht1->used==0){
            d->rehashing=0;
        }
    }
}

dictEntry *dictAddRaw(dict *d, void *key){
    dictEntry *de=malloc(sizeof(dictEntry *));

    if (d->rehashing!=0)
        _dictRehashStep(d);
    if (de){
        unsigned long hashIndex;
        dictHashTable *dht0=&(d->ht[0]);


        hashIndex=d->func.hashfunc(d->privdata, key)%dht0->size;
        de->next=dht0->table[hashIndex];
        dht0->table[hashIndex]=de;
        _dictSetKey(d, de, key);
        ++dht0->used;

        if (d->rehashing==0 && dht0->used/(float)dht0->size>_EXPAND_RATIO){
            d->ht[1]=d->ht[0];
            _dictHashTableInit(&(d->ht[0]), d->ht[1].size*2);
            d->rehashing=1;
        }
    }
    return de;
}

dict *dictCreate(dictFunc dictfunc, void *privdata, unsigned long size){
    dict *d=malloc(sizeof(dict));

    d->func=dictfunc;
    d->privdata=privdata;
    d->rehashing=0;

    _dictHashTableInit(&d->ht[0], size);
    _dictHashTableInit(&d->ht[1], 0);

    return d;
}

void dictFree(dict *d){
    _dictFreeHashTable(d, 0);
    _dictFreeHashTable(d, 1);
    free(d);
}
