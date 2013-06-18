#include "dict.h"
#include "dllist.h"
#include "skiplist.h"
#include "hash.h"
#include "btree.h"

#define printDivLine(X) printf("==========%s==========\n", X)

int test_keycmp_func(void *privdata, const void *key1, const void *key2){
    return strcmp(key1, key2);
}

static void test_skiplist(){
    printDivLine("test_skiplist");

    entryFunc ef;
    skiplist *sl;
    skiplistNode *sln;

    setEntryFunc(ef, NULL, test_keycmp_func, NULL, NULL, NULL, NULL);
    sl=skiplistCreate(0.5, &ef, NULL);

    sln=skiplistAddRaw(sl, "a"); skiplistSetPoint(sl, sln, "hello");
    sln=skiplistAddRaw(sl, "e"); skiplistSetPoint(sl, sln, "world");
    sln=skiplistAddRaw(sl, "w"); skiplistSetPoint(sl, sln, "!");
    sln=skiplistAddRaw(sl, "b"); skiplistSetPoint(sl, sln, "good");
    sln=skiplistAddRaw(sl, "d"); skiplistSetPoint(sl, sln, "morning");
    sln=skiplistAddRaw(sl, "x"); skiplistSetPoint(sl, sln, "apple");
    sln=skiplistAddRaw(sl, "h"); skiplistSetPoint(sl, sln, "orange");
    sln=skiplistAddRaw(sl, "c"); skiplistSetPoint(sl, sln, "gcc");

    skiplistDelete(sl, "b", 0);

    sln=skiplistSearch(sl, "h");
    printf("%s\n", (char *)skiplistNodePoint(sln));

    skiplistFree(sl);
}

static int dllist_filter(void *s){
    return *((char *)s)%2==0?1:-1;
}

static void test_dllist(){
    printDivLine("test_dllist");

    dllist *dl=dllistCreate(NULL, NULL);
    dllistNode *dln;

    dllistLeftPush(dl, "1");
    dllistLeftPush(dl, "2");
    dllistRightPush(dl, "3");
    dllistLeftPush(dl, "4");
    dllistRightPull(dl);
    dllistRightPush(dl, "5");
    dllistRightPush(dl, "6");
    dllistLeftPush(dl, "7");

    dln=dl->head;
    while (dln){
        printf("%s,", (char *)dln->value);
        dln=dln->next;
    }
    printf("\n");
    dllistReverse(dl);
    dln=dl->head;
    while (dln){
        printf("%s,", (char *)dln->value);
        dln=dln->next;
    }
    printf("\n");

    dllistFilter(dl, dllist_filter);
    dln=dl->head;
    while (dln){
        printf("%s,", (char *)dln->value);
        dln=dln->next;
    }
    printf("\n");

    dln=dl->tail;
    while (dln){
        printf("%s,", (char *)dln->value);
        dln=dln->prev;
    }
    printf("\n");

    dllistFree(dl);
}

void test_hash(){
    printDivLine("test_hash");

    uint32_t h[4];
    char *key="k&JHBJdsbg&^Tdhw3*(W&fhejkvbt$#25/";
    murmurHash_x64_128(key, strlen(key), 19881218, h);

    printf("%8x %8x %8x %8x\n", h[0], h[1], h[2], h[3]);
}

unsigned long test_dict_hash_func(void *privdata, const void *key){
    uint32_t h[4];
    murmurHash_x64_128(key, strlen(key), 19881218, h);

    return h[0];
}

void test_dict(){
    printDivLine("test_dict");

    entryFunc ef;
    dict *d;
    dictEntry *de;
    int i;
    char *s[6]={"hello", "world", "!", "test", "dict", "func"};

    setEntryFunc(ef, test_dict_hash_func, test_keycmp_func, NULL, NULL, NULL,
            NULL);
    d=dictCreate(&ef, NULL, 2);

    de=dictAddRaw(d, "hello"); dictSetUnsigedInteger(de, 0);
    de=dictAddRaw(d, "world"); dictSetUnsigedInteger(de, 1);
    de=dictAddRaw(d, "!"); dictSetUnsigedInteger(de, 2);
    de=dictAddRaw(d, "test"); dictSetUnsigedInteger(de, 3);
    de=dictAddRaw(d, "dict"); dictSetUnsigedInteger(de, 4);
    de=dictAddRaw(d, "func"); dictSetPoint(d, de, "string");

    printf("%d\n", dictDelete(d, "!", 0));

    for (i=0; i<6; ++i){
        de=dictFind(d, s[i]);
        if (de){
            if (dictEntryType(de)==ENTRY_TYPE_UINT){
                printf("%s\t%li\n", s[i], dictEntryUInt(de));
            }
            else if (dictEntryType(de)==ENTRY_TYPE_POINT){
                printf("%s\t%s\n", s[i], (char *)dictEntryPoint(de));
            }
        }
        else{
            printf("%s\tnull\n", s[i]);
        }
    }

    dictFree(d);
}

int test_cmp_func(void *privdata, const void *ev1,
        const void *ev2){
    return strcmp(((entryValue *)ev1)->val.point,
            ((entryValue *)ev2)->val.point);
}

static void test_btree(){
    printDivLine("test_btree");

    entryFunc ef;
    entryValue ev;
    bt_sets *bts;
    int i;
    char *str[20]={"C", "N", "G", "A", "H", "E", "K", "Q", "M", "F", "W", "L",
        "T", "Z", "D", "P", "R", "X", "Y", "S"};

    setEntryFunc(ef, NULL, test_cmp_func, NULL, NULL, NULL,
            NULL);
    bts=bt_setsCreate(3, &ef, NULL);

    ev.type=ENTRY_TYPE_POINT;
    for (i=0; i<20; ++i){
        ev.val.point=str[i];
        bt_setsAdd(bts, ev);
        bt_setsTraversalPrint(bts->root);
        printf("\n");
    }

    for (i=0; i<20; ++i){
        //printf("%s ", str[i]);
        //fflush(stdout);
        ev.val.point=str[i];
        bt_setsDel(bts, ev, 0);
        bt_setsTraversalPrint(bts->root);
        printf("\n");
    }

    bt_setsFree(bts);
}

int main(int argc, char **argv){
    test_dict();
    test_skiplist();
    test_dllist();
    test_hash();
    test_btree();

    return 0;
}
