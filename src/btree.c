#include "btree.h"

#define _next(_e_) \
    (_e_)=(_e_)->next

#define _prev(_e_) \
    (_e_)=(_e_)->prev

#define _entryCompare(_bts_, _e1_, _e2_) \
    ((_bts_)->func->keycmpfunc((_bts_)->privdata, &(_e1_), &(_e2_)))

#define _setMidEntry(_i_, _n_) \
    for ((_i_)=0, (_n_)->midEntry=(_n_)->entry; \
            (_i_)<=(_n_)->size/2 && (_n_)->midEntry; \
            ++(_i_), _next((_n_)->midEntry))

static bt_setsNode *_bt_setsNodeCreate(){
    bt_setsNode *btsn=malloc(sizeof(bt_setsNode));

    if (btsn){
        btsn->size=0;
        btsn->midEntry=NULL;
        btsn->entry=NULL;
        btsn->lastChild=NULL;
    }
    return btsn;
}

//static bt_setsEntry *_bt_setsEntryCreate(entryValue ev){
//    bt_setsEntry *btse=malloc(sizeof(bt_setsEntry));
//    if (btse){
//        btse
//    }
//}

bt_sets *bt_setsCreate(unsigned int keyNum, entryFunc *func, void *privdata){
    bt_sets *bts=malloc(sizeof(bt_sets));

    if (bts){
        bt_setsNode *btsn=_bt_setsNodeCreate();

        if (btsn){
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

static bt_setsEntry *_bt_setsAddValueToNode(bt_sets *bts, bt_setsNode *btsn,
        entryValue ev, bt_setsNode *child){
    bt_setsEntry *btse, *ne;
    int comp, i;

    for (btse=btsn->entry, i=0; btse; ++i){
        comp=_entryCompare(bts, ev, btse->value);
        if (comp>0){
            _next(btse);
        }
        else if (comp==0){
            return NULL;
        }
        else{
            break;
        }
    }

    ne=malloc(sizeof(bt_setsEntry));
    if (ne){
        ne->value=ev;
        ne->child=child;
        ne->prev=btse?btse->prev:NULL;
        ne->next=btse;
        btse->prev=ne;

        if (i==0){
            btsn->entry=ne;
        }

        if (btsn->size==0){
            btsn->midEntry=ne;
        }
        else{
            int left=btsn->size/2, right=btsn->size-left-1;
            if (i>left && left==right){
                _next(btsn->midEntry);
            }
            else if (left>right){
                _prev(btsn->midEntry);
            }
        }
        ++btsn->size;
    }
    return ne;
}

static void *_bt_setsNodeSplit(bt_setsNode *btsn, bt_setsEntry *me,
        bt_setsNode *nn){
    nn=malloc(sizeof(bt_setsNode));

    if (nn){
        unsigned int size=btsn->size;
        int i;

        me=btsn->midEntry;

        nn->parent=btsn->parent;
        nn->lastChild=btsn->lastChild;
        nn->entry=me->next;
        nn->size=size-size/2-1;

        btsn->lastChild=me->child;
        btsn->size=size/2;

        if (me->prev) me->prev->next=NULL;
        if (me->next) me->next->prev=NULL;

        _setMidEntry(i, nn);
        _setMidEntry(i, btsn);

        me->child=btsn;
    }
}

bt_setsEntry *bt_setsAdd(bt_sets *bts, entryValue ev){
    bt_setsNode *btsn;
    bt_setsEntry *btse, **p;
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

    if (btsn->size<bts->keyNum){
        for (p=&btsn->entry; *p; ){
            btse=*p;
            comp=_entryCompare(bts, ev, btse->value);
            if (comp>0){
                p=&btse->next;
            }
            else{
                if (comp==0){
                    return NULL;
                }
                break;
            }
        }
        btse=malloc(sizeof(bt_setsEntry));
        btse->value=ev;
        btse->child=NULL;
        btse->next=*p?(*p)->next:NULL;
        *p=btse;
    }
    else{
    }

    return NULL;
}
