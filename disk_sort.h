#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <math.h>
#include "utils.h"

int compare (const void*, const void*);
int read_rec_chunk(FILE*, int, int, Record*);