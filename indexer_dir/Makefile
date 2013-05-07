CC = gcc
CFLAGS = -Wall -pedantic -std=c99
CFLAGS1 = -Wall 

EXEC = indexer
LDFLAGS = 
TEST = ./BATS.sh

# my project details
#OBJS = crawler.o hash.o html.o
#SRCS = crawler.c hash.c html.c crawler.h hash.h html.h

OBJS = indexer.o hash.o file.o 

SRCS = indexer.c hash.c indexer.h hash.h
CFLAGS1SRCS = file.c # need diff flags

# Commands start with TAB not spaces
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
	$(TEST) # test the indexer once made
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
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./indexer ../crawler_dir/data index.dat

clean:
	rm -f *~
	rm -f *#
	rm -f ./prs
	rm -f *.o
	rm -f *.gch
	rm -f core.*

cleanlog:
	rm -f *log.*
	rm -f index.dat


