#include "disk_sort.h"

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

/**
 * Read a chunk of records from file given chunk_size 
 * 
 * Return: a pointer of memory that allocated for a chunk 
 */
Record* read_rec_chunk(FILE* fp_read, int block_size, int chunk_size)
{
	Record *buffer, *records_chunk, *curr_record;
	int nblocks = (int)chunk_size/block_size;
	int nrecs = block_size / sizeof(Record);

	if ( !( records_chunk = malloc(chunk_size) ) ){
		fprintf(stderr, "malloc buffer failed. \n");
		return NULL;
	}
	curr_record = records_chunk;

	if ( !( buffer = malloc(block_size) ) ){
		fprintf(stderr, "malloc buffer failed. \n");
		return NULL;
	}

	int block_count = 0;
	while( (block_count <= nblocks) 
		&& fread(buffer, sizeof(Record), nrecs, fp_read) )
	{
		/* last block */
		if((block_count == nblocks) && (chunk_size % block_size != 0)){
			nrecs = (chunk_size % block_size) / sizeof(Record);
		}

		int i;
		for(i = 0; i < nrecs; i ++){
			curr_record->uid1 = buffer[i].uid1;
			curr_record->uid2 = buffer[i].uid2;
			curr_record++;
		}
		
		block_count++;
	}	
	free(buffer);

	return records_chunk;
}


int main(int argc, char* argv[])
{
	FILE* fp_read;
	FILE* fp_write;
	Record *chunk_buffer;
	char *file_name = 0;
	int block_size = 0;
	int mem_size = 0;
	int file_size;
	int nrec = 0;
	int chunk_size = 0;
	
	if(argc != 4){
		fprintf(stderr, "Insufficient Arguments : disk_sort <input filename> <memory Size> <block size>.\n");
		return (-1);
	}

	/* param checks */
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
	nrec = ceil( (float)chunk_size/sizeof(Record) );

	//block_num = number of blocks available in memory
	// fpos_t filepos[k];
	// Record** sorting_buf = malloc(sizeof(Record*) * k); 
	
	int i;
	for(i = 0; i < k; i ++){
		/* implicit malloc */
		chunk_buffer = read_rec_chunk(fp_read, block_size, chunk_size);

		if (!chunk_buffer)
		{
			return -1;
		}

		/* sort and write */
		qsort (chunk_buffer, nrec, sizeof(Record), compare);

		print_records(chunk_buffer, nrec); // test

		fwrite (chunk_buffer, sizeof(Record), nrec, fp_write);
		fflush (fp_write);

		/* free */
		free(chunk_buffer);
	}


	/*
	//----------Phase 2----------------
	for(i = 0; i<k; k++){ //initializes file position variables for each chunk
		fseek(fp_write, i*chunk_size, SEEK_SET);
		fgetpos(fp_write, &filepos[i])	
	}
	fseek(fp_write, 0, SEEK_SET); */
	
	//free(buffer);
	fclose(fp_read);
	fclose(fp_write);
	
  	return 0;
}