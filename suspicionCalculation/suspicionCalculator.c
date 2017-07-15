#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "suspicionCalculator.h"
double SUSPICION_SCORE = 0;

bool alert(struct keyBundleStore* store, double*  lastEntry){
	int char1 = lastEntry[0];
	int char2 = lastEntry[1];
	double lastInterval = lastEntry[2];
	double lastD1 = lastEntry[3];
	double lastD2 = lastEntry[4];

	struct keyBundle *bundle = store->kbs_data[char1][char2];
	
	double avgD1, avgD2, avgInterval, stdD1, stdD2, stdInterval;
	avgD1 = avgD2 = avgInterval = stdD1 = stdD2 = stdInterval = 0;

	for(int i = 0; i < NUM_REMEMBERED; ++i){
		avgInterval += bundle->k_dataTimes[TRAVEL_TIMES_INDEX][i];
		avgD1 += bundle->k_dataTimes[FIRST_DELAY_INDEX][i];
		avgD2 += bundle->k_dataTimes[SECOND_DELAY_INDEX][i];
	}
	avgInterval /= NUM_REMEMBERED;
	avgD1 /= NUM_REMEMBERED;
	avgD2 /= NUM_REMEMBERED;

	for(int i = 0; i < NUM_REMEMBERED; ++i){
		stdD1 += pow((avgD1 - bundle->k_dataTimes[FIRST_DELAY_INDEX][i]), 2);		
		stdD2 += pow((avgD2 - bundle->k_dataTimes[SECOND_DELAY_INDEX][i]), 2);
		stdInterval += pow((avgInterval - bundle->k_dataTimes[TRAVEL_TIMES_INDEX][i]), 2);
	}
	stdD1 = sqrt(stdD1/NUM_REMEMBERED);
	stdD2 = sqrt(stdD2/NUM_REMEMBERED);
	stdInterval = sqrt(stdInterval/NUM_REMEMBERED);
	
	double suspD1, suspD2, suspInterval;
	suspInterval = fabs(lastInterval - avgInterval)/stdInterval;
	suspD1 = fabs(lastD1 - avgD1)/stdD1;
	suspD2 = fabs(lastD2 - avgD2)/stdD2;

	int totalSuspicion = 0;
	if(suspD1 > SUSPICION_CAP){suspD1 = SUSPICION_CAP; totalSuspicion++;}
	if(suspD2 > SUSPICION_CAP){suspD2 = SUSPICION_CAP; totalSuspicion++;}
	if(suspInterval > SUSPICION_CAP){suspInterval = SUSPICION_CAP; totalSuspicion++;}

	SUSPICION_SCORE = SUSPICION_SCORE * TURN_DEPRECATE +
		pow(SUSPICION_EXP_BASE, suspInterval) + pow(SUSPICION_EXP_BASE, suspD1) + pow(SUSPICION_EXP_BASE, suspD2);

	return (totalSuspicion > 1);
}

