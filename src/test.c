#include "dict.h"
#include "dllist.h"
#include "skiplist.h"
#include "hash.h"

int test_keycmp_func(void *privdata, const void *key1, const void *key2){
    return strcmp(key1, key2);
}

static void test_skiplist(){
    skiplistFunc slf;
    skiplist *sl;
    skiplistNode *sln;

    slf.keycmpfunc=test_keycmp_func;
    slf.keyfreefunc=NULL;
    slf.valfreefunc=NULL;
    slf.keydupfunc=NULL;
    slf.valdupfunc=NULL;

    sl=skiplistCreate(0.5, &slf, NULL);

    //skiplistInsert(sl, "a", "hello");
    //skiplistInsert(sl, "e", "world");
    //skiplistInsert(sl, "w", "!");
    //skiplistInsert(sl, "b", "good");
    //skiplistInsert(sl, "d", "morning");
    //skiplistInsert(sl, "x", "apple");
    //skiplistInsert(sl, "h", "orange");
    //skiplistInsert(sl, "c", "gcc");
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
    printf("%s\n", (char *)sln->value.point);

    skiplistFree(sl);
}

static int dllist_filter(void *s){
    return *((char *)s)%2==0?1:-1;
}

static void test_dllist(){
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
    dictFunc df;
    dict *d;
    dictEntry *de;
    int i;
    char *s[6]={"hello", "world", "!", "test", "dict", "func"};

    df.hashfunc=test_dict_hash_func;
    df.keycmpfunc=test_keycmp_func;
    df.keydupfunc=NULL;
    df.keyfreefunc=NULL;
    df.valdupfunc=NULL;
    df.valfreefunc=NULL;
    d=dictCreate(&df, NULL, 2);

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
            if (de->type==DICT_ENTRY_TYPE_UINT){
                printf("%s\t%li\n", s[i], de->value.ui);
            }
            else if (de->type==DICT_ENTRY_TYPE_POINT){
                printf("%s\t%s\n", s[i], (char *)de->value.point);
            }
        }
        else{
            printf("%s\tnull\n", s[i]);
        }
    }

    dictFree(d);
}

int main(int argc, char **argv){
    test_dict();
    test_skiplist();
    test_dllist();
    test_hash();

    printf("%zu\n", sizeof(dictEntry));

    return 0;
}
