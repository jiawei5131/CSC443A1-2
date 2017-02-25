#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <math.h>

/**
 * Sort the binary file pointed by fp_read in memory, and write each
 * chunk into a separate file called phase1_n where n is the chunk ID
 *
 * Return number of chunks sorted
 **/
int sortrun(FILE* fp_read, int mem_size, int block_size);
