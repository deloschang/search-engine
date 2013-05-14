# This is the makefile for the query engine

CC = gcc
CFLAGS = -Wall -pedantic -std=c99
CFLAGS1 = -Wall 

EXEC = queryengine

LDFLAGS = 

# query engine details
OBJS = queryengine.o index.o hash.o querylogic.o
SRCS = queryengine.c queryengine.h ../utils/index.c ../utils/hash.c  ../utils/hash.h querylogic.c 

# query engine unit test details
EXEC2 = queryengine_test
OBJS2 = queryengine_test.o index.o querylogic.o
SRCS2 = queryengine_test.c ../utils/index.c ../utils/index.h querylogic.c 

# Commands start with TAB not spaces
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
$(OBJS): $(SRCS) $(CFLAGS1SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)
unit: $(SRCS2) $(CFLAGS1SRCS2)
	$(CC) $(CFLAGS) -c $(SRCS2)
	$(CC) $(CFLAGS) -o $(EXEC2) $(OBJS2)
debug: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	#$(CC) $(CFLAGS1) -c $(CFLAGS1SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)
	gdb --args queryengine ../indexer_dir/index.dat ../crawler_dir/data
	#gdb --args queryengine index.dat ../crawler_dir/data
debug2: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	#$(CC) $(CFLAGS1) -c $(CFLAGS1SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)
	#gdb --args indexer ../crawler_dir/data/ index.dat index.dat index_new.dat

valgrind: $(OBJS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)
	#valgrind --tool=memcheck --leak-check=full --track-origins=yes ./queryengine ../indexer_dir/index.dat ../crawler_dir/data
	valgrind --tool=memcheck --leak-check=full ./queryengine ../indexer_dir/index.dat ../crawler_dir/data

valgrindgdb: $(OBJS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)
	valgrind --tool=memcheck --leak-check=full --vgdb-error=0 ./queryengine ../indexer_dir/index.dat ../crawler_dir/data



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


