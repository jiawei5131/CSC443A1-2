#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <math.h>
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
Record* read_blocks_routine(FILE* fp_read, int block_size, int num_blocks){
	//int total_rec_in_RAM;
	Record *buffer = NULL;
	Record *records_chunk = malloc(block_size*num_blocks);
	Record *curr_record = records_chunk;
	int current_block = 0;
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
		//total_rec_in_RAM = 1 + (len_file - 1) / sizeof(Record);
	}
	/* malloc buffer */

	/* back to start */
	if ( fseek(fp_read, 0L, SEEK_SET) != 0 ){
		fprintf(stderr, "Seek start of file failed. \n");
	}
	/* read into RAM */
	if (!(buffer = malloc(block_size)) ){
		fprintf(stderr, "Calloc buffer failed. \n");
	}
	while(current_block < num_blocks && fread(buffer, sizeof(Record), floor(block_size/sizeof(Record)) , fp_read)){
		int i;
		for(i=0; i<floor(block_size/sizeof(Record)); i++){
			if((buffer[i].uid1==0) && (buffer[i].uid2==0)) break;
			curr_record->uid1 = buffer[i].uid1;
			curr_record->uid2 = buffer[i].uid2;
			curr_record++;
			
		}
		//printf("%d, %d\n",records_chunk->uid1, records_chunk->uid2);
		current_block++;
		free(buffer);
		
		if ( (buffer = calloc(floor(num_blocks*block_size/sizeof(Record)) , sizeof(Record))) == NULL ){
		fprintf(stderr, "Calloc buffer failed. \n");
		break;
		}
		
	}
	/*if ( num_rec_read != total_rec_in_RAM ){
		fprintf(stderr, "Reading records failed. \n");
	} */
	
	return records_chunk;
}

int main(int argc, char* argv[]){
	FILE* fp_read;
	Record *buffer;
	char *file_name;
	int block_size;
	int mem_size;
	//int k;

	if(argc != 4){
		fprintf(stderr,"Insufficient Arguments : write_blocks_seq <input filename> <memory Size> <block size>.\n");
		return (-1);
	}
	//Checking if arguments are valid
	file_name = argv[1]; // TODO check if it ends with .csv
	mem_size = atoi(argv[2]);
	block_size = atoi(argv[3]);	
	
	if((block_size == 0) | (mem_size == 0)){
		fprintf(stderr,"<block size> and <memory size> are required to be integers greater than one.\n");
		return (-1);
	}
	if ( !( fp_read = fopen(file_name, "rb") ) ){
		fprintf(stderr, "Could not open file \"trunc%s\" for reading. \n", argv[1]);
		return (-1);
	}

	buffer = read_blocks_routine(fp_read, block_size, 2);
	qsort (buffer, 27, sizeof(Record), compare);
	//printf("%d, %d\n",buffer->uid1, buffer->uid2);
	print_records(buffer,27);		
  	return 0;
}