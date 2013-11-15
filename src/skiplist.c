#include "skiplist.h"

#define SKIPLIST_MAXLEVEL 32

#define SKIPLIST_OPT_INSERT 0x00
#define SKIPLIST_OPT_WRITE 0x01

static uint8_t _setSeed=0;

static unsigned _randomLevel(int maxLevel, float p){
    int level=1;

    if (!_setSeed){
        srand((int)time(0));
        _setSeed=1;
    }
    while (((float)rand())/RAND_MAX<p && level<maxLevel){
        ++level;
    }
    return level;
}

static skiplistNode *_skiplistCreateNode(int level, void *key){
    skiplistNode *sln=malloc(sizeof(skiplistNode)+level*sizeof(skiplistNode *));
    if (sln){
        sln->key=key;
        sln->value.type=ENTRY_TYPE_UNKNOWN;
    }
    return sln;
}

skiplist *skiplistCreate(float p, entryFunc *func, void *privdata){
    skiplist *sl=malloc(sizeof(skiplist));
    int i;

    if (sl){
        sl->level=0;
        sl->length=0;
        sl->p=p;
        sl->func=func;
        sl->privdata=privdata;
        sl->header=_skiplistCreateNode(SKIPLIST_MAXLEVEL, NULL);
        for (i=0; i<SKIPLIST_MAXLEVEL; ++i){
            sl->header->forward[i]=NULL;
        }
    }
    return sl;
}

void skiplistFree(skiplist *sl){
    skiplistNode *node=sl->header->forward[0], *next;

    free(sl->header);
    while (node){
        next=node->forward[0];
        skiplistFreeKey(sl, node);
        skiplistFreeVal(sl, node);
        free(node);
        node=next;
    }
    free(sl);
}

skiplistNode *skiplistAddRaw(skiplist *sl, void *key){
    skiplistNode *x, *update[SKIPLIST_MAXLEVEL];
    int i, level, maxLevel;

    x=sl->header;
    for (i=sl->level-1; i>=0; i--){
        while (x->forward[i] &&
                skiplistKeyCompare(sl, x->forward[i]->key, key)<0){
            x=x->forward[i];
        }
        update[i]=x;
    }

    x=x->forward[0];
    if (x && skiplistKeyCompare(sl, x->key, key)==0){
        return x;
    }

    maxLevel=(int)ceil(log(sl->length+1)/log(1/sl->p));
    level=_randomLevel(maxLevel, sl->p);
    x=_skiplistCreateNode(level, key);

    if (x){
        if (level>sl->level){
            for (i=sl->level; i<level; ++i){
                update[i]=sl->header;
            }
            sl->level=level;
        }
        for (i=0; i<level; ++i){
            x->forward[i]=update[i]->forward[i];
            update[i]->forward[i]=x;
        }
        ++sl->length;
        
        return x;
    } else{
        return NULL;
    }
}

skiplistNode *skiplistSearch(skiplist *sl, void *key){
    skiplistNode *x=sl->header;
    int i;

    for (i=sl->level-1; i>=0; i--){
        while (x->forward[i] &&
                skiplistKeyCompare(sl, x->forward[i]->key, key)<0){
            x=x->forward[i];
        }
    }

    x=x->forward[0];
    if (x && skiplistKeyCompare(sl, x->key, key)==0){
        return x;
    } else{
        return NULL;
    }
}

int skiplistDelete(skiplist *sl, void *key, const int freeval){
    skiplistNode *x, *update[SKIPLIST_MAXLEVEL];
    int i;

    x=sl->header;
    for (i=sl->level-1; i>=0; i--){
        while (x->forward[i] &&
                skiplistKeyCompare(sl, x->forward[i]->key, key)<0){
            x=x->forward[i];
        }
        update[i]=x;
    }

    x=x->forward[0];
    if (x && skiplistKeyCompare(sl, x->key, key)==0){
        for (i=0; i<sl->level; ++i){
            if (update[i]->forward[i]!=x){
                break;
            }
            update[i]->forward[i]=x->forward[i];
        }

        if (freeval==1){
            skiplistFreeKey(sl, x);
            skiplistFreeVal(sl, x);
        }
        free(x);

        while (sl->level>0 && sl->header->forward[sl->level-1]==NULL){
            sl->level--;
        }
        sl->length--;

        return SKIPLIST_OPT_OK;
    }
    return SKIPLIST_OPT_ERR;
}
