#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <math.h>
#include "utils.h"
#include "merge.h"

int read_rec_chunk (FILE*, int, int, Record*);
int sortrun (FILE*, int, int);
