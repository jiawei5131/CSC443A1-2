#include <math.h>
#include "merge.h"


//manager fields should be already initialized in the caller
int merge_runs (MergeManager * merger){	
	int  result; //stores SUCCESS/FAILURE returned at the end	
	
	//1. go in the loop through all input files and fill-in initial buffers
	if (init_merge (merger) != SUCCESS){
		return FAILURE;
	}

	while (merger->current_heap_size > 0) { //heap is not empty
		HeapElement smallest;
		Record next; //here next comes from input buffer
		
		if(get_top_heap_element (merger, &smallest)!=SUCCESS)
			return FAILURE;

		result = get_next_input_element (merger, smallest.run_id, &next);
		
		if (result==FAILURE)
			return FAILURE;

		if(result==SUCCESS) {//next element exists, may also return EMPTY
			if(insert_into_heap (merger, smallest.run_id, &next)!=SUCCESS)
				return FAILURE;
		}		


		merger->output_buffer [merger->current_output_buffer_position].uid1=smallest.UID1;
		merger->output_buffer [merger->current_output_buffer_position].uid2=smallest.UID2;
		
		merger->current_output_buffer_position++;

        //staying on the last slot of the output buffer - next will cause overflow
		if(merger->current_output_buffer_position == merger-> output_buffer_capacity ) {
			if(flush_output_buffer(merger)!=SUCCESS) {
				return FAILURE;			
				merger->current_output_buffer_position=0;
			}	
		}
	
	}

	
	//flush what remains in output buffer
	if(merger->current_output_buffer_position > 0) {
		if(flush_output_buffer(merger)!=SUCCESS)
			return FAILURE;
	}
	
	clean_up(merger);
	return SUCCESS;	
}


int get_top_heap_element (MergeManager * merger, HeapElement * result){
	HeapElement item;
	int child, parent;

	if(merger->current_heap_size == 0){
		printf( "UNEXPECTED ERROR: popping top element from an empty heap\n");
		return FAILURE;
	}

	*result=merger->heap[0];  //to be returned

	//now we need to reorganize heap - keep the smallest on top
	item = merger->heap [--merger->current_heap_size]; // to be reinserted 

	parent =0;
	while ((child = (2 * parent) + 1) < merger->current_heap_size) {
		// if there are two children, compare them 
		if (child + 1 < merger->current_heap_size && 
				(compare_heap_elements(&(merger->heap[child]),&(merger->heap[child + 1]))>0)) 
			++child;
		
		// compare item with the larger 
		if (compare_heap_elements(&item, &(merger->heap[child]))>0) {
			merger->heap[parent] = merger->heap[child];
			parent = child;
		} 
		else 
			break;
	}
	merger->heap[parent] = item;
	
	return SUCCESS;
}

int insert_into_heap (MergeManager * merger, int run_id, Record *input){

	HeapElement new_heap_element;
	int child, parent;

	new_heap_element.UID1 = input->uid1;
	new_heap_element.UID2 = input->uid2;
	new_heap_element.run_id = run_id;
	
	if (merger->current_heap_size == merger->heap_capacity) {
		printf( "Unexpected ERROR: heap is full\n");
		return FAILURE;
	}
  	
	child = merger->current_heap_size++; /* the next available slot in the heap */
	
	while (child > 0) {
		parent = (child - 1) / 2;
		if (compare_heap_elements(&(merger->heap[parent]),&new_heap_element)>0) {
			merger->heap[child] = merger->heap[parent];
			child = parent;
		} 
		else 
			break;
	}
	merger->heap[child]= new_heap_element;	
	return SUCCESS;
}



/*
** TO IMPLEMENT
*/

int init_merge (MergeManager * manager) {
	/* read each run into its corresponding input_buffer */
	int K = manager->heap_capacity;
	Record head;
	int result;

	/* load each run */
	int chunk_id;
	for (chunk_id = 0; chunk_id < K; chunk_id ++){
		/* fill input_file_number */
		manager->input_file_numbers[chunk_id] = chunk_id;

		/* read records into chunk_id input_buffer */
		result = refill_buffer(manager, chunk_id);

		if (result == FAILURE || result == EMPTY){
			fprintf(stderr, "init_merge: refill buffer.\n");
			return FAILURE;
		}

		/* insert the head of input_buffer to heap */
		result = get_next_input_element (manager, chunk_id, &head);

		if (result == FAILURE){
			fprintf(stderr, "init_merge: get head input element.\n");
			return FAILURE;
		}

		if(result == SUCCESS) {
			if(insert_into_heap (manager, chunk_id, &head) == FAILURE){
				fprintf(stderr, "init_merge: insert head into heap.\n");
				return FAILURE;
			}
		}
	}

	return SUCCESS;
}



int flush_output_buffer (MergeManager * manager) {
	int position = manager->current_output_buffer_position;
	Record* buffer= manager->output_buffer;
	FILE* outputFP = manager->outputFP;

	if(fwrite(buffer, sizeof(Record), position, outputFP) == 0){
		fprintf(stderr, "writing to output failed \n");
		return FAILURE;
	}

	/* flush */
	fflush(outputFP);

	/* reset */
	manager->current_output_buffer_position = 0;

	return SUCCESS;
}



int get_next_input_element(MergeManager * manager, int file_number, Record *result) {

	//Checks if input_buffer[file_number] is empty
	if((manager->total_input_buffer_elements[file_number]-1) == manager->current_input_buffer_positions[file_number]){
		//load from file and update record-keeping variables
		int refill_result = refill_buffer(manager, file_number);

		if(refill_result == EMPTY){
			return EMPTY;	
		} else if (refill_result == FAILURE){
			fprintf(stderr, "get_next_input_element: refill_buffer \n");
			return FAILURE;
		}
	}
	
	//store input element in result
	*result = manager->input_buffers[file_number][manager->current_input_buffer_positions[file_number]];

	//update record-keeping variables
	manager->current_input_buffer_positions[file_number] ++;
	
	return SUCCESS;
}



int refill_buffer (MergeManager * manager, int file_number) {
	int num_rec_read;
	int capacity = manager->input_buffer_capacity;
	int position = manager->current_input_file_positions[file_number];

	/* input file exhausted */
	if (position == -1){
		return EMPTY;
	}

	manager->inputFP = get_read_fp(file_number);
	if (!(manager->inputFP)){
		return FAILURE;
	}
	
	/* seek the right position to read */
	fseek(manager->inputFP, position, SEEK_SET);
	num_rec_read = fread(manager->input_buffers[file_number], sizeof(Record), capacity, manager->inputFP);
	if (num_rec_read != capacity && !feof(manager->inputFP)){
			fprintf(stderr, "refill_buffer: Reading from file failed \n");
			return FAILURE;
	}
	
	//update position in the file
	manager->current_input_file_positions[file_number] = ftell(manager->inputFP);
	if (feof(manager->inputFP)){
		manager->current_input_file_positions[file_number] = -1;
	}
	fclose(manager->inputFP);

	manager->current_input_buffer_positions[file_number] = 0;
	manager->total_input_buffer_elements[file_number] = num_rec_read;

	return SUCCESS;
}




void clean_up (MergeManager * merger) {
	free(merger->heap);
	free(merger->input_file_numbers);
	fclose(merger->outputFP);
	free(merger->output_buffer);

	/* free all input buffers */
	int i;
	for (i = 0; i < merger->heap_capacity; i ++){
		free(merger->input_buffers[i]);	
	}
	free(merger->input_buffers);
	
	free(merger->current_input_file_positions);
	free(merger->current_input_buffer_positions);
	free(merger->total_input_buffer_elements);
	
	/* free MergeManager */
	free(merger);
}


/**
 * Return positive if b->UID2 is smaller than a->UID2
 **/
int compare_heap_elements (HeapElement *a, HeapElement *b) {
	int a_f = a->UID2;
	int b_f = b->UID2;
	return (a_f - b_f);
}


/*======================= Helper functions =======================*/

/**
 * Return the size of each buffer for phase II
 **/
int get_buf_size(int mem_size, int block_size, int K){
	/* k input buffer and 1 output buffer */
	int max_buf_size = mem_size / (K + 1);
	int nblock_per_buf = max_buf_size / block_size;

	if (nblock_per_buf <= 0){
		fprintf(stderr, "buffer cannot hold at least 1 block\n");
		return -1;
	}

	return nblock_per_buf * block_size;
}


/**
 * Check if the memsize is enough for the structure 
 **/
int has_enough_mem(int mem_size, int buf_size, int K){
	int frag_mem = mem_size - buf_size * (K + 1);
	int extra_mem = 5 * MB + frag_mem;

	int mem_str_require = sizeof(MergeManager)		// MergeManager
					+ K * sizeof(HeapElement)	// heap
					+ K * sizeof(int)		// input_file_numbers
					+ K * sizeof(int)		// current_input_file_positions
					+ K * sizeof(int)		// current_input_buffer_positions
					+ K * sizeof(int);		// total_input_buffer_elements

	return (mem_str_require <= extra_mem);
}


/**
 * Initialize a MergeManager merger for Phase II
 * Return SUCCESS or FAILURE
 **/
int init_MergeManager(MergeManager *merger, 
					  int total_sort_runs, 
					  int mem_size, 
					  int block_size,
					  char* output_fpath){
	/* K */
	int K = total_sort_runs;
	Record **input_buffers;
	int num_rec_per_buf, buf_size;

	/* size of each buffer - aligned with block_size */
	buf_size = get_buf_size(mem_size, block_size, K);
	if (buf_size <= 0){
		fprintf(stderr, "buffer size\n");
		return FAILURE;
	}
	
	num_rec_per_buf = buf_size / sizeof(Record);

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
	merger->current_heap_size = 0;
	merger->heap_capacity = K;

	/* input_file_numbers */
	if ( !(merger->input_file_numbers = calloc(K, sizeof(int))) ){
		fprintf(stderr, "calloc \n");
		return FAILURE;
	}

	/* outputFP */
	if ( !(merger->outputFP = fopen(output_fpath, "wb")) ){
		fprintf(stderr, "Could not open file %s for writing", output_fpath);
		return FAILURE;
	}

	/* output_buffer */
	if ( !(merger->output_buffer = calloc(num_rec_per_buf, sizeof(Record))) ){
		fprintf(stderr, "calloc \n");
		return FAILURE;
	}

	merger->current_output_buffer_position = 0;
	merger->output_buffer_capacity = num_rec_per_buf;

	/* input_buffers */
	merger->input_buffers = calloc(K, sizeof(Record*));
	input_buffers = merger->input_buffers;

	int chunk_id;
	for (chunk_id = 0; chunk_id < K; chunk_id ++){
		if (! ( input_buffers[chunk_id] = calloc(num_rec_per_buf, sizeof(Record))) ){
			fprintf(stderr, "calloc \n");
			return FAILURE;
		}
	}
	merger->input_buffer_capacity = num_rec_per_buf;

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
	strcpy(merger->output_file_name, output_fpath);
	strcpy(merger->input_prefix, INPUT_PREFIX);

	return SUCCESS;
}

/*======================= Helper Ends =======================*/