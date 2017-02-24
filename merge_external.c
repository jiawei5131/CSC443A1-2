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
	int input_buffer_capacity = manager->input_buffer_capacity;

	int chunk_id;
	for (chunk_id = 0; chunk_id < K; chunk_id ++){
		if ( !(manager->inputFP = get_read_fp(chunk_id)) ){
			return FAILURE;
		}
	}

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
	/*Takes next element in input buffers and insert into heap*/
	
	//Checks if input_buffer[file_number] is empty
	if(manager->total_input_buffer_elements[file_number] == 0){
		//load from file and update record-keeping variables
		refill_buffer(manager, file_number);
	}
	
	//store input element in result
	result = &(manager->input_buffers[file_number][manager->current_input_buffer_positions[file_number]]);
	Record* input = (Record*) malloc(sizeof(Record));
	input->uid1 = result->uid1;
	input->uid2 = result->uid2;
	//insert copy into heap
	insert_into_heap(manager, file_number, input);
	//update record-keeping variables
	manager->current_input_buffer_positions[file_number] += 1;
	manager->total_input_buffer_elements[file_number] -= 1;
	free(result);
	
	return SUCCESS;
}



int refill_buffer (MergeManager * manager, int file_number) {
	/*fill input buffer with records from given file and file position.*/
	char input_file_name[MAX_PATH_LENGTH];
	int num_rec_read = 0;
	manager->current_input_buffer_positions[file_number] = 0;
	manager->inputFP = get_read_fp(file_number);
	
	//set file pointer to the correct position
	fseek(manager->inputFP, manager->current_input_file_positions[file_number]*sizeof(Record), SEEK_SET);
	if ((num_rec_read = fread(manager->input_buffers[file_number], sizeof(Record), manager->input_buffer_capacity, manager->inputFP))==0){
			fprintf(stderr, "Reading from file failed \n");
			return FAILURE;
	}
	
	//update position in the file
	manager->current_input_file_positions[file_number] += ftell(manager->inputFP);
	fclose(manager->inputFP);
	manager->total_input_buffer_elements[file_number] = num_rec_read;
	return SUCCESS;
}



void clean_up (MergeManager * merger) {
	//not sure if I did this part right
	free(merger->inputFP);
	fclose(merger->outputFP);
	free(merger->outputFP);
	free(merger->input_buffers);
	free(merger->output_buffer);
	free(merger->input_file_numbers);
	free(merger->current_input_file_positions);
	free(merger->current_input_buffer_positions);
	free(merger->total_input_buffer_elements);
	free(merger->heap);
}



int compare_heap_elements (HeapElement *a, HeapElement *b) {
	return 0;
}

int main(int argc, char* argv[]){return 0;}