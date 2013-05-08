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

//static unsigned _maxLevel(int n, float p){
//    return (int)ceil(log(n)/log(1/p));
//}

static skiplistNode *_skiplistCreateNode(int level, char *key, void *value,
        void (*freeNode)(void *)){
    skiplistNode *sln=malloc(sizeof(skiplistNode)*(1+level));
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
    sl->header=_skiplistCreateNode(SKILLIST_MAXLEVEL, NULL, NULL, NULL);
    for (i=0; i<SKILLIST_MAXLEVEL; i++){
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

skiplistNode *skiplistInsert(skiplist *sl, char *key, void *value,
        void (*freeNode)(void *)){
    skiplistNode *x, *update[SKILLIST_MAXLEVEL];
    int i, level;

    x=sl->header;
    for (i=sl->level-1; i>=0; i--){
        while (x->forward[i] && strcmp(x->forward[i]->key, key)==-1){
            x=x->forward[i];
        }
        update[i]=x;
    }

    if (x->forward[1] && strcmp(x->forward[1]->key, key)==0){
        return NULL;
    }

    level=_randomLevel(sl->length, sl->p);
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
    
    return x;
}

void *skiplistSearch(skiplist *sl, char *key){
    skiplistNode *x=sl->header;
    int i;

    for (i=sl->level-1; i>=0; i--){
        while (x->forward[i] && strcmp(x->forward[i]->key, key)==-1){
            x=x->forward[i];
        }
    }

    x=x->forward[0];
    if (x && strcmp(x->key, key)==0){
        return x->value;
    }
    else{
        return NULL;
    }
}
