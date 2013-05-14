CC = gcc
CFLAGS = -Wall -pedantic -std=c99
CFLAGS1 = -Wall 

EXEC = indexer
LDFLAGS = 

# my project details
OBJS = indexer.o hash.o file.o index.o

SRCS = indexer.c ../utils/hash.c ../utils/hash.h ../utils/file.h ../utils/index.c
CFLAGS1SRCS = ../utils/file.c # need diff flags

# Commands start with TAB not spaces
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
$(OBJS): $(SRCS) $(CFLAGS1SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)
	$(CC) $(CFLAGS1) -c $(CFLAGS1SRCS)
debug: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS1) -c $(CFLAGS1SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)
	gdb --args indexer ../crawler_dir/data/ index.dat
debug2: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS1) -c $(CFLAGS1SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)
	gdb --args indexer ../crawler_dir/data/ index.dat index.dat index_new.dat

valgrind: $(OBJS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS1) -c $(CFLAGS1SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)
	valgrind --tool=memcheck --leak-check=full ./indexer ../crawler_dir/data index.dat

clean:
	rm -f *~
	rm -f *#
	rm -f ./prs
	rm -f *.o
	rm -f *.gch
	rm -f core.*
	rm -f vgcore.*

cleanlog:
	rm -f *log.*
	rm -f index.dat
	rm -f index_new.dat


