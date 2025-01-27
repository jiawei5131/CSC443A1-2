#include "utils.h"

/**
 * Helper function 
 *
 * Process the second input argument
 */
int get_size_byte(char *param){
	int num = atoi(param);
	int size_byte = 0;
	char* unit;
	/* number */
	if ( num <= 0 ){
		fprintf(stderr, "<block size>: out of range. \n");
		return (-1);
	}

	unit = param + (strlen(param) - 2);
	/* B or MB or KB*/
	if (strcmp(unit + 1, "B") == 0){
		if (strcmp(unit, "MB") == 0){
			size_byte = num * MB;
		}else if (strcmp(unit, "KB") == 0){
			size_byte = num * KB;
		}
		else{	// unit B
			size_byte = num;
		}
	}else{
		fprintf(stderr, "<block size / mem size>: unit must be 'MB' or 'KB' or 'B'. \n");
		return (-1);
	}

	if (size_byte % sizeof(Record) != 0){	
		fprintf(stderr, 
			"<block size / mem size>: must be a multiple of size of Record(8 Bytes). \n");
		return (-1);
	}

	return size_byte;
}


/**
 * Helper function
 *
 * Print records from the array
 **/
void print_records(Record* records, int n){
	int i;
	
	for (i = 0; i < n; i++){
		printf("%d,%d\n",records->uid1, records->uid2);
		records++;
	}
}


/** 
 * Get size of file in bytes
 **/
long get_file_size(FILE* file){
	long filesize;

	if ( fseek(file, 0L, SEEK_END) != 0 ){
		fprintf(stderr, "Seek end of file failed. \n");
		return (-1);
	}

	filesize = ftell(file);
	rewind(file);

	return filesize;
}


/** 
 * Get remaining size of file in bytes
 **/
long get_remain_file_size(FILE* file) {
	long position, filesize, remain_fsize;
	fflush(file);
	/* save position */
	position = ftell(file);

	if ( fseek(file, 0L, SEEK_END) != 0 ){
		fprintf(stderr, "Seek end of file failed. \n");
		return (-1);
	}

	filesize = ftell(file);
	remain_fsize = filesize - position;
	
	/* seek back */
	fseek(file, position, SEEK_SET);
	
	return remain_fsize;
}


/**
 * Count the number of digits of an int
 **/
int count_digits(int num){
	int n = num;
    int count = 0;

    while(n != 0){
        n /= 10;
        ++count;
    }
    
    return count;
}


/**
 * Convert int to str
 **/
char* int_to_string(int K){
	char* buf = malloc(sizeof(char) * count_digits(K));

	if (!buf){
		fprintf(stderr, 
			"malloc for converting int [%d] to str.\n", K);
	}

	sprintf(buf, "%d", K);
	return buf;
}


/**
 * Create a file called "phase1_[chunk_id]" for writing
 * 
 * Return a pointer for the file
 **/
FILE* get_write_fp(int chunk_id){
	FILE* fp_write;

	/* get file path */
	char write_file_path[1024] = INPUT_PREFIX;
	char* chunk_id_str = int_to_string(chunk_id);
	strcat(write_file_path, chunk_id_str);
	free(chunk_id_str);

	/* open for write */
	if ( !( fp_write = fopen(write_file_path , "wb") ) ){
		fprintf(stderr, "Could not open file \"%s\" for writing. \n", write_file_path);
		return NULL;
	}

	return fp_write;
}


/**
 * Open the file "phase1_[chunk_id]" for reading
 * 
 * Return a pointer for the file
 **/
FILE* get_read_fp(int chunk_id){
	FILE* fp_read;

	/* get file path */
	char read_file_path[1024] = INPUT_PREFIX;
	char* chunk_id_str = int_to_string(chunk_id);
	strcat(read_file_path, chunk_id_str);
	free(chunk_id_str);

	/* open for reading */
	if ( !( fp_read = fopen(read_file_path , "rb") ) ){
		fprintf(stderr, "Could not open file \"%s\" for reading. \n", read_file_path);
		return NULL;
	}

	return fp_read;
}


/**
 * Return ceilled result of the division
 *   result = numer / denom
 **/
int ceil_div(int numer, int denom){
	return ( 1 + ( numer - 1) / denom );
}


/**
* Compares two records a and b 
* with respect to the value of the integer field f.
* Returns an integer which indicates relative order: 
* positive: record a > record b
* negative: record a < record b
* zero: equal records
*/
int compare (const void *a, const void *b){
	int a_f = ((const struct record*)a)->uid2;
	int b_f = ((const struct record*)b)->uid2;
	return (a_f - b_f);
}