#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "../structures/dataTranslation.h"


#define SUSPICION_THRESHOLD 1200
#define SUSPICION_CAP 4
#define SUSPICION_EXP_BASE 2
#define TURN_DEPRECATE .9
double SUSPICION_SCORE = 0;


bool alert(struct keyBundleStore* store, double*  lastEntry);
