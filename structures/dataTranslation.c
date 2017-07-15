/*
 *
 * Definitions for reading data and putting into data structures
 *
 */

#include "dataTranslation.h"
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

/////////////////////////////
// Key Bundle Struct
struct keyBundle *
kb_create(char first, char second) {

    struct keyBundle *kb;

    kb = malloc(sizeof(*kb));
    if (kb == NULL) {
        return NULL;
    }

    kb->k_firstPressed = first;
    kb->k_secondPressed = second;

    int i;
    //init data time arrays
    for (i = 0; i < NUM_TIME_ARRAYS; i++) {
        initArr(kb->k_dataTimes[i]);
    }
    
    //init old times
    for (i = 0; i < NUM_TIME_ARRAYS; i++) {
        kb->k_dataTimesOldest[i] = -1;
    }

    return kb;
}

void
kb_destroy(struct keyBundle *kb) {
    assert(kb != NULL);

    free(kb);
}
//////////////////////////////
// Key Bundle Helpers
void 
kbs_insert_times(struct keyBundleStore *kbs, char first, char second, int travelTime, int firstDelay, int secondDelay) {
    assert(kbs != NULL);

    //Find or create our struct
    struct keyBundle *kb;
    if (kbs->kbs_data[first][second] == NULL) {
        kb = kb_create(first, second);
        kbs->kbs_data[first][second] = kb;
    } else {
        kb = kbs->kbs_data[first][second];
    }

    //Insert each time
    kb_insert_time(kb, travelTime, TRAVEL_TIMES_INDEX);
    kb_insert_time(kb, firstDelay, FIRST_DELAY_INDEX);
    kb_insert_time(kb, secondDelay, SECOND_DELAY_INDEX);
}

void
kb_insert_time(struct keyBundle *kb, int timeToInsert, int arrType) {
    assert(kb != NULL);

    int indexToInsert = -1;
    bool replacing = false;

    int *arr = kb->k_dataTimes[arrType];

    //Find index to put travelTime
    if (kb->k_dataTimesOldest[arrType] == -1) {
        indexToInsert = findOpenIndex(kb->k_dataTimes[arrType]);
        
        //If arr full begin replacing
        if (indexToInsert == -1) {
            kb->k_dataTimesOldest[arrType] = 0;
            replacing = true;
        }
    } else {
        indexToInsert = kb->k_dataTimesOldest[arrType];
        replacing = true;
    }

    arr[indexToInsert] = timeToInsert;
    
    //Update replacing.  If bigger than array wrap to 0
    if (replacing) {
        kb->k_dataTimesOldest[arrType] += 1;
        if(kb->k_dataTimesOldest[arrType] >= NUM_REMEMBERED) {
            kb->k_dataTimesOldest[arrType] = 0;
        }
    }
}

//Return -1 if nothing otherwise last time
int
kbs_get_last_time(struct keyBundleStore *kbs, char first, char second, int arrType) {
    assert(kbs != NULL);

    struct keyBundle *kb;
    int *arr;
    int toReturn;
    //If what we want is not null
    if (kbs->kbs_data[first][second] != NULL) {
        
        //get what we want
        kb = kbs->kbs_data[first][second];
        
        //get proper array
        arr = kb->k_dataTimes[arrType];
        
        //if we have not replaced yet
        if (kb->k_dataTimesOldest[arrType] == -1) {
            
            //find open
            toReturn = findOpenIndex(arr);
            
            //if nothing in arr return -1
            if (toReturn == 0) {
                return -1;
            } else {
                //if something in arr return the freshest element
                return kb->k_dataTimes[arrType][toReturn - 1];
            }
        } else {
            //if we have replaced

            //get the index of oldest item
            toReturn = kb->k_dataTimesOldest[arrType];
            //if oldest is at 0 we want to wrap around to last index
            if (toReturn == 0) {
                //return element at end of list
                return kb->k_dataTimes[arrType][NUM_REMEMBERED - 1];
            } else {
                //return element before oldest
                return kb->k_dataTimes[arrType][toReturn -1];
            }
        }
    } else {
        //return negative 1 if not possible
        return -1;
    }


}

///////////////////////////
// Mass Store Struct
struct keyBundleStore *
kbs_create(void) {

    struct keyBundleStore *kbs;

    kbs = malloc(sizeof(*kbs));
    if (kbs == NULL) {
        return NULL;
    }

    return kbs;
}

void
kbs_destroy(struct keyBundleStore *kbs) {
    assert(kbs != NULL);
    
    int i, j;
    for(i = 0; i < MASS_STORE_SIZE; i++) {
        for(j = 0; j < MASS_STORE_SIZE; j++) {
            if (kbs->kbs_data[i][j] != NULL) {
                kb_destroy(kbs->kbs_data[i][j]);
            }
        }
    }

    free(kbs);
}

////////////////////////////
//  General Helpers
void
initArr(int *arr) {

    int i;
    for( i = 0; i < NUM_REMEMBERED; i++) {
        arr[i] = -1;
    }
}

int
findOpenIndex(int *arr) {
    int i;
    for(i = 0; i < NUM_REMEMBERED; i++){
        if (arr[i] == -1) {
            return i;
        }
    }
    return -1;
}
