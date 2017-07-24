#include "dataTranslation.h"
#include <stdio.h>

int
main(void) {

    //struct keyBundle *test = kb_create('a', 'b');

    //struct keyBundleStore *testkbs = kbs_create();

    //kb_insert_time(test, 1, 0);

    //kbs_insert_times(testkbs, 'a', 'b', 1, 2, 3);
	//printf( "%s\n", structToString(test));

    //kbs_insert_times(testkbs, 'a', 'b', 1, 2, 3);
	//makeFile(testkbs);
	
    //printf("THIS IS IS:\t%d\n", kbs_get_last_time(testkbs, 'a', 'b', 0));
    
    //double *testReturn = readData("1,2,3,4,5");
    //int i = 0;
    //for ( i = 0; i < 5; i++) {
    //    printf("THis is the num:\t%f\n", testReturn[i]);;
    //}
    struct keyBundleStore *kbs = buildFromFile("testData.csv");
    //printf("This is the data:\t%f\n", kbs->kbs_data[30][30]->k_dataTimes[2][20]);

    makeFile(kbs);

    struct keyBundleStore *kbs2 = buildFromFile("file.txt");
    //printf("This is the new Data:\t%f\n", kbs2->kbs_data[30][30]->k_dataTimes[2][20]);

    //double toTest[5] = {1.00, 1.00, 2.00, 2.00, 2.00};

    //printf("Are we sus?\t%d\n", alert(kbs, toTest));

    return 1;
}
