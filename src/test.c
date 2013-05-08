#include "skiplist.h"

int main(int argc, char **argv){
    skiplist *sl=skiplistInit(0.5);
    int a=1, b=2;

    skiplistInsert(sl, "a", &a, NULL);
    skiplistInsert(sl, "b", &b, NULL);

    printf("%d\n", *((int *)skiplistSearch(sl, "a")));

    free(sl);
    return 0;
}
