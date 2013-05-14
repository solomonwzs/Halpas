#include "dllist.h"
#include "skiplist.h"
#include "hash.h"

struct node{
    int i;
    struct node *next;
};// __attribute__((packed));

static void test_skiplist(){
    skiplist *sl=skiplistCreate(0.5, NULL);
    skiplistNode *sln;

    skiplistInsert(sl, "a", "hello");
    skiplistInsert(sl, "e", "world");
    skiplistInsert(sl, "w", "!");
    skiplistInsert(sl, "b", "good");
    skiplistInsert(sl, "d", "morning");
    skiplistInsert(sl, "x", "apple");
    skiplistInsert(sl, "h", "orange");
    skiplistInsert(sl, "c", "gcc");

    skiplistDelete(sl, "b");

    sln=skiplistSearch(sl, "h");
    printf("%s\n", (char *)sln->value);

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

int main(int argc, char **argv){
    test_skiplist();
    test_dllist();

    printf("%zu\n", sizeof(struct node));

    return 0;
}
