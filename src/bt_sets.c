#include "btree.h"

#define _next(_e_) \
    (_e_)=(_e_)->next

#define _prev(_e_) \
    (_e_)=(_e_)->prev

#define _minEntryNum(_s_) \
    (ceil((_s_)->keyNum/2)-1)

#define _setChild(_e_, _n_) do{\
    typeof((_e_)->child) __n_=(_n_); \
    (_e_)->child=(__n_); \
    if (__n_) {\
        (__n_)->pEntry=(_e_); \
    }\
} while(0)

#define _entryCompare(_bts_, _e1_, _e2_) \
    ((_e2_).type==ENTRY_TYPE_MAX? \
     -1: \
     (_bts_)->func->keycmpfunc((_bts_)->privdata, &(_e1_), &(_e2_)))

#define _getMidEntry(_i_, _n_, _mid_) \
    for ((_i_)=0, (_mid_)=(_n_)->entry; \
            (_i_)<(_n_)->size/2 && (_mid_); \
            ++(_i_), _next(_mid_))

#define _setPrevEntry(_curr_, _e_) do{\
    (_e_)->prev=(_curr_)->prev; \
    (_e_)->next=(_curr_); \
    if ((_curr_)->prev){ \
        (_curr_)->prev->next=(_e_); \
    } \
    (_curr_)->prev=(_e_); \
} while(0)

#define _setNextEntry(_curr_, _e_) do{\
    (_e_)->next=(_curr_)->next; \
    (_e_)->prev=(_curr_); \
    if ((_curr_)->next){ \
        (_curr_)->next->prev=(_e_); \
    } \
    (_curr_)->next=(_e_); \
} while(0)

#define _creatEndEntry(_e_) do{\
    (_e_)=malloc(sizeof(bt_setsEntry)); \
    (_e_)->next=NULL; \
    (_e_)->value.type=ENTRY_TYPE_MAX; \
} while(0)

#define _freeEntry(_bts_, _e_) do{\
    if ((_bts_)->func->valfreefunc && (_e_)->value.type==ENTRY_TYPE_POINT) \
        (_bts_)->func->valfreefunc((_bts_)->privdata, (_e_)->value.val.point);\
} while(0)

void bt_setsTraversalPrint(bt_setsNode *btsn){
    bt_setsEntry *btse=btsn->entry;

    printf("{ ");
    while (btse){
        if (btse->child){
            bt_setsTraversalPrint(btse->child);
        }
        if (btse->value.type!=ENTRY_TYPE_MAX){
            printEntryValue(btse->value);
        }
        _next(btse);
    }
    printf(" }");
}

static void _bt_setsFreeNode(bt_setsNode *btsn){
    bt_setsEntry *btse=btsn->entry;
    while (btse){
        if (btse->child){
            _bt_setsFreeNode(btse->child);
        }
        _next(btse);
    }
    free(btsn);
}

static int _bt_setsFind(const bt_sets *bts, const entryValue ev,
        bt_setsNode **btsn, bt_setsEntry **btse){
    int comp;

    *btsn=bts->root;
    *btse=(*btsn)->entry;
    while (*btsn){
        while (*btse){
            comp=_entryCompare(bts, ev, (*btse)->value);
            if (comp==0){
                return BTREE_OPT_OK;
            }
            else if (comp<0){
                *btsn=(*btse)->child;
                *btse=(*btsn)->entry;
            }
            else{
                _next(*btse);
            }
        }
    }
    return BTREE_OPT_ERR;
}

void bt_setsDel(bt_sets *bts, entryValue ev, const int freeval){
    bt_setsNode *btsn;
    bt_setsEntry *btse;

    if (_bt_setsFind(bts, ev, &btsn, &btse)==BTREE_OPT_OK){
        printEntryValue(btsn->entry->value);
    }
}

bt_sets *bt_setsCreate(unsigned int keyNum, entryFunc *func, void *privdata){
    bt_sets *bts=malloc(sizeof(bt_sets));
    bt_setsNode *btsn=malloc(sizeof(bt_setsNode));
    bt_setsEntry *btse;

    _creatEndEntry(btse);
    btse->prev=NULL;
    _setChild(btse, NULL);

    btsn->size=0;
    btsn->parent=NULL;
    btsn->entry=btse;

    bts->root=btsn;
    bts->size=0;
    bts->keyNum=keyNum;
    bts->privdata=privdata;
    bts->func=func;
    bts->height=1;
    return bts;
}

static int _bt_setsAddEntryToNode(bt_sets *bts, bt_setsNode *btsn,
        bt_setsEntry *btse){
    bt_setsEntry *p;
    int comp, i;
    entryValue ev=btse->value;

    for (p=btsn->entry; p; _next(p)){
        comp=_entryCompare(bts, ev, p->value);
        if (comp==0){
            return BTREE_OPT_ERR;
        }
        else if (comp<0){
            break;
        }
    }
    _setPrevEntry(p, btse);

    while (btsn->entry->prev){
        _prev(btsn->entry);
    }
    ++btsn->size;

    return BTREE_OPT_OK;
}

static int _bt_setsNodeSplit(bt_setsNode *btsn, bt_setsEntry **me, bt_setsNode **nn){
    (*nn)=malloc(sizeof(bt_setsNode));
    if (nn){
        unsigned int size=btsn->size;
        bt_setsEntry *btse;
        int i;

        _getMidEntry(i, btsn, *me);

        (*nn)->entry=btsn->entry;
        (*nn)->size=size/2;
        _creatEndEntry(btse);
        btse->prev=(*me)->prev;
        _setChild(btse, (*me)->child);
        (*me)->prev->next=btse;

        btsn->entry=(*me)->next;
        btsn->size=size-size/2-1;

        if ((*me)->next) (*me)->next->prev=NULL;

        _setChild(*me, *nn);

        return BTREE_OPT_OK;
    }
    return BTREE_OPT_ERR;
}

bt_setsEntry *bt_setsAdd(bt_sets *bts, entryValue ev){
    bt_setsNode *btsn, *parent, *nn;
    bt_setsEntry *btse, *me;
    unsigned int i;
    int comp;

    btsn=bts->root;
    for (i=1; i<bts->height; ++i){
        for (btse=btsn->entry; btse; ){
            comp=_entryCompare(bts, ev, btse->value);
            if (comp>0){
                btse=btse->next;
            }
            else{
                if (comp==0){
                    return NULL;
                }
                btsn=btse->child;
                break;
            }
        }
    }

    btse=malloc(sizeof(bt_setsEntry));
    if (btse){
        ++bts->size;
        btse->value=ev;
        _setChild(btse, NULL);
        me=btse;
        while (_bt_setsAddEntryToNode(bts, btsn, me)==BTREE_OPT_OK){
            if (btsn->size>bts->keyNum){
                _bt_setsNodeSplit(btsn, &me, &nn);
                parent=btsn->parent;
                if (parent==NULL){
                    bt_setsEntry *ne;

                    parent=malloc(sizeof(bt_setsNode));
                    parent->size=1;
                    parent->parent=NULL;
                    parent->entry=me;

                    _creatEndEntry(ne);
                    ne->prev=me;
                    _setChild(ne, btsn);

                    me->next=ne;
                    _setChild(me, nn);
                    me->prev=NULL;

                    bts->root=parent;
                    ++bts->height;

                    nn->parent=parent;
                    btsn->parent=parent;

                    return btse;
                }
                else{
                    nn->parent=parent;
                    btsn=parent;
                }
            }
            else{
                return btse;
            }
        }
        free(btse);
    }
    return NULL;
}

void bt_setsFree(bt_sets *bts){
    _bt_setsFreeNode(bts->root);
    free(bts);
}