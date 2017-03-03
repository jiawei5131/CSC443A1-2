# CSC443-1

Intruction:
	This project includs two programs, namely, disk_sort and distribution, implemented for CSC443 Assignment 1.2 .


Usage:
	To compile both programs:
		* Run command-line:
			make

	To clean up *.o files:
		* Run command-lin:
			make clean


disk_sort takes in an UID1-sorted binary file and output an UID2-sorted binary file in the folder dataset. The program will generate multiple "phase1" binary files if needed in the folder dataset.

disk_sort:
	<input filename>: A binary file generated from edges.csv - sorted by UID1.
	<memory Size>: Memroy size could be allocated for sorting. The input argument should be a number followed by "MB" or "KB" or "B", for example "200MB".
	<block size>: Block size that the program allowed to use when I/O. The input argument should be a number followed by "MB" or "KB" or "B", for example "1024KB".


distribution takes in an either UID1-sorted or UID2-sorted binary file, and output the number of users for each out_degree or in_degree, respectively.
** NOTE: To avoid the results being printed in stdout, please use command-line:
	distribution <file_name> <block_size> <column_id> >> degree.csv

distribution:
	<file_name>: A binary file that is either sorted by UID1 or UID2
	<block_size>: Block size that the program allowed to use when I/O. The input argument should be a number followed by "MB" or "KB" or "B", for example "1024KB".
	<column_id>: UID1 or UID2 according to which of in_degree or out_degree distribution is interested.


To generate a graph or both in_degree or out_degree distributions, please run the python scrip named "plot_result.py" in command-line:
	python3 plot_result.py