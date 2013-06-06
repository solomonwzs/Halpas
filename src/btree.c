#include "btree.h"

#define _entryCompare(_bts_, _e1_, _e2_) \
    ((_bts_)->func->keycmpfunc((_bts_)->privdata, &(_e1_), &(_e2_)))

static bt_setsNode *_bt_setsNodeCreate(){
    bt_setsNode *btsn=malloc(sizeof(bt_setsNode));
    if (btsn){
        btsn->size=0;
        btsn->entry=NULL;
        btsn->lastEntry=NULL;
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
            bts->root=NULL;
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
        btsn=btsn->lastEntry->child;
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

    return NULL;
}
