#include "btree.h"

#define _next(_e_) \
    (_e_)=(_e_)->next

#define _prev(_e_) \
    (_e_)=(_e_)->prev

#define _lastEntry(_n_) \
    ((_n_)->last->prev)

#define _isRoot(_n_) \
    ((_n_)->pEntry==NULL)

#define _isLeaf(_n_) \
    ((_n_)->head->child==NULL)

#define _isHead(_e_) \
    ((_e_)->prev==NULL)

#define _checkHead(_n_, _e_) do{\
    if (_isHead(_e_)){\
        _next((_n_)->head); \
        (_n_)->head->prev=NULL; \
    }\
} while(0)

#define _checkRoot(_s_, _n_) do{\
    if (_isRoot(_n_) && (_n_)->size==0){\
        (_s_)->root=(_n_)->last->child; \
        (_s_)->root->pEntry=NULL; \
        --(_s_)->height; \
        free((_n_)->last); \
        free(_n_); \
    }\
} while(0)

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

static bt_setsEntry *_bt_setsDelMax(bt_sets *bts, bt_setsNode *btsn);
static bt_setsEntry *_bt_setsDelMin(bt_sets *bts, bt_setsNode *btsn);

static void _bt_setsRotateRight(bt_sets *bts, bt_setsNode *ln){
    bt_setsEntry *last=ln->last->prev;
    bt_setsEntry *btse=ln->pEntry;
    bt_setsNode *rn=_rightBrother(ln);
    bt_setsNode *tmp;

    _removeEntry(last);
    last->prev=NULL;

    _linkEntries(last, rn->head);
    rn->head=last;

    tmp=ln->last->child;
    _setChild(ln->last, last->child);
    _setChild(last, tmp);
    swap(btse->value, last->value);

    --ln->size;
    ++rn->size;
}

static void _bt_setsRotateLeft(bt_sets *bts, bt_setsNode *rn){
    bt_setsEntry *head=rn->head;
    bt_setsNode *ln=_leftBrother(rn);
    bt_setsEntry *btse=ln->pEntry;
    bt_setsNode *tmp;

    _next(rn->head);
    _removeEntry(head);

    _linkEntries(ln->last->prev, head);
    _linkEntries(head, ln->last);

    tmp=ln->last->child;
    _setChild(ln->last, head->child);
    _setChild(head, tmp);
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

static void _bt_setsFreeNode(bt_sets *bts, bt_setsNode *btsn){
    bt_setsEntry *btse=btsn->head, *next;
    while (btse){
        if (btse->child){
            _bt_setsFreeNode(bts, btse->child);
        }
        next=btse->next;
        if (bts){
            _freeEntry(bts, btse);
        }
        free(btse);
        btse=next;
    }
    free(btsn);
}

bt_setsEntry *bt_setsFind(const bt_sets *bts, const entryValue ev){
    int comp;
    bt_setsNode *btsn=bts->root;
    bt_setsEntry *btse=btsn->head;

    while (btsn){
        comp=_entryCompare(bts, ev, btse->value);
        if (comp==0){
            return btse;
        }
        else if (comp<0){
            if (_isLeaf(btsn)){
                return NULL;
            }
            btsn=btse->child;
            btse=btsn->head;
        }
        else {
            _next(btse);
        }
    }
    return NULL;
}

static void _bt_setsNodeMerger(bt_setsNode *left, bt_setsEntry *btse,
        bt_setsNode *right){
    bt_setsEntry *last=left->last->prev;

    _setChild(btse, left->last->child);

    _linkEntries(last, btse);
    _linkEntries(btse, right->head);

    right->head=left->head;
    right->size+=(left->size+1);

    free(left->last);
    free(left);
}

static bt_setsEntry *_bt_setsDelEntry(bt_sets *bts, bt_setsNode *btsn,
        bt_setsEntry *btse){
    bt_setsEntry *e;

    if (_isLeaf(btsn)){
        _checkHead(btsn, btse);
        _removeEntry(btse);
        --btsn->size;
        return btse;
    }
    else{
        if (_leftChild(btse)->size>_minEntryNum(bts)){
            e=_bt_setsDelMax(bts, _leftChild(btse));
            swap(btse->value, e->value);
            return e;
        }
        else if (_rightChild(btse)->size>_minEntryNum(bts)){
            e=_bt_setsDelMin(bts, _rightChild(btse));
            swap(btse->value, e->value);
            return e;
        }
        else{
            bt_setsNode *rightChild=_rightChild(btse);
            bt_setsNode *leftChild=_leftChild(btse);

            _checkHead(btsn, btse);
            _removeEntry(btse);
            _bt_setsNodeMerger(leftChild, btse, rightChild);
            --btsn->size;
            _checkRoot(bts, btsn);
            return _bt_setsDelEntry(bts, rightChild, btse);
        }
    }
}

static bt_setsEntry *_bt_setsDelMax(bt_sets *bts, bt_setsNode *btsn){
    bt_setsEntry *last;

    if (_isLeaf(btsn)){
        last=btsn->last->prev;
        _removeEntry(last);
        --btsn->size;
        return last;
    }
    else{
        bt_setsNode *child=btsn->last->child;
        bt_setsNode *lb=_leftBrother(child);
        unsigned int minSize=_minEntryNum(bts);

        if (child->size>minSize){
        }
        else if (lb->size>minSize){
            _bt_setsRotateRight(bts, lb);
        }
        else{
            last=btsn->last->prev;
            _removeEntry(last);
            --btsn->size;
            _bt_setsNodeMerger(lb, last, child);
        }
        return _bt_setsDelMax(bts, child);
    }
}

static bt_setsEntry *_bt_setsDelMin(bt_sets *bts, bt_setsNode *btsn){
    bt_setsEntry *head;

    if (_isLeaf(btsn)){
        head=btsn->head;
        _next(btsn->head);
        _removeEntry(head);
        --btsn->size;
        return head;
    }
    else{
        bt_setsNode *child=btsn->head->child;
        bt_setsNode *rb=_rightBrother(child);
        unsigned int minSize=_minEntryNum(bts);

        if (child->size>minSize){
            return _bt_setsDelMin(bts, child);
        }
        if (rb->size>minSize){
            _bt_setsRotateLeft(bts, rb);
            return _bt_setsDelMin(bts, child);
        }
        else{
            head=btsn->head;
            _next(btsn->head);
            _removeEntry(head);
            --btsn->size;
            _bt_setsNodeMerger(child, head, rb);
            return _bt_setsDelMin(bts, rb);
        }
    }
}

static bt_setsEntry *_bt_setsDelValue(bt_sets *bts, const entryValue ev){
    bt_setsNode *btsn=bts->root;
    bt_setsEntry *btse=btsn->head;
    int comp;
    unsigned int minSize=_minEntryNum(bts);

    while (1){
        comp=_entryCompare(bts, ev, btse->value);
        if (comp==0){
            return _bt_setsDelEntry(bts, btsn, btse);
        }
        else if (comp<0){
            if (btse->child->size==minSize){
                bt_setsNode *child=btse->child;
                bt_setsNode *lb=_leftBrother(child);
                bt_setsNode *rb=_rightBrother(child);

                if (lb && lb->size>minSize){
                    _bt_setsRotateRight(bts, lb);
                }
                else if (rb && rb->size>minSize){
                    _bt_setsRotateLeft(bts, rb);
                }
                else{
                    if (lb){
                        bt_setsEntry *mid=btse->prev;
                        _checkHead(btsn, mid);
                        _removeEntry(mid);
                        _bt_setsNodeMerger(lb, mid, child);
                        btse=mid->next;
                    }
                    else{
                        _checkHead(btsn, btse);
                        _removeEntry(btse);
                        _bt_setsNodeMerger(child, btse, rb);
                        btse=rb->head;
                    }
                    --btsn->size;
                    _checkRoot(bts, btsn);
                    btsn=lb?child:rb;

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
    if (bt_setsFind(bts, ev)){
        bt_setsEntry *btse=_bt_setsDelValue(bts, ev);

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
    btsn->pEntry=NULL;
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
        bt_setsEntry *btse, bt_setsEntry *se){
    bt_setsEntry *p;
    int comp;
    entryValue ev=btse->value;

    for (p=se?se:btsn->head; p; _next(p)){
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
        btsn->head=btse;
    }
    _linkEntries(btse, p);
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
        _linkEntries((*me)->prev, btse);
        (*nn)->last=btse;
        _setChild(btse, (*me)->child);

        btsn->head=(*me)->next;
        btsn->head->prev=NULL;
        btsn->size=size-size/2-1;

        return BTREE_OPT_OK;
    }
    return BTREE_OPT_ERR;
}

void bt_setsFree(bt_sets *bts, const int freeval){
    if (freeval!=0){
        _bt_setsFreeNode(bts, bts->root);
    }
    else{
        _bt_setsFreeNode(NULL, bts->root);
    }
    free(bts);
}

bt_setsEntry *bt_setsAdd(bt_sets *bts, entryValue ev){
    bt_setsNode *btsn, **parents;
    bt_setsEntry *btse, *me;
    unsigned int i;
    int comp;
    bt_setsNode *sn=NULL, *nn, *parent;
    bt_setsEntry *se=NULL;

    btsn=bts->root;
    parents=malloc(sizeof(bt_setsNode)*bts->height);
    parents[0]=bts->root;
    for (i=1; i<bts->height; ++i){
        for (btse=btsn->head; btse; ){
            comp=_entryCompare(bts, ev, btse->value);
            if (comp>0){
                btse=btse->next;
            }
            else{
                if (comp==0){
                    free(parents);
                    return NULL;
                }
                btsn=btse->child;
                parents[i]=btsn;
                break;
            }
        }
    }
    btse=malloc(sizeof(bt_setsEntry));
    btse->value=ev;
    _setChild(btse, NULL);

    ++bts->size;
    me=btse;
    i=bts->height-1;
    while (_bt_setsAddEntryToNode(bts, btsn, me, se)==BTREE_OPT_OK){
        if (sn){
            _setChild(me->next, sn);
            sn=NULL;
        }
        if (btsn->size>_maxEntryNum(bts)){
            _bt_setsNodeSplit(btsn, &me, &nn);
            parent=(--i)>=0 && i<bts->height?parents[i]:NULL;
            if (parent==NULL){
                bt_setsEntry *ne;

                parent=malloc(sizeof(bt_setsNode));
                parent->size=1;
                parent->pEntry=NULL;
                parent->head=me;

                _creatEndEntry(ne);
                ne->prev=me;
                parent->last=ne;
                _setChild(ne, btsn);

                me->next=ne;
                me->prev=NULL;
                _setChild(me, nn);

                bts->root=parent;
                ++bts->height;

                free(parents);
                return btse;
            }
            else{
                _setChild(me, nn);
                sn=btsn;
                se=btsn->pEntry;
                btsn=parent;
            }
        }
        else{
            free(parents);
            return btse;
        }
    }

    free(parents);
    free(btse);
    return NULL;
}
