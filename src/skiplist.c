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

static void _swap_value(skiplistNode *sln, void *value,
        void (*freeNode)(void *)){
    if (sln->freeNode){
        sln->freeNode(sln->value);
    }
    sln->freeNode=freeNode;
    sln->value=value;
}

static skiplistNode *_skiplistCreateNode(int level, char *key, void *value,
        void (*freeNode)(void *)){
    skiplistNode *sln=malloc(sizeof(skiplistNode)+level*sizeof(skiplistNode *));
    sln->key=key;
    sln->value=value;
    sln->freeNode=freeNode;

    return sln;
}

static void _skiplistFreeNode(skiplistNode *sln){
    if (sln->freeNode){
        sln->freeNode(sln->value);
    }
    free(sln);
}

skiplist *skiplistInit(float p){
    skiplist *sl=malloc(sizeof(skiplist));
    int i;

    sl->level=0;
    sl->length=0;
    sl->p=p;
    sl->header=_skiplistCreateNode(SKIPLIST_MAXLEVEL, NULL, NULL, NULL);
    for (i=0; i<SKIPLIST_MAXLEVEL; i++){
        sl->header->forward[i]=NULL;
    }
    return sl;
}

void skiplistFree(skiplist *sl){
    skiplistNode *node=sl->header->forward[0], *next;

    _skiplistFreeNode(sl->header);
    while (node){
        next=node->forward[0];
        _skiplistFreeNode(node);
        node=next;
    }
    free(sl);
}

static int _skiplistPut(skiplist *sl, char *key, void *value,
        void (*freeNode)(void *), int opt){
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
            _swap_value(x, value, freeNode);
            return 1;
        }
    }

    maxLevel=(int)ceil(log(sl->length+1)/log(1/sl->p));
    level=_randomLevel(maxLevel, sl->p);
    if (level>sl->level){
        for (i=sl->level; i<level; i++){
            update[i]=sl->header;
        }
        sl->level=level;
    }
    x=_skiplistCreateNode(level, key, value, freeNode);
    for (i=0; i<level; i++){
        x->forward[i]=update[i]->forward[i];
        update[i]->forward[i]=x;
    }
    sl->length++;
    
    return 1;
}

int skiplistInsert(skiplist *sl, char *key, void *value,
        void (*freeNode)(void *)){
    return _skiplistPut(sl, key, value, freeNode, SKIPLIST_OPT_INSERT);
}

int skiplistWrite(skiplist *sl, char *key, void *value,
        void (*freeNode)(void *)){
    return _skiplistPut(sl, key, value, freeNode, SKIPLIST_OPT_WRITE);
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

int skiplistUpdate(skiplist *sl, char *key, void *value,
        void (*freeNode)(void *)){
    skiplistNode *x=skiplistSearch(sl, key);
    if (x){
        _swap_value(x, value, freeNode);
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
        _skiplistFreeNode(x);

        while (sl->level>0 && sl->header->forward[sl->level-1]==NULL){
            sl->level--;
        }
        sl->length--;
    }
}
