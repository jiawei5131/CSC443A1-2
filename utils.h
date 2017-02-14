#define KB 1024
#define MB 1024 * 1024

typedef struct record  {
 	int uid1;
	int uid2;
} Record;

typedef struct small_rec_buf {
	Record* rec;
	int flags;
	fpos_t cur_pos;
	fpos_t end_pos;
} Small_rec_buf;

/* Process the second input argument */
int get_size_byte(char*);

/* print Records in the array */
void print_records(Record*, int);

/* Get size of file */
int get_file_size(FILE*);

/*  */
int load_into_mem(fpos_t*, int, FILE*, Record**);