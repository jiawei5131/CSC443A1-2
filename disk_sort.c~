#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <math.h>
#include "utils.h"

/**
* Compares two records a and b 
* with respect to the value of the integer field f.
* Returns an integer which indicates relative order: 
* positive: record a > record b
* negative: record a < record b
* zero: equal records
*/
int compare (const void *a, const void *b) 
{
	int a_f = ((const struct record*)a)->uid2;
	int b_f = ((const struct record*)b)->uid2;
	return (a_f - b_f);
}

//Read a chunk of records from file given chunk_size
Record* read_records(FILE* fp_read, int block_size, int chunk_size){
	// int total_rec_in_RAM;
	Record *buffer;
	Record *records_chunk = (Record*)malloc(chunk_size);
	Record *curr_record = records_chunk;
	int block_count = 0;
	int nblocks = (int)chunk_size/block_size;

	/* read into RAM */
	if (!(buffer = (Record*)malloc(block_size)) ){
		fprintf(stderr, "malloc buffer failed. \n");
	}

	//printf("%d\n", block_count);
	while((block_count <= nblocks) && fread(buffer, sizeof(Record), block_size/sizeof(Record), fp_read)){
		//printf("nrecs: %d\n", sizeof(Record));
		int nrecs = block_size/sizeof(Record);
		if((block_count == nblocks) && (chunk_size%block_size != 0)){
			nrecs = (chunk_size%block_size)/sizeof(Record);
		}
		int i;

		for(i=0; i<nrecs; i++){
			//if(((buffer[i].uid1==0) && (buffer[i].uid2==0))|| (count_rec >= num_rec)) break;
			curr_record->uid1 = buffer[i].uid1;
			curr_record->uid2 = buffer[i].uid2;
			curr_record++;
		}
		//printf("%d, %d\n",buffer->uid1, buffer->uid2);
		block_count++;
	}	
	free(buffer);
	return records_chunk;
}


int main(int argc, char* argv[])
{
	FILE* fp_read;
	FILE* fp_write;
	Record *buffer;
	char *file_name = 0;
	int block_size = 0;
	int mem_size = 0;
	int file_size;
	int nrec = 0;
	int chunk_size = 0;
	
	if(argc != 4){
		fprintf(stderr,"Insufficient Arguments : write_blocks_seq <input filename> <memory Size> <block size>.\n");
		return (-1);
	}

	//Checking if arguments are valid
	file_name = argv[1];
	mem_size = atoi(argv[2]);
	block_size = atoi(argv[3]);	
	
	if((block_size == 0) | (mem_size == 0)){
		fprintf(stderr,"<block size> and <memory size> are required to be integers greater than one.\n");
		return (-1);
	}
	if((block_size > mem_size)){
		fprintf(stderr,"block size must not be greater than memory size.\n");
		return (-1);
	}
	if ( !( fp_read = fopen(file_name, "rb") ) ){
		fprintf(stderr, "Could not open file \"trunc%s\" for reading. \n", argv[1]);
		return (-1);
	}
	if ( !( fp_write = fopen ( "sortedrecords.dat" , "wb" ) ) )
	{
		fprintf(stderr, "Could not open file \"%s\" for writing. \n", "sortedrecords.dat");
		return (-1);
	}

	file_size = get_file_size(fp_read);
	/* k = number of chunks file is split into */
	int k = ceil( (float) file_size / mem_size );	
	chunk_size = ceil((float) file_size / k);	
	nrec = ceil((float)chunk_size/sizeof(Record));
	//block_num = number of blocks available in memory
	buffer = (Record *)malloc(chunk_size);
	fpos_t filepos[k]
	Record sorting_buf[k][2];
	int i;
	//printf("nblocks: %x\n file: %d\n", num_blocks, file_size);
	for(i = 0; i < k; i ++){
		buffer = read_records(fp_read, block_size, chunk_size);
		qsort (buffer, nrec, sizeof(Record), compare);
		//print_records(buffer, nrec);
		fwrite (buffer, sizeof(Record), nrec, fp_write);
		fflush (fp_write);
		//write to sortedrecords.dat		
	}	
	//----------Phase 2----------------
	for(i = 0; i<k; k++){ //initializes file position variables for each chunk
		fseek(fp_write, i*chunk_size, SEEK_SET);
		fgetpos(fp_write, &filepos[i])	
	}
	
	//free(buffer);
	fclose(fp_read);
	fclose(fp_write);
	
  	return 0;
}