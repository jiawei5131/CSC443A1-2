#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include "utils.c"

/**
* Compares two records a and b 
* with respect to the value of the integer field f.
* Returns an integer which indicates relative order: 
* positive: record a > record b
* negative: record a < record b
* zero: equal records
*/
int compare (const void *a, const void *b) {
 int a_f = ((const struct record*)a)->uid2;
 int b_f = ((const struct record*)b)->uid2;
 return (a_f - b_f);
}
//Return a list of records in the given file
Record* read_blocks_routine(FILE* fp_read, int block_size){
	int total_rec_in_RAM;
	Record *buffer = NULL;
	/* get the size of file */
	if ( fseek(fp_read, 0L, SEEK_END) != 0 ){
		fprintf(stderr, "Seek end of file failed. \n");
	}
	long len_file = ftell(fp_read);
	/* calculate number of record in total */
	if ( len_file == -1L ){
		fprintf(stderr, "ftell \n");
	}else{
		/* ceiling */
		total_rec_in_RAM = 1 + (len_file - 1) / sizeof(Record);
	}
	/* malloc buffer */
	if ( (buffer = calloc(total_rec_in_RAM , sizeof(Record))) == NULL ){
		fprintf(stderr, "Calloc buffer failed. \n");
	}
	/* back to start */
	if ( fseek(fp_read, 0L, SEEK_SET) != 0 ){
		fprintf(stderr, "Seek start of file failed. \n");
	}
	/* read into RAM */
	int num_rec_read = fread(buffer, sizeof(Record), total_rec_in_RAM, fp_read);
	if ( num_rec_read != total_rec_in_RAM && !feof(fp_read) )
	{
		fprintf(stderr, "Reading records failed. \n");
	} 

	return buffer;
}

int main(int argc, char* argv[]){
	FILE* fp_read;
	Record *buffer;
	char *file_name;
	int block_size;

	if(argc != 3){
		fprintf(stderr,"Insufficient Arguments : write_blocks_seq <input filename> <block size>.\n");
		return (-1);
	}
	//Checking if arguments are valid
	file_name = argv[1]; // TODO check if it ends with .csv
	block_size = atoi(argv[2]);
	if(block_size == 0){
		fprintf(stderr,"<block size> is required to be an integer greater than one.\n");
		return (-1);
	}
	if ( !( fp_read = fopen(file_name, "rb") ) ){
		fprintf(stderr, "Could not open file \"%s\" for reading. \n", argv[1]);
		return (-1);
	}

	buffer = read_blocks_routine(fp_read, block_size);
	qsort (buffer, 10, sizeof(Record), compare);
	print_records(buffer,10);		
  	return 0;
}