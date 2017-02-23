#include "merge.h"

char *write_file_path = "dataset/phase_2.dat";
int extra_mem = 5 * MB;

int main (int argc, char **argv) {
	//process and validate command-line arguments
	FILE *fp_read, *fp_write;
	Record *chunk_buffer;
	char *file_name = 0;
	int block_size = 0;
	int mem_size = 0;
	long file_size;
	MergeManager manager;

	/* command-line arguments check */
	if(argc != 4){
		fprintf(stderr, "Insufficient Arguments : merge <input filename> <memory Size> <block size>.\n");
		return FAILURE;
	}

	/* param checks */
	file_name = argv[1];
	mem_size = get_size_byte(argv[2]) + extra_mem;
	block_size = get_size_byte(argv[3]);	
	
	if ((block_size <= 0) || (mem_size <= 0))
	{
		fprintf(stderr,"<block size> or <memory size> are invalid.\n");
		return FAILURE;
	}
	if (block_size > mem_size)
	{
		fprintf(stderr,"block size must not be greater than memory size.\n");
		return FAILURE;
	}
	if ( !( fp_read = fopen(file_name, "rb") ) )
	{
		fprintf(stderr, "Could not open file \"trunc%s\" for reading. \n", file_name);
		return FAILURE;
	}
	if ( !( fp_write = fopen(write_file_path , "wb") ) )
	{
		fprintf(stderr, "Could not open file \"%s\" for writing. \n", write_file_path);
		return FAILURE;
	}

	/* Phase I */

	//initialize all fields according to the input and the results of Phase I


	return merge_runs (&manager);
}