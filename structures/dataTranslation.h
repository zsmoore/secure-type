#ifndef _DATATRANSLATION_H_
#define _DATATRANSLATION_H_
/*
 *
 *  Header file for reading data and putting into data structures
 *
 */
#define TRAVEL_TIMES_INDEX 0
#define FIRST_DELAY_INDEX 1
#define SECOND_DELAY_INDEX 2

#define NUM_TIME_ARRAYS 3

#define NUM_REMEMBERED 30
#define MASS_STORE_SIZE 200
////////////////////////
//  Struct for holding data
struct keyBundle {
    char k_firstPressed;
    char k_secondPressed;
    double k_dataTimes[NUM_TIME_ARRAYS][NUM_REMEMBERED];
    double k_dataTimesOldest[NUM_TIME_ARRAYS];
};

struct keyBundle *kb_create(char first, char second);
void kb_destroy(struct keyBundle *);

////////////////////////////
//  Mass Store
struct keyBundleStore {
    struct keyBundle *kbs_data[MASS_STORE_SIZE][MASS_STORE_SIZE];
    struct keyBundle *kbs_lastInserted;
};

struct keyBundleStore *kbs_create(void);
void kbs_destroy(struct keyBundleStore *);

//////////////////////////
// Key Bundle Helpers
void kb_insert_time(struct keyBundle *, double timeToInsert, int arrType);

////////////////////////////
//  Mass Store Helpers
void kbs_insert_times(struct keyBundleStore *, char first, char second, double travelTime, double firstDelay, double secondDelay);
double kbs_get_last_time(struct keyBundleStore *, char first, char second, int arrType);


///////////////////////
///  Read in data
double *readData(char *);
struct keyBundleStore *buildFromFile(char *filename);

////////////////////////////
//  General Helpers
void initArr(double *);
int findOpenIndex(double *);
void copyArr(struct keyBundle *, double *, int arrType);
#endif
