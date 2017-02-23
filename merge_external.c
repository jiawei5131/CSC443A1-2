#include "merge.h"
#include <math.h>

//manager fields should be already initialized in the caller
int merge_runs (MergeManager * merger, int K){	
	int  result; //stores SUCCESS/FAILURE returned at the end	
	
	//1. go in the loop through all input files and fill-in initial buffers
	if (init_merge (merger, K)!=SUCCESS)
		return FAILURE;

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

int init_merge (MergeManager * manager, int K) {
	//manager->heap=malloc(sizeof(HeapElement *)*K*2);
	//*manager->inputFP = 
	//long file_size = get_file_size(manager->inputFP);
	//int chunk_size = floor((float)file_size/K);
	//int last_chunk_size = file_size%chunk_size;
	manager->inputFP = fopen("dataset/phase1.dat", "w"); //all record chunks from phase 1 stored in phase1.dat
	manager->outputFP = fopen("dataset/phase2.dat", "w"); //open output file
	manager->output_buffer_capacity = 2;
	manager->output_buffer = malloc(sizeof(Record)*manager->output_buffer_capacity); //output buffer has capacity for 2 records
	manager->current_output_buffer_position = 0; 
	manager->input_buffers = malloc(K*sizeof(Record*));
	manager->current_input_buffer_positions = malloc(K*sizeof(int)); //all buffer positions point within the same file
	manager->total_input_buffer_elements = malloc(K*sizeof(int));
	manager->input_buffer_capacity = 2;
	manager->current_input_file_positions =  malloc(K*sizeof(int));
	
	int i; //initialize arrays
	for(i=0; i<K; i++){
		manager->input_buffers = malloc(2*sizeof(Record));
		manager->current_input_buffer_positions[i]=0;
		manager->total_input_buffer_elements[i] = 2;
		manager->current_input_file_positions[i] =  K*i;
	}

	manager->current_heap_size = K*2*sizeof(Record);
	manager->heap_capacity = K*2;	
	return SUCCESS;
}

int flush_output_buffer (MergeManager * manager) {
	/* flush the buffer to output file */
	int position = manager->current_output_buffer_position;
	int capacity = manager->output_buffer_capacity;
	Record* buffer= manager->output_buffer;
	FILE* outputFP = manager->outputFP;

	if (position == capacity){
		if(fwrite(buffer, sizeof(Record), capacity, outputFP) == 0){
				fprintf(stderr, "writing to output failed \n");
			return FAILURE;
		}
	}

	//reset output buffer after flush
	manager->current_output_buffer_position = 0;

	return SUCCESS;
}

int get_next_input_element(MergeManager * manager, int file_number, Record *result) {
	int K = manager->heap_capacity;
	int i;
	int input_buffer_position;
	for (i = 0; i<K; i++){
		if(manager->input_buffer_capacity > manager->current_input_buffer_positions[i]){
			input_buffer_position = manager->current_input_buffer_positions[i];
			break;
		}
	}
	//if there are no more records in the input buffer, we read from file
	if((i == (K-1)) & (manager->input_buffer_capacity == manager->current_input_buffer_positions[i])){
		refill_buffer(manager, file_number);	
	}else{
		//insert record at run_id = i and position = input_buffer_position into heap
		result = &(manager->input_buffers[i][input_buffer_position]);
		Record* input = (Record*) malloc(sizeof(Record));
		input->uid1 = result->uid1;
		input->uid2 = result->uid2;
		insert_into_heap(manager, i, input);
		manager->current_input_buffer_positions[i] += 1;
		//free(&manager->input_buffers[i][input_buffer_position]);
		//manager->input_buffers[i][input_buffer_position] =(Record) malloc(sizeof(Record));
	}
	return SUCCESS;
}

int refill_buffer (MergeManager * manager, int file_number) {
	return SUCCESS;
}

void clean_up (MergeManager * merger) {
	
}

int compare_heap_elements (HeapElement *a, HeapElement *b) {
	return 0;
}
int main(int argc, char* argv[]){return 0;}