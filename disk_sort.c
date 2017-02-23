#include "disk_sort.h"

char *write_file_path = "dataset/sortedrecords.dat";

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
 * Return: number of records read
 */
int read_rec_chunk(FILE* fp_read, int block_size, int chunk_size, Record* chunk_buffer)
{
	Record *buffer, *cur_rec, *buf_walker;
	int nblocks, num_rec_per_block, num_rec_last_block;
	int read_count = 0;

	/* calculation */
	nblocks = ceil_div(chunk_size, block_size);
	num_rec_per_block = block_size / sizeof(Record);
	num_rec_last_block = (chunk_size / sizeof(Record)) % num_rec_per_block;

	/* temp buffer */
	buffer = malloc(num_rec_per_block * sizeof(Record));
	if (!buffer){
		fprintf(stderr, "malloc buffer failed. \n");
		return -1;
	}

	cur_rec = chunk_buffer;
	int block_count;
	for (block_count = 1; block_count <= nblocks; block_count ++)
	{
		int num_rec_read, num_rec_to_read;

		/* read a block of records */
		if (block_count == nblocks && num_rec_last_block != 0)
		{
			/* last block */
			num_rec_to_read = num_rec_last_block;
		}
		else
		{
			/* other blocks */
			num_rec_to_read = num_rec_per_block;
		}

		num_rec_read = fread(buffer, sizeof(Record), num_rec_to_read, fp_read);
		
		/* reset walker */
		buf_walker = buffer;
		if ( num_rec_read != num_rec_to_read && !feof(fp_read) )
		{
			fprintf(stderr, "Reading records failed. \n");
			return -1;
		}

		printf("rec read :%d\n", num_rec_read);

		int i;
		for(i = 0; i < num_rec_read; i ++){
			cur_rec->uid1 = buf_walker->uid1;
			cur_rec->uid2 = buf_walker->uid2;

			/* walk */
			cur_rec++;
			buf_walker++;
			read_count++;
		}

		if ( feof(fp_read) )
		{
			break;
		}
	}

	free(buffer);

	return read_count;
}


int main(int argc, char* argv[])
{
	FILE* fp_read;
	FILE* fp_write;
	Record *chunk_buffer;
	char *file_name = 0;
	int block_size = 0;
	int mem_size = 0;
	long file_size;
	
	if(argc != 4){
		fprintf(stderr, "Insufficient Arguments : disk_sort <input filename> <memory Size> <block size>.\n");
		return (-1);
	}

	/* param checks */
	file_name = argv[1];
	mem_size = get_size_byte(argv[2]);
	block_size = get_size_byte(argv[3]);	
	
	if ((block_size <= 0) || (mem_size <= 0))
	{
		fprintf(stderr,"<block size> or <memory size> are invalid.\n");
		return (-1);
	}
	if (block_size > mem_size)
	{
		fprintf(stderr,"block size must not be greater than memory size.\n");
		return (-1);
	}
	if ( !( fp_read = fopen(file_name, "rb") ) )
	{
		fprintf(stderr, "Could not open file \"trunc%s\" for reading. \n", file_name);
		return (-1);
	}
	if ( !( fp_write = fopen(write_file_path , "wb") ) )
	{
		fprintf(stderr, "Could not open file \"%s\" for writing. \n", write_file_path);
		return (-1);
	}

	/* calculate the chunk size */
	file_size = get_file_size(fp_read);
	int num_rec_per_chunk = mem_size / sizeof(Record); // floor division
	int chunk_size = num_rec_per_chunk * sizeof(Record);
	int num_chunk = ceil_div(file_size, chunk_size);

	/* malloc chunk */
	chunk_buffer = malloc(chunk_size);
	if (!chunk_buffer){
		fprintf(stderr, "malloc buffer failed. \n");
		return -1;
	}	
	
	int i, num_rec_read;
	for(i = 0; i < num_chunk; i ++){
		/* read a chunk of records into buffer */
		num_rec_read = read_rec_chunk(fp_read, block_size, chunk_size, chunk_buffer);

		if (num_rec_read <= 0)
		{
			return -1;
		}

		/* sort and write */
		qsort (chunk_buffer, num_rec_read, sizeof(Record), compare);

		print_records(chunk_buffer, num_rec_read); // test

		fwrite (chunk_buffer, sizeof(Record), num_rec_read, fp_write);
		fflush (fp_write);	
	}

	/* clean up */
	free(chunk_buffer);
	fclose(fp_read);
	fclose(fp_write);
	
  	return 0;
}