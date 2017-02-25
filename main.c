#include <stdio.h>
#include <stdlib.h>
#include "merge.h"
#include "sortrun.h"

char *output_fpath = "dataset/phase_2.dat";


int main (int argc, char **argv) {
	FILE *fp_read;
	char *file_name = 0;
	int block_size = 0;
	int mem_size = 0;
	int total_sort_runs, max_chunk_id;
	MergeManager *merger;

	/* command-line arguments check */
	if(argc != 4){
		fprintf(stderr, "Insufficient Arguments : disk_sort <input filename> <memory Size> <block size>.\n");
		return FAILURE;
	}

	file_name = argv[1];
	mem_size = get_size_byte(argv[2]);
	block_size = get_size_byte(argv[3]);	
	
	if ((block_size <= 0) || (mem_size <= 0)){
		fprintf(stderr,"<block size> or <memory size> are invalid.\n");
		return FAILURE;
	}
	if (block_size > mem_size){
		fprintf(stderr,"block size must not be greater than memory size.\n");
		return FAILURE;
	}

	/* open input binary file */
	if ( !( fp_read = fopen(file_name, "rb") ) ){
		fprintf(stderr, "Could not open file \"%s\" for reading. \n", file_name);
		return FAILURE;
	}

	/* Phase I - sort and write each run on disk */
	max_chunk_id = sortrun(fp_read, mem_size, block_size);
	if (max_chunk_id < 0){
		fprintf(stderr, "sortrun \n");
		return FAILURE;
	}

	/* Phase II - merge merger calloc */
	if ( !(merger = calloc(1, sizeof(MergeManager))) ){
		fprintf(stderr, "calloc \n");
		return FAILURE;
	}

	/* initialize */
	total_sort_runs = max_chunk_id + 1; // chunk_id starts from 0
	if (init_MergeManager(merger, total_sort_runs, mem_size, block_size, output_fpath) == FAILURE){
		return FAILURE;
	}
	
	return merge_runs(merger);
}