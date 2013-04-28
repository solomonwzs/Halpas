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

static unsigned _maxLevel(int n, float p){
    return (int)ceil(log(n)/log(1/p));
}

int skiplistInit(skiplist *list, float p){
    list->level=0;
    list->length=0;
    list->p=p;
    list->header=NULL;
    return 1;
}

int skiplistAdd(skiplist *list, char *key, void *value){
    skiplistNode *x, *newNode;
    skiplistNode **newForward, *update[SKILLIST_MAXLEVEL];
    unsigned i, newNodeLevel, maxLevel;
    int ok=1;

    x=list->header;
    for (i=list->level-1; i>=0; i--){
        while (x!=NULL && strcmp(x->forward[i]->key, key)==-1){
            x=x->forward[i];
        }
        update[i]=x;
    }
    x=x->forward[0];
    if (strcmp(x->key, key)==0){
        ok=0;
    }
    else{
        maxLevel=_maxLevel(list->length+1, list->p);
        newNodeLevel=_randomLevel(maxLevel, list->p);

        newNode=malloc(sizeof(skiplistNode));
        newNode->key=key;
        newNode->value=value;

        if (newNodeLevel>list->level){
            newForward=malloc(sizeof(skiplistNode *)*newNodeLevel);
            memcpy(newForward, list->header->forward, list->level+1);
            for (i=list->level; i<newNodeLevel; i++){
                newForward[i]=newNode;
            }
        }
    }
    
    return ok;
}
