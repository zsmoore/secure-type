#include "dataTranslation.h"
#include <stdio.h>

int
main(void) {

    struct keyBundle *test = kb_create('a', 'b');
    kb_destroy(test);

    struct keyBundleStore *testkbs = kbs_create();
    kbs_destroy(testkbs);

    kb_insert_time(test, 1, 0);

    kbs_insert_times(testkbs, 'a', 'b', 1, 2, 3);

    return 1;
}
