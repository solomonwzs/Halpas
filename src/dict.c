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

#define _dictKeyCompare(_d_, _key1_, _key2_) \
    (((_d_)->func.keycmpfunc)? \
        (_d_)->func.keycmpfunc((_d_)->privdata, _key1_, _key2_): \
        (_key1_)!=(_key2_))

#define _dictRehashing(_d_) \
    ((_d_)->rehashing!=0)

static int _dictHashTableInit(dictHashTable *ht, unsigned long size){
    ht->size=size;
    ht->used=0;

    if (size==0){
        ht->table=NULL;
    }
    else{
        ht->table=malloc(sizeof(dictEntry *)*size);

        if (ht){
            int i;
            for (i=0; i<size; ++i){
                ht->table[i]=NULL;
            }
        }
        else{
            return DICT_OPT_ERR;
        }
    }
    return DICT_OPT_OK;
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

static unsigned long _dictKeyHashIndex(dict *d, const void *key){
    unsigned long h, index;
    uint8_t i;
    dictEntry *de;

    h=d->func.hashfunc(d->privdata, key);
    for (i=0; i<2; ++i){
        index=h%d->ht[i].size;
        de=d->ht[i].table[index];

        while (de){
            if (_dictKeyCompare(d, key, de->key)==0)
                return -1;
            de=de->next;
        }
        if (!_dictRehashing(d)) break;
    }

    return h%d->ht[0].size;
}

dictEntry *dictAddRaw(dict *d, void *key){
    dictEntry *de=malloc(sizeof(dictEntry *));
    unsigned long hashIndex=_dictKeyHashIndex(d, key);

    if (_dictRehashing(d))
        _dictRehashStep(d);

    if (hashIndex==-1)
        return NULL;

    de=malloc(sizeof(dictEntry *));
    if (de){
        dictHashTable *dht0=&(d->ht[0]);

        de->next=dht0->table[hashIndex];
        dht0->table[hashIndex]=de;
        _dictSetKey(d, de, key);
        ++dht0->used;

        if (!_dictRehashing(d) && dht0->used/(float)dht0->size>_EXPAND_RATIO){
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

dictEntry *dictFind(dict *d, const void *key){
    unsigned long h;
    uint8_t i;
    dictEntry *de;

    if (_dictRehashing(d))
        _dictRehashStep(d);
    h=d->func.hashfunc(d->privdata, key);
    for (i=0; i<2; ++i){
        de=d->ht[i].table[h%d->ht[i].size];
        while (de){
            if (_dictKeyCompare(d, key, de->key)==0)
                return de;
            de=de->next;
        }
        if (!_dictRehashing(d))
            return NULL;
    }

    return NULL;
}

void dictFree(dict *d){
    _dictFreeHashTable(d, 0);
    _dictFreeHashTable(d, 1);
    free(d);
}

int dictDelete(dict *d, const void *key, int freeval){
    unsigned long h;
    uint8_t i;
    dictEntry *de, **p;

    if (_dictRehashing(d))
        _dictRehashStep(d);
    h=d->func.hashfunc(d->privdata, key);
    for (i=0; i<2; ++i){
        for (p=&d->ht[i].table[h%d->ht[i].size]; *p;){
            de=*p;
            if (_dictKeyCompare(d, key, de->key)==0){
                if (freeval){
                    _dictFreeKey(d, de);
                    _dictFreeVal(d, de);
                }
                *p=de->next;
                --d->ht[i].used;
                free(de);
                return DICT_OPT_OK;
            }
            else{
                p=&de->next;
            }
        }
        if (!_dictRehashing(d))
            return DICT_OPT_ERR;
    }
    return DICT_OPT_ERR;
}
