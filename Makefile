CC = gcc

CFLAGS = -g -Wall

all: disk_sort distribution

merge.o: merge_external.c merge.h
	$(CC) -c $(CFLAGS) -o $@ $<

distribution.o: distribution.c utils.h
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.c %.h
	$(CC) -c $(CFLAGS) -o $@ $<

disk_sort: utils.o merge.o sortrun.o main.o
	$(CC) $(CFLAGS) -o $@ $^

distribution: distribution.o utils.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o 

clean-ex:
	find . -maxdepth 1 -type f -perm -111 -delete
