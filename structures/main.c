#include "dataTranslation.h"
#include <stdio.h>

int
main(void) {

    struct keyBundle *test = kb_create('a', 'b');

    struct keyBundleStore *testkbs = kbs_create();

    kb_insert_time(test, 1, 0);

    kbs_insert_times(testkbs, 'a', 'b', 1, 2, 3);

    printf("THIS IS IS:\t%d\n", kbs_get_last_time(testkbs, 'a', 'b', 0));

    return 1;
}
