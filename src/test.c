#include "skiplist.h"

int main(int argc, char **argv){
    skiplist *sl=skiplistInit(0.5);
    skiplistNode *sln;

    skiplistInsert(sl, "a", "hello", NULL);
    skiplistInsert(sl, "e", "world", NULL);
    skiplistInsert(sl, "w", "!", NULL);
    skiplistInsert(sl, "b", "good", NULL);
    skiplistInsert(sl, "d", "morning", NULL);
    skiplistInsert(sl, "x", "apple", NULL);
    skiplistInsert(sl, "h", "orange", NULL);
    skiplistInsert(sl, "c", "gcc", NULL);

    skiplistDelete(sl, "b");

    sln=skiplistSearch(sl, "h");
    printf("%s\n", (char *)sln->value);

    free(sl);

    return 0;
}
