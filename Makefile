CC = gcc

CFLAGS = -g -Wall

all: disk_sort

%.o: %.c %.h
	$(CC) -c $(CFLAGS) -o $@ $<

disk_sort: disk_sort.o utils.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o 

clean-ex:
	find . -maxdepth 1 -type f -perm -111 -delete
