#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FLAG_UID1 1
#define FLAG_UID2 2
#define INIT_ARRAY_SIZE 128

typedef struct DegreeArray {
	int *array;
	int max_degree;
} DegreeArray;


/**
 * Return an array of int called degree_array which records the number
 * of users for each degree
 **/
DegreeArray *count_degree(FILE* fp_read, int block_size, int uid_flag, DegreeArray *degree_array);

/**
 * get flag for UID1 or UID2
 **/
int get_uid_flag(char* column_id);

/**
 * Initialize degree_array
 **/
DegreeArray *init_degree_array();

/**
 * Increase the size of degree_array->array for higher max degree
 **/
DegreeArray *increase_array_size(DegreeArray *degree_array, int new_max_degree);

/**
 * Set the space re-allocated to zero
 **/
void zero_out_new_space(DegreeArray *degree_array, int new_max_degree);

/**
 * Cleanup degree_array
 **/
void cleanup(DegreeArray *degree_array);

/**
 * Print the contents of degree_array to stdout
 **/
void output_degree_array(DegreeArray *degree_array);