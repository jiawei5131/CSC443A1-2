#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "distribution.h"


/**
 * Return an array of int called degree_array which records the number
 * of users for each degree
 **/
DegreeArray *count_degree(FILE* fp_read, 
				  int block_size, 
				  int uid_flag,		// UID1 or UID2
				  DegreeArray *degree_array)
{
	int num_blocks;
	int rec_per_block = block_size / sizeof(Record);
	long file_size;
	Record *buffer = NULL;
	Record *cur_rec = NULL;

	/* malloc buffer */
	if ( (buffer = calloc(rec_per_block , sizeof(Record))) == NULL ){
		fprintf(stderr, "Calloc buffer failed. \n");
		return NULL;
	}

	/* calculate number of blocks in the file */
	file_size = get_file_size(fp_read);
	if ( file_size == -1L ){
		fprintf(stderr, "get_file_size \n");
		return NULL;
	}

	num_blocks = 1 + (file_size - 1) / block_size;

	int MAX = degree_array->max_degree;	// max degree
	int cur_uid_degree = 0;	// num of follower for current uid
	int cur_uid = 0;
	int last_uid = cur_uid;

	int i, num_rec_read, rec_count;
	for (i = 0; i < num_blocks; i ++){
		/* read a block */
		num_rec_read = fread(buffer, sizeof(Record), rec_per_block, fp_read);
		if ( num_rec_read != rec_per_block && !feof(fp_read) ){
			fprintf(stderr, "fread records \n");
			return NULL;
		} 

		/* reset to head of the buffer */
		cur_rec = buffer;
		for (rec_count = 0; rec_count < num_rec_read; rec_count ++, cur_rec ++){
			if (uid_flag == FLAG_UID1){
				cur_uid = cur_rec->uid1;	
			} else {
				cur_uid = cur_rec->uid2;
			}
			

			/* beginning of records */
			if (last_uid == 0){
				last_uid = cur_uid;
			}

			if (cur_uid == last_uid){
				/* same user */
				cur_uid_degree ++;
			} else {
				/* end of a user */
				if (cur_uid_degree > MAX){
					degree_array = increase_array_size(degree_array, cur_uid_degree);
					MAX = degree_array->max_degree;
				}
				degree_array->array[cur_uid_degree] ++;

				/* switch user */
				last_uid = cur_uid;
				cur_uid_degree = 1;

			}
		}
	}

	/* clean up */
	free(buffer);

	return degree_array;
}


/**
 * get UID1 or UID2
 **/
int get_uid_flag(char* column_id){
	if (strcmp("UID1", column_id) == 0){
		return FLAG_UID1;
	} else if (strcmp("UID2", column_id) == 0){
		return FLAG_UID2;
	} else {
		fprintf(stderr, "<column_id>: must be \"UID1\" or \"UID2\" \n");
		return -1;
	}
}


/**
 * Initialize degree_array
 **/
DegreeArray *init_degree_array(){
	DegreeArray *degree_array;

	if ( (degree_array = calloc(1, sizeof(DegreeArray))) == NULL ){
		fprintf(stderr, "init_degree_array: calloc DegreeArray failed. \n");
		return NULL;	
	}

	if ( (degree_array->array = calloc(INIT_ARRAY_SIZE, sizeof(int))) == NULL ){
		fprintf(stderr, "init_degree_array: calloc DegreeArray->array failed. \n");
		return NULL;
	}
	degree_array->max_degree = INIT_ARRAY_SIZE - 1; // including 0

	return degree_array;
}


/**
 * Increase the size of degree_array->array for higher max degree
 **/
DegreeArray *increase_array_size(DegreeArray *degree_array, int new_max_degree){
	int new_size = new_max_degree + 1;

	degree_array->array = realloc(degree_array->array, 
									new_size * sizeof(int));
	if (!degree_array->array){
		fprintf(stderr, "increase_array_size: realloc DegreeArray->array failed. \n");
		return NULL;
	}

	zero_out_new_space(degree_array, new_max_degree);
	degree_array->max_degree = new_max_degree;

	return degree_array;
}


/**
 * Set the space re-allocated to zero
 **/
void zero_out_new_space(DegreeArray *degree_array, int new_max_degree){
	int *array = degree_array->array;
	int max_degree = degree_array->max_degree;

	int i;
	for (i = max_degree + 1; i <= new_max_degree; i ++){
		array[i] = 0;
	}
}


/**
 * Cleanup degree_array
 **/
void cleanup(DegreeArray *degree_array){
	free(degree_array->array);
	free(degree_array);
}


/**
 * Print the contents of degree_array to stdout
 **/
void output_degree_array(DegreeArray *degree_array){
	int i;
	for (i = 0; i <= degree_array->max_degree; i ++){
		printf("%d, %d\n", i, degree_array->array[i]);
	}
}



int main(int argc, char **argv){
	FILE *fp_read;
	char *file_name;
	int block_size, uid_flag;
	DegreeArray *degree_array;

	/* param check */
	if (argc != 4)
	{
		fprintf(stderr, 
			"Usage: distribution <file_name> <block_size> <column_id> \n");
		return (-1);
	}

	/* open text file for reading */
	file_name = argv[1];
	if ( !( fp_read = fopen(file_name, "rb") ) )
	{
		fprintf(stderr, "Could not open file \"%s\" for reading. \n", file_name);
		return (-1);
	}

	/* block_size */
	if ( (block_size = get_size_byte(argv[2])) < 0 )
	{
		fprintf(stderr, "get block_size\n");
		return (-1);
	}

	/* column_id */
	if ( (uid_flag = get_uid_flag(argv[3])) < 0 ){
		fprintf(stderr, "get uid_flag\n");
		return (-1);
	}


	if ( !(degree_array = init_degree_array()) ){
		fprintf(stderr, "init_degree_array\n");
		return (-1);
	}

	degree_array = count_degree(fp_read, block_size, uid_flag, degree_array);
	if (!degree_array){
		fprintf(stderr, "count_degree\n");
		return (-1);
	}

	output_degree_array(degree_array);
	cleanup(degree_array);

	fclose(fp_read);

	return 0;
}