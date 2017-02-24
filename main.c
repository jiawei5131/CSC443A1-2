#include "utils.h"
#include "merge.h"
#include "sortrun.h"

char *write_file_path = "dataset/phase_2.dat";

/**
 * Initialize a MergeManager merger for Phase II
 * Return SUCCESS or FAILURE
 **/
int init_MergeManager(MergeManager *merger, 
								int total_sort_runs, int mem_size, int block_size){
	/* K */
	int K = total_sort_runs;
	Record **input_buffers;

	/* size of each buffer - aligned with block_size */
	int buf_size = get_buf_size(mem_size, block_size, K);
	if (buf_size <= 0){
		fprintf(stderr, "buffer size\n");
		return FAILURE;
	}

	/* check if the bookeeping structure has enough space */
	if (!has_enough_mem(mem_size, buf_size, K)){
		fprintf(stderr, "Not enough memory for Phase II\n");
		return FAILURE;
	}
	
	/* calloc - heap */
	if ( !(merger->heap = calloc(K, sizeof(HeapElement))) ){
		fprintf(stderr, "calloc \n");
		return FAILURE;
	}
	merger->current_heap_size = K;
	merger->heap_capacity = K;

	/* input_file_numbers */
	if ( !(merger->input_file_numbers = calloc(K, sizeof(int))) ){
		fprintf(stderr, "calloc \n");
		return FAILURE;
	}

	/* outputFP */
	if ( !(merger->outputFP = fopen(write_file_path, "wb")) ){
		fprintf(stderr, "Could not open file %s for writing", write_file_path);
		return FAILURE;
	}

	/* output_buffer */
	if ( !(merger->output_buffer = calloc(1, buf_size)) ){
		fprintf(stderr, "calloc \n");
		return FAILURE;
	}

	merger->current_output_buffer_position = 0;
	merger->output_buffer_capacity = buf_size;

	/* input_buffers */
	merger->input_buffers = calloc(K, sizeof(Record*));
	input_buffers = merger->input_buffers;

	int i;
	for (i = 0; i < K; i ++){
		if (! ( input_buffers[i] = calloc(1, buf_size)) ){
			fprintf(stderr, "calloc \n");
			return FAILURE;
		}
	}

	merger->current_input_file_positions = calloc(K, sizeof(int));
	if (!merger->current_input_file_positions){
		fprintf(stderr, "calloc \n");
		return FAILURE;
	}

	merger->current_input_buffer_positions = calloc(K, sizeof(int));
	if (!merger->current_input_buffer_positions){
		fprintf(stderr, "calloc \n");
		return FAILURE;
	}

	merger->total_input_buffer_elements = calloc(K, sizeof(int));
	if (!merger->total_input_buffer_elements){
		fprintf(stderr, "calloc \n");
		return FAILURE;
	}

	/* file names and input prefix */
	strcpy(merger->output_file_name, write_file_path);
	strcpy(merger->input_prefix, INPUT_PREFIX);

	return SUCCESS;
}


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
	
	/* param checks */
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
	if (init_MergeManager(merger, total_sort_runs, mem_size, block_size) == FAILURE){
		return FAILURE;
	}
	
	return merge_runs(merger);
}