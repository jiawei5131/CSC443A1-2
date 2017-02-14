#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/**
 * Helper function 
 *
 * Process the second input argument
 */
int get_size_byte(char *param)
{
	int num;
	int size_byte = 0;
	char* unit;
	/* number */
	if ( (num = atoi(param)) <= 0 )
	{
		fprintf(stderr, "<block size>: out of range. \n");
		return (-1);
	}
	unit = param + (strlen(param) - 2);
	/* MB or KB*/
	if (strcmp(unit, "MB") == 0)
	{
		size_byte = num * MB;
	}
	else if (strcmp(unit, "KB") == 0)
	{
		size_byte = num * KB;
	}else	
	{
		fprintf(stderr, "<block size>: unit must be 'MB' or 'KB'. \n");
		return (-1);
	}
	if (size_byte % sizeof(Record) != 0)
	{	
		fprintf(stderr, 
			"<block size>: must be a multiple of size of Record(8 Bytes). \n");
		return (-1);
	}
	return size_byte;
}

/**
 * Helper function
 *
 * Print records from the array
 **/
void print_records(Record* records, int n)
{
	int i;
	
	for (i = 0; i < n; i++)
	{
		printf("%d,%d\n",records->uid1, records->uid2);
		records++;
	}
}

// Get size of file
int get_file_size(FILE* file) 
{
	fseek(file, 0L, SEEK_END);
	int filesize = ftell(file);
	rewind(file);
	return filesize;
}