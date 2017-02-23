#define KB 1024
#define MB 1024 * 1024
#include <math.h>
typedef struct record  {
 	int uid1;
	int uid2;
} Record;

/* Process the second input argument */
int get_size_byte(char*);

/* print Records in the array */
void print_records(Record*, int);

/* Get size of file */
long get_file_size(FILE*);

/* Get remaining size of file */
long get_remain_file_size(FILE*);

/* Ceil division */
int ceil_div(int, int);
