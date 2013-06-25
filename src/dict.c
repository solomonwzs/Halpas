#include "dict.h"

#define _EXPAND_RATIO 1

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
#ifdef _DICT_DEBUG
                printf("dict free\t%s\n", (char *)de->key);
#endif
                next=de->next;
                dictFreeKey(d, de);
                dictFreeVal(d, de);
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

            hashIndex=d->func->hashfunc(d->privdata, de->key)%dht0->size;
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

    h=d->func->hashfunc(d->privdata, key);
    for (i=0; i<2; ++i){
        index=h%d->ht[i].size;
        de=d->ht[i].table[index];

        while (de){
            if (dictKeyCompare(d, key, de->key)==0)
                return -1;
            de=de->next;
        }
        if (!dictRehashing(d)) break;
    }

    return h%d->ht[0].size;
}

dictEntry *dictAddRaw(dict *d, void *key){
    dictEntry *de;
    unsigned long hashIndex=_dictKeyHashIndex(d, key);

    if (dictRehashing(d))
        _dictRehashStep(d);

    if (hashIndex==-1)
        return NULL;

    de=malloc(sizeof(dictEntry));
    if (de){
        dictHashTable *dht0=&(d->ht[0]);

        de->next=dht0->table[hashIndex];
        de->value.type=ENTRY_TYPE_UNKNOWN;
        dht0->table[hashIndex]=de;
        dictSetKey(d, de, key);
        ++dht0->used;

        if (!dictRehashing(d) && dht0->used/(float)dht0->size>_EXPAND_RATIO){
            _dictFreeHashTable(d, 1);
            d->ht[1]=d->ht[0];
            _dictHashTableInit(&(d->ht[0]), d->ht[1].size*2);
            d->rehashing=1;
        }
    }
    return de;
}

dict *dictCreate(entryFunc *dictfunc, void *privdata, unsigned long size){
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

    if (dictRehashing(d))
        _dictRehashStep(d);
    h=d->func->hashfunc(d->privdata, key);
    for (i=0; i<2; ++i){
        de=d->ht[i].table[h%d->ht[i].size];
        while (de){
            if (dictKeyCompare(d, key, de->key)==0)
                return de;
            de=de->next;
        }
        if (!dictRehashing(d))
            return NULL;
    }

    return NULL;
}

void dictFree(dict *d){
    _dictFreeHashTable(d, 0);
    _dictFreeHashTable(d, 1);
    free(d);
}

int dictDelete(dict *d, const void *key, const int freeval){
    unsigned long h;
    uint8_t i;
    dictEntry *de, **p;

    if (dictRehashing(d))
        _dictRehashStep(d);
    h=d->func->hashfunc(d->privdata, key);
    for (i=0; i<2; ++i){
        for (p=&d->ht[i].table[h%d->ht[i].size]; *p;){
            de=*p;
            if (dictKeyCompare(d, key, de->key)==0){
                if (freeval){
                    dictFreeKey(d, de);
                    dictFreeVal(d, de);
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
        if (!dictRehashing(d))
            return DICT_OPT_ERR;
    }
    return DICT_OPT_ERR;
}
