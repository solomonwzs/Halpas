#include "btree.h"

bt_sets *bt_setsCreate(unsigned int keyNum){
    bt_sets *bts=malloc(sizeof(bt_sets));

    if (bts){
        bts->root=NULL;
        bts->size=0;
        bts->keyNum=keyNum;
    }
    return bts;
}
