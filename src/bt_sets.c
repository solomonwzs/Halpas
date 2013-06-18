#include "btree.h"

#define _next(_e_) \
    (_e_)=(_e_)->next

#define _prev(_e_) \
    (_e_)=(_e_)->prev

#define _lastEntry(_n_) \
    ((_n_)->last->prev)

#define _isRoot(_n_) \
    ((_n_)->parent==NULL)

#define _isLeaf(_n_) \
    ((_n_)->head->child==NULL)

#define _leftBrother(_n_) \
    (((_n_)->pEntry && (_n_)->pEntry->prev)?(_n_)->pEntry->prev->child:NULL)

#define _rightBrother(_n_) \
    (((_n_)->pEntry && (_n_)->pEntry->next)?(_n_)->pEntry->next->child:NULL)

#define _leftChild(_e_) \
    ((_e_)->child?(_e_)->child:NULL)

#define _rightChild(_e_) \
    (((_e_)->next && (_e_)->next->child)?(_e_)->next->child:NULL)

#define _minEntryNum(_s_) \
    ((_s_)->d-1)

#define _maxEntryNum(_s_) \
    ((_s_)->d*2-2)

#define _setChild(_e_, _n_) do{\
    typeof((_e_)->child) __n_=(_n_); \
    (_e_)->child=(__n_); \
    if (__n_) {\
        (__n_)->pEntry=(_e_); \
    }\
} while(0)

#define _removeEntry(_e_) do{\
    if ((_e_)->prev){ \
        (_e_)->prev->next=(_e_)->next; \
    } \
    (_e_)->next->prev=(_e_)->prev; \
} while(0)

#define _entryCompare(_bts_, _e1_, _e2_) \
    ((_e2_).type==ENTRY_TYPE_MAX? \
     -1: \
     (_bts_)->func->keycmpfunc((_bts_)->privdata, &(_e1_), &(_e2_)))

#define _getMidEntry(_i_, _n_, _mid_) \
    for ((_i_)=0, (_mid_)=(_n_)->head; \
            (_i_)<(_n_)->size/2 && (_mid_); \
            ++(_i_), _next(_mid_))

#define _creatEndEntry(_e_) do{\
    (_e_)=malloc(sizeof(bt_setsEntry)); \
    (_e_)->next=NULL; \
    (_e_)->value.type=ENTRY_TYPE_MAX; \
} while(0)

#define _freeEntry(_bts_, _ev_) do{\
    if ((_bts_)->func->valfreefunc && (_ev_)->value.type==ENTRY_TYPE_POINT) \
        (_bts_)->func->valfreefunc((_bts_)->privdata, (_ev_)->value.val.point);\
} while(0)

#define _linkEntries(_e0_, _e1_) do{\
    (_e0_)->next=(_e1_); \
    (_e1_)->prev=(_e0_); \
} while(0)

FORCE_INLINE void _bt_setsRotateRight(bt_setsNode *ln){
    bt_setsEntry *last=ln->last->prev;
    bt_setsEntry *btse=ln->pEntry;
    bt_setsNode *rn=_rightBrother(ln);

    _removeEntry(last);
    last->prev=NULL;
    _linkEntries(last, rn->head);
    rn->head=last;
    swap(btse->prev->value, last->value);

    --ln->size;
    ++rn->size;
}

FORCE_INLINE void _bt_setsRotateLeft(bt_setsNode *rn){
    bt_setsEntry *head=rn->head;
    bt_setsNode *ln=_leftBrother(rn);
    bt_setsEntry *btse=ln->pEntry;

    _removeEntry(head);
    _linkEntries(ln->last->prev, head);
    _linkEntries(head, ln->last);
    swap(btse->value, head->value);

    --rn->size;
    ++ln->size;
}

void bt_setsTraversalPrint(bt_setsNode *btsn){
    bt_setsEntry *btse=btsn->head;

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
    bt_setsEntry *btse=btsn->head;
    while (btse){
        if (btse->child){
            _bt_setsFreeNode(btse->child);
        }
        _next(btse);
    }
    free(btsn);
}

int bt_setsFind(const bt_sets *bts, const entryValue ev){
    int comp;
    bt_setsNode *btsn=bts->root;
    bt_setsEntry *btse=btsn->head;

    while (btsn){
        comp=_entryCompare(bts, ev, btse->value);
        if (comp==0){
            return BTS_ENTRY_FOUND;
        }
        else if (comp<0){
            btsn=btse->child;
            btse=btsn->head;
        }
        else {
            _next(btse);
        }
    }
    return BTS_ENTRY_NOT_FOUND;
}

static void _bt_setsNodeMerger(bt_setsNode *left, bt_setsEntry *btse,
        bt_setsNode *right){
    bt_setsEntry *last=left->last->prev;
    _setChild(btse, left->last->child);

    _linkEntries(last, btse);
    _linkEntries(btse, right->head);

    while (right->head->prev){
        _prev(right->head);
    }

    right->size+=(left->size+1);

    free(left->last);
    free(left);
}

static bt_setsEntry *_bt_setsDelEntry(bt_sets *bts, bt_setsNode *btsn,
        bt_setsEntry *btse, const unsigned int minSize){
    if (_isLeaf(btsn)){
        if (btsn->size>minSize){
            _removeEntry(btse);
            --btsn->size;
        }
        else{
            bt_setsNode *lb=_leftBrother(btsn);
            bt_setsNode *rb=_rightBrother(btsn);

            if (lb && lb->size>minSize){
                _bt_setsRotateRight(lb);
                _removeEntry(btse);
                --lb->size;
            }
            else if (rb && rb->size>minSize){
                _bt_setsRotateLeft(rb);
                _removeEntry(btse);
                --rb->size;
            }
            else{
                --btsn->parent->size;
                lb?
                    _bt_setsNodeMerger(lb, lb->pEntry, btsn):
                    _bt_setsNodeMerger(btsn, btsn->pEntry, rb);

                if (_isRoot(btsn) && btsn->parent->size==0){
                    bts->root=lb?btsn:rb;
                    --bts->height;
                    free(btsn->parent->last);
                    free(btsn->parent);
                }
            }
        }
        return btse;
    }
    else{
        if (_leftChild(btse)->size>minSize){
            swap(btse->value, _leftChild(btse)->last->prev->value);
            return _bt_setsDelEntry(bts, _leftChild(btse),
                    _leftChild(btse)->last->prev, minSize);
        }
        else if (_rightChild(btse)->size>minSize){
            swap(btse->value, _rightChild(btse)->head->value);
            return _bt_setsDelEntry(bts, _rightChild(btse),
                    _rightChild(btse)->head, minSize);
        }
        else{
            bt_setsNode *rightChild=_rightChild(btse);
            _bt_setsNodeMerger(_leftChild(btse), btse, rightChild);
            --btsn->size;
            return _bt_setsDelEntry(bts, rightChild, btse, minSize);
        }
    }
}

static bt_setsEntry *_bt_setsDelValue(bt_sets *bts, const entryValue ev,
        const unsigned int minSize){
    bt_setsNode *btsn=bts->root;
    bt_setsEntry *btse=btsn->head;
    int comp;

    while (1){
        comp=_entryCompare(bts, ev, btse->value);
        if (comp==0){
            return _bt_setsDelEntry(bts, btsn, btse, minSize);
        }
        else if (comp<0){
            if (btse->child->size==minSize){
                bt_setsNode *child=btse->child;
                bt_setsNode *lb=_leftBrother(child);
                bt_setsNode *rb=_rightBrother(child);

                if (lb && lb->size>minSize){
                    _bt_setsRotateRight(lb);
                }
                else if (rb && rb->size>minSize){
                    _bt_setsRotateLeft(rb);
                }
                else{
                    if (lb){
                        _bt_setsNodeMerger(lb, btse->prev, child);
                        _next(btse);
                    }
                    else{
                        _bt_setsNodeMerger(child, btse, rb);
                        btse=rb->head;
                    }
                    --btsn->size;

                    if (_isRoot(btsn) && btsn->size==0){
                        bts->root=lb?child:rb;
                        --bts->height;
                        free(btsn->last);
                        free(btsn);
                    }

                    continue;
                }
            }
            btsn=btse->child;
            btse=btsn->head;
        }
        else{
            _next(btse);
        }
    }
}

void bt_setsDel(bt_sets *bts, entryValue ev, const int freeval){
    if (bt_setsFind(bts, ev)==BTS_ENTRY_FOUND){
        bt_setsEntry *btse=_bt_setsDelValue(bts, ev, _minEntryNum(bts));

        if (freeval!=0){
            _freeEntry(bts, btse);
        }
        free(btse);
    }
}

bt_sets *bt_setsCreate(unsigned int d, entryFunc *func, void *privdata){
    bt_sets *bts=malloc(sizeof(bt_sets));
    bt_setsNode *btsn=malloc(sizeof(bt_setsNode));
    bt_setsEntry *btse;

    if (d<2){
        return NULL;
    }

    _creatEndEntry(btse);
    btse->prev=NULL;
    _setChild(btse, NULL);

    btsn->size=0;
    btsn->parent=NULL;
    btsn->head=btse;
    btsn->last=btse;

    bts->root=btsn;
    bts->size=0;
    bts->d=d;
    bts->privdata=privdata;
    bts->func=func;
    bts->height=1;
    return bts;
}

static int _bt_setsAddEntryToNode(bt_sets *bts, bt_setsNode *btsn,
        bt_setsEntry *btse){
    bt_setsEntry *p;
    int comp;
    entryValue ev=btse->value;

    for (p=btsn->head; p; _next(p)){
        comp=_entryCompare(bts, ev, p->value);
        if (comp==0){
            return BTREE_OPT_ERR;
        }
        else if (comp<0){
            break;
        }
    }
    if (p->prev){
        _linkEntries(p->prev, btse);
    }
    else{
        btse->prev=NULL;
    }
    _linkEntries(btse, p);

    while (btsn->head->prev){
        _prev(btsn->head);
    }
    while (btsn->last->next){
        _next(btsn->last);
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

        (*nn)->head=btsn->head;
        (*nn)->size=size/2;
        _creatEndEntry(btse);
        btse->prev=(*me)->prev;
        _setChild(btse, (*me)->child);
        (*me)->prev->next=btse;
        (*nn)->last=btse;

        btsn->head=(*me)->next;
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
        for (btse=btsn->head; btse; ){
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
            if (btsn->size>_maxEntryNum(bts)){
                _bt_setsNodeSplit(btsn, &me, &nn);
                parent=btsn->parent;
                if (parent==NULL){
                    bt_setsEntry *ne;

                    parent=malloc(sizeof(bt_setsNode));
                    parent->size=1;
                    parent->parent=NULL;
                    parent->head=me;

                    _creatEndEntry(ne);
                    ne->prev=me;
                    _setChild(ne, btsn);
                    parent->last=ne;

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
