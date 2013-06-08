#include "btree.h"

#define _next(_e_) \
    (_e_)=(_e_)->next

#define _prev(_e_) \
    (_e_)=(_e_)->prev

#define _entryCompare(_bts_, _e1_, _e2_) \
    ((_bts_)->func->keycmpfunc((_bts_)->privdata, &(_e1_), &(_e2_)))

#define _getMidEntry(_i_, _n_, _mid_) \
    for ((_i_)=0, (_mid_)=(_n_)->entry; \
            (_i_)<=(_n_)->size/2 && (_mid_); \
            ++(_i_), _next(_mid_))

#define _setPrevEntry(_curr_, _e_) do{\
    (_e_)->prev=(_curr_)->prev; \
    (_e_)->next=(_curr_); \
    (_curr_)->prev=(_e_); \
} while(0)

bt_sets *bt_setsCreate(unsigned int keyNum, entryFunc *func, void *privdata){
    bt_sets *bts=malloc(sizeof(bt_sets));

    if (bts){
        bt_setsNode *btsn=malloc(sizeof(bt_setsNode));

        if (btsn){
            btsn->size=0;
            btsn->parent=NULL;
            btsn->entry=NULL;
            btsn->lastChild=NULL;

            bts->root=btsn;
            bts->size=0;
            bts->keyNum=keyNum;
            bts->privdata=privdata;
            bts->func=func;
            bts->height=1;
        }
        else{
            free(bts);
            return NULL;
        }
    }
    return bts;
}

static int _bt_setsAddEntryToNode(bt_sets *bts, bt_setsNode *btsn,
        bt_setsEntry *btse){
    bt_setsEntry *p;
    int comp, i;
    entryValue ev=btse->value;

    for (p=btsn->entry, i=0; p && i<btsn->size-1; ++i){
        comp=_entryCompare(bts, ev, p->value);
        if (comp>0){
            _next(p);
        }
        else if (comp==0){
            return BTREE_OPT_ERR;
        }
        else{
            break;
        }
    }

    if (i==0){
        btse->prev=NULL;
        btse->next=NULL;
        btsn->entry=btse;
    }
    else if (i==btsn->size-1){
        comp=_entryCompare(bts, ev, p->value);
        if (comp==0){
            return BTREE_OPT_ERR;
        }
        else if (comp<0){
            _setPrevEntry(p, btse);
        }
        else{
            btse->prev=p;
            btse->next=NULL;
            p->next=btse;
        }
    }
    else{
        _setPrevEntry(p, btse);
    }
    ++btsn->size;

    return BTREE_OPT_OK;
}

static int _bt_setsNodeSplit(bt_setsNode *btsn, bt_setsEntry **me){
    bt_setsNode *nn=malloc(sizeof(bt_setsNode));
    if (nn){
        unsigned int size=btsn->size;
        int i;

        _getMidEntry(i, btsn, *me);

        nn->parent=btsn->parent;
        nn->lastChild=(*me)->child;
        nn->entry=btsn->entry;
        nn->size=size/2;

        btsn->entry=(*me)->next;
        btsn->size=size-size/2-1;

        if ((*me)->prev) (*me)->prev->next=NULL;
        if ((*me)->next) (*me)->next->prev=NULL;

        (*me)->child=btsn;

        return BTREE_OPT_OK;
    }
    return BTREE_OPT_ERR;
}

bt_setsEntry *bt_setsAdd(bt_sets *bts, entryValue ev){
    bt_setsNode *btsn;
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
        btsn=btsn->lastChild;
    }

    btse=malloc(sizeof(bt_setsEntry));
    if (btse){
        ++bts->size;
        btse->value=ev;
        btse->child=NULL;
        me=btse;
        while (_bt_setsAddEntryToNode(bts, btsn, me)==BTREE_OPT_OK){
            if (btsn->size>bts->keyNum){
                _bt_setsNodeSplit(btsn, &me);
                btsn=btsn->parent;
                if (btsn==NULL){
                    btsn=malloc(sizeof(bt_setsNode));
                    btsn->size=1;
                    btsn->parent=NULL;
                    btsn->lastChild=NULL;
                    btsn->entry=me;

                    me->next=NULL;
                    me->prev=NULL;

                    bts->root=btsn;
                    ++bts->height;

                    return btse;
                }
                else{
                    btsn=btsn->parent;
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

void bt_setsFree(bt_sets *bts){
    _bt_setsFreeNode(bts->root);
    free(bts);
}
