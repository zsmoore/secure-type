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
#include <string.h>
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
kbs_insert_times(struct keyBundleStore *kbs, char first, char second, double travelTime, double firstDelay, double secondDelay) {
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

    kbs->kbs_lastInserted = kb;
}

void
kb_insert_time(struct keyBundle *kb, double timeToInsert, int arrType) {
    assert(kb != NULL);

    int indexToInsert = -1;
    bool replacing = false;

    double *arr = kb->k_dataTimes[arrType];

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
double
kbs_get_last_time(struct keyBundleStore *kbs, char first, char second, int arrType) {
    assert(kbs != NULL);

    struct keyBundle *kb;
    double *arr;
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

    kbs->kbs_lastInserted = NULL;

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
initArr(double *arr) {

    int i;
    for( i = 0; i < NUM_REMEMBERED; i++) {
        arr[i] = -1;
    }
}

int
findOpenIndex(double *arr) {
    int i;
    for(i = 0; i < NUM_REMEMBERED; i++){
        if (arr[i] == -1) {
            return i;
        }
    }
    return -1;
}

double *
readData(char *data) {
    double *ret = malloc((sizeof(int) * 5) + 1);
    if (ret == NULL) {
        return NULL;
    }

    char *strCopy = strdup(data);
    strCopy = strtok(strCopy, ",");
    
    int index = 0;
    while (strCopy != NULL) {
        ret[index] = atof(strCopy);
        index += 1;
        strCopy = strtok(NULL, ",");
    }
    
    return ret;
}

struct keyBundleStore *
buildFromFile(char *filename) {

    FILE *file = fopen(filename, "r");
    char *line;
    char *semiColonSplit;
    char *member;
    char *arr;
    size_t len = 0;
    ssize_t read;

    struct keyBundleStore *kbs = kbs_create();
    struct keyBundle *kb;
    int i = 0;
    int j = 0;
    int n = 0;
    int k = 0;
    if (file != NULL) {
        while((read = getline(&line, &len, file)) != -1) {
            semiColonSplit = strdup(line);
            char *end_str;
            semiColonSplit = strtok_r(semiColonSplit, ";", &end_str);
            i = 0; 
            while (semiColonSplit != NULL) {
                char *end_str2;
                member = strdup(semiColonSplit);                
                if (strcmp(member, "NULL") == 0) {
                    kbs->kbs_data[i][j] = NULL;
                    i++;
                    continue;
                }
                
                member = strtok_r(member, ",", &end_str2);

                kb = malloc(sizeof(*kb));
                n = 0;
                while (member != NULL) {
                    if (n == 0) {
                        kb->k_firstPressed = (char) atoi(member);
                    } else if (n == 1) {
                        kb->k_secondPressed = (char) atoi(member);
                    } else if(n == 2 || n == 3 || n == 4) {
                        arr = strdup(member);
                        arr = strtok(arr, "*");
                        double dataTimes[NUM_REMEMBERED];
                        k = 0;
                        while (arr != NULL) {
                            dataTimes[k] = atof(arr);
                            arr = strtok(NULL, "*");
                            k++;
                        }
                        if (n == 2) {
                            copyArr(kb, dataTimes, TRAVEL_TIMES_INDEX);
                        } else if (n == 3) {
                            copyArr(kb, dataTimes, FIRST_DELAY_INDEX);
                        } else if (n == 4) {
                            copyArr(kb, dataTimes, SECOND_DELAY_INDEX);
                        }
                    }
                    member = strtok_r(NULL, ",", &end_str2);
                    n++;
                }
                kbs->kbs_data[i][j] = kb;
                semiColonSplit = strtok_r(NULL, ";", &end_str);
                i++;
            }
            j++;
        }

    } else {
        return NULL;
    }

    fclose(file);
    if (line != NULL) {
        free(line);
    }
    return kbs;
}

void
copyArr(struct keyBundle *kb, double *toCop, int arrType) {
    assert(kb != NULL);

    int i;
    for(i = 0; i < NUM_REMEMBERED; i++) {
        kb->k_dataTimes[arrType][i] = toCop[i];
    }
}
char * structToString(struct keyBundle *kb) {

	if (kb == NULL) {
		return "NULL;";
	}
	
	char * data = (char *) malloc((sizeof(char) * NUM_REMEMBERED * NUM_TIME_ARRAYS) + (sizeof(double) * NUM_REMEMBERED * NUM_TIME_ARRAYS) + (sizeof(char) * 100));
	
	char * fChar = (char*) malloc(sizeof(char));
	char * sChar = (char*) malloc(sizeof(char));
	sprintf(fChar, "%c", kb->k_firstPressed);
	sprintf(sChar, "%c", kb->k_secondPressed);

	strcat(data, fChar);
	strcat(data, ",");
	strcat(data, sChar);
	strcat(data, ",");
	
	free(fChar);
	free(sChar);

	int i = 0;
	for (; i < NUM_TIME_ARRAYS; i++) {
		int j = 0;
		for (; j < NUM_REMEMBERED; j++) {
			char * temp = (char*) malloc(sizeof(double));
			sprintf(temp, "%f", kb->k_dataTimes[i][j]);
			strcat(data, temp);
			free(temp);
			if (j < NUM_REMEMBERED-1)
				strcat(data, "*");
		}
		if (i < NUM_TIME_ARRAYS-1)
			strcat(data, ",");
	}
	strcat(data, ";");
	return data;
}

void makeFile (struct keyBundleStore * kbs) {
	int i = 0;
	FILE * fp;

	fp = fopen ("file.txt", "w+");

	for (; i < MASS_STORE_SIZE; i++) {
		int j = 0;
		for (; j < MASS_STORE_SIZE; j++) {
			fprintf(fp, "%s", structToString(kbs->kbs_data[i][j]));			
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}
