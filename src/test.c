#include "dllist.h"
#include "skiplist.h"

int main(int argc, char **argv){
    skiplist *sl=skiplistCreate(0.5, NULL);
    skiplistNode *sln;

    //dllist *dl=dllistCreate(NULL, NULL);

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

    return 0;
}
