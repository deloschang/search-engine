# This is the makefile for the query engine

CC = gcc
CFLAGS = -Wall -pedantic -std=c99
CFLAGS1 = -Wall 

EXEC = queryengine
LDFLAGS = 

# my project details
#OBJS = indexer.o hash.o file.o 
OBJS = queryengine.o index.o hash.o

#SRCS = indexer.c hash.c indexer.h hash.h file.h
SRCS = queryengine.c queryengine.h ../utils/index.c ../utils/index.h ../utils/hash.c  ../utils/hash.h
#CFLAGS1SRCS = file.c # need diff flags

# Commands start with TAB not spaces
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
$(OBJS): $(SRCS) $(CFLAGS1SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)
	#$(CC) $(CFLAGS1) -c $(CFLAGS1SRCS)
debug: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	#$(CC) $(CFLAGS1) -c $(CFLAGS1SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)
	gdb --args queryengine ../indexer_dir/index.dat ../crawler_dir/data
debug2: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	#$(CC) $(CFLAGS1) -c $(CFLAGS1SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)
	#gdb --args indexer ../crawler_dir/data/ index.dat index.dat index_new.dat

valgrind: $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
	#valgrind --tool=memcheck --leak-check=full --track-origins=yes ./queryengine ../indexer_dir/index.dat ../crawler_dir/data
	valgrind --tool=memcheck --leak-check=full ./queryengine ../indexer_dir/index.dat ../crawler_dir/data

clean:
	rm -f *~
	rm -f *#
	rm -f ./prs
	rm -f *.o
	rm -f *.gch
	rm -f core.*

cleanlog:
	rm -f *log.*


