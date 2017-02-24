#define KB 1024
#define MB 1024 * 1024
#define INPUT_PREFIX "dataset/phase1_"
#include <math.h>
#include "merge.h"

typedef struct record  {
 	int uid1;
	int uid2;
} Record;

/* Process the second input argument */
int get_size_byte(char* param);

/* print Records in the array */
void print_records(Record* records, int n);

/* Get size of file */
long get_file_size(FILE* file);

/* Get remaining size of file */
long get_remain_file_size(FILE* file);

/* count the number of digits of an int */
int count_digits(int num);

/* Convert int to str */
char* int_to_string(int K);

/* Return a pointer for writing file "phase1_[chunk_id]" */
FILE* get_write_fp(int chunk_id);

/* Return a pointer for reading file "phase1_[chunk_id]" */
FILE* get_read_fp(int chunk_id);

/* Return the size of each buffer for phase II */
int get_buf_size(int mem_size, int block_size, int K);

/* Check if the memsize is enough for the structure */
int has_enough_mem(int mem_size, int buf_size, int K);

/* Ceil division */
int ceil_div(int, int);

/**
* Compares two records a and b 
* with respect to the value of the integer field f.
* Returns an integer which indicates relative order: 
* positive: record a > record b
* negative: record a < record b
* zero: equal records
*/
int compare (const void *, const void *);
