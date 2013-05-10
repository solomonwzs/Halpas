#include "skiplist.h"

static uint8_t _setSeed=0;

static unsigned _randomLevel(int maxLevel, float p){
    int level=1;

    if (!_setSeed){
        srand((int)time(0));
        _setSeed=1;
    }
    while (((float)rand())/RAND_MAX<p && level<maxLevel){
        level++;
    }
    return level;
}

static skiplistNode *_skiplistCreateNode(int level, char *key, void *value){
    skiplistNode *sln=malloc(sizeof(skiplistNode)+level*sizeof(skiplistNode *));
    if (sln){
        sln->key=key;
        sln->value=value;
    }
    return sln;
}

skiplist *skiplistCreate(float p, void (*freeFunc)(void *)){
    skiplist *sl=malloc(sizeof(skiplist));
    int i;

    if (sl){
        sl->level=0;
        sl->length=0;
        sl->p=p;
        sl->free=freeFunc;
        sl->header=_skiplistCreateNode(SKIPLIST_MAXLEVEL, NULL, NULL);
        for (i=0; i<SKIPLIST_MAXLEVEL; i++){
            sl->header->forward[i]=NULL;
        }
    }
    return sl;
}

void skiplistFree(skiplist *sl){
    skiplistNode *node=sl->header->forward[0], *next;

    free(sl->header);
    if (sl->free){
        while (node){
            next=node->forward[0];
            sl->free(node->value);
            free(node);
            node=next;
        }
    }
    else{
        while (node){
            next=node->forward[0];
            free(node);
            node=next;
        }
    }
    free(sl);
}

static int _skiplistPut(skiplist *sl, char *key, void *value, int opt){
    skiplistNode *x, *update[SKIPLIST_MAXLEVEL];
    int i, level, maxLevel;

    x=sl->header;
    for (i=sl->level-1; i>=0; i--){
        while (x->forward[i] && strcmp(x->forward[i]->key, key)<0){
            x=x->forward[i];
        }
        update[i]=x;
    }

    x=x->forward[0];
    if (x && strcmp(x->key, key)==0){
        if (opt==SKIPLIST_OPT_INSERT){
            return 0;
        }
        else{
            if (sl->free){
                sl->free(x->value);
                x->value=value;
            }
            return 1;
        }
    }

    maxLevel=(int)ceil(log(sl->length+1)/log(1/sl->p));
    level=_randomLevel(maxLevel, sl->p);
    x=_skiplistCreateNode(level, key, value);

    if (x){
        if (level>sl->level){
            for (i=sl->level; i<level; i++){
                update[i]=sl->header;
            }
            sl->level=level;
        }
        for (i=0; i<level; i++){
            x->forward[i]=update[i]->forward[i];
            update[i]->forward[i]=x;
        }
        sl->length++;
        
        return 1;
    }
    else{
        return 0;
    }
}

int skiplistInsert(skiplist *sl, char *key, void *value){
    return _skiplistPut(sl, key, value, SKIPLIST_OPT_INSERT);
}

int skiplistWrite(skiplist *sl, char *key, void *value){
    return _skiplistPut(sl, key, value, SKIPLIST_OPT_WRITE);
}

skiplistNode *skiplistSearch(skiplist *sl, char *key){
    skiplistNode *x=sl->header;
    int i;

    for (i=sl->level-1; i>=0; i--){
        while (x->forward[i] && strcmp(x->forward[i]->key, key)<0){
            x=x->forward[i];
        }
    }

    x=x->forward[0];
    if (x && strcmp(x->key, key)==0){
        return x;
    }
    else{
        return NULL;
    }
}

int skiplistUpdate(skiplist *sl, char *key, void *value){
    skiplistNode *x=skiplistSearch(sl, key);
    if (x){
        if (sl->free){
            sl->free(x->value);
            x->value=value;
        }
        return 1;
    }
    else{
        return 0;
    }
}

void skiplistDelete(skiplist *sl, char *key){
    skiplistNode *x, *update[SKIPLIST_MAXLEVEL];
    int i;

    x=sl->header;
    for (i=sl->level-1; i>=0; i--){
        while (x->forward[i] && strcmp(x->forward[i]->key, key)<0){
            x=x->forward[i];
        }
        update[i]=x;
    }

    x=x->forward[0];
    if (x && strcmp(x->key, key)==0){
        for (i=0; i<sl->level; i++){
            if (update[i]->forward[i]!=x){
                break;
            }
            update[i]->forward[i]=x->forward[i];
        }
        if (sl->free){
            sl->free(x->value);
            free(x);
        }

        while (sl->level>0 && sl->header->forward[sl->level-1]==NULL){
            sl->level--;
        }
        sl->length--;
    }
}
