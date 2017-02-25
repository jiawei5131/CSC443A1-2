#include "sortrun.h"
#include "utils.h"

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
	for (block_count = 1; block_count <= nblocks; block_count ++){
		int num_rec_read=0, num_rec_to_read=0;

		/* read a block of records */
		if (block_count == nblocks && num_rec_last_block != 0){
			/* last block */
			num_rec_to_read = num_rec_last_block;
			
		}else{
			/* other blocks */
			num_rec_to_read = num_rec_per_block;
		}

		num_rec_read = fread(buffer, sizeof(Record), num_rec_to_read, fp_read);
		
		/* reset walker */
		buf_walker = buffer;
		if ( num_rec_read != num_rec_to_read && !feof(fp_read) ){
			fprintf(stderr, "Reading records failed. \n");
			return -1;
		}

		int i;
		for(i = 0; i < num_rec_read; i ++){
			cur_rec->uid1 = buf_walker->uid1;
			cur_rec->uid2 = buf_walker->uid2;

			/* walk */
			cur_rec++;
			buf_walker++;
			read_count++;
		}

		/* end of file */
		if ( feof(fp_read) ){
			break;
		}
	}

	free(buffer);

	return read_count;
}


/**
 * Sort the binary file pointed by fp_read in memory, and write each
 * chunk into a separate file called phase1_n where n is the chunk ID
 *
 * Return number of chunks sorted
 **/
int sortrun(FILE* fp_read, int mem_size, int block_size){
	FILE* fp_write;
	Record *chunk_buffer;
	long file_size;

	/* calculate the chunk size */
	file_size = get_file_size(fp_read);
	int chunk_size = mem_size;
	int num_chunk = ceil_div(file_size, chunk_size);

	/* malloc chunk */
	chunk_buffer = malloc(chunk_size);
	if (!chunk_buffer){
		fprintf(stderr, "malloc buffer failed. \n");
		return -1;
	}	

	int i, num_rec_read, chunk_id;
	for(i = 0, chunk_id = 0; i < num_chunk; i ++, chunk_id ++){
		/* open phase1_[chunk_id] for writing */
		fp_write = get_write_fp(chunk_id);
		if (!fp_write){
			return -1;
		}

		/* read a chunk of records into buffer */
		num_rec_read = read_rec_chunk(fp_read, block_size, chunk_size, chunk_buffer);
		if (num_rec_read <= 0){
			return -1;
		}

		/* sort and write */
		qsort (chunk_buffer, num_rec_read, sizeof(Record), compare);
		fwrite (chunk_buffer, sizeof(Record), num_rec_read, fp_write);
		fflush (fp_write);

		/* close */
		fclose(fp_write);
	}

	/* clean up */
	free(chunk_buffer);
	fclose(fp_read);
	
  	return (chunk_id - 1);
}