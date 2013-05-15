# This is the makefile for the query engine

CC = gcc
CFLAGS = -Wall -pedantic -std=c99
CFLAGS1 = -Wall 

EXEC = queryengine

LDFLAGS = 

# query engine details
#OBJS = queryengine.o index.o hash.o querylogic.o file.o
#SRCS = queryengine.c queryengine.h ../utils/index.c ../utils/hash.c  ../utils/hash.h querylogic.c 
OBJS = queryengine.o querylogic.o 
SRCS = queryengine.c queryengine.h querylogic.c 

# query engine unit test details
EXEC2 = queryengine_test
OBJS2 = queryengine_test.o querylogic.o 
SRCS2 = queryengine_test.c querylogic.c 

#CFLAGS1SRCS = ../utils/file.c # need diff flags

UTILDIR=../utils/
UTILFLAG=-ltseutil
UTILLIB=$(UTILDIR)libtseutil.a
UTILC=$(UTILDIR)hash.c $(UTILDIR)file.c $(UTILDIR)index.c
UTILH=$(UTILC:.c=.h)

# Commands start with TAB not spaces
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) -L$(UTILDIR) $(UTILFLAG)
$(OBJS): $(SRCS) 
	$(CC) $(CFLAGS) -c $(SRCS)
unit: $(SRCS2) 
	$(CC) $(CFLAGS) -c $(SRCS2) 
	$(CC) $(CFLAGS) -o $(EXEC2) $(OBJS2) -L$(UTILDIR) $(UTILFLAG)
unit2: $(SRCS2) 
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS2)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC2) $(OBJS2) -L$(UTILDIR) $(UTILFLAG)
	gdb --args queryengine_test

debug: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS) -L$(UTILDIR) $(UTILFLAG)
	gdb --args queryengine ../indexer_dir/index.dat ../crawler_dir/data
debug2: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS) -L$(UTILDIR) $(UTILFLAG)
	#gdb --args indexer ../crawler_dir/data/ index.dat index.dat index_new.dat

valgrind: $(OBJS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS) -L$(UTILDIR) $(UTILFLAG)
	#valgrind --tool=memcheck --leak-check=full --track-origins=yes ./queryengine ../indexer_dir/index.dat ../crawler_dir/data
	valgrind --tool=memcheck --leak-check=full ./queryengine ../indexer_dir/index.dat ../crawler_dir/data

valgrindgdb: $(OBJS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS) -L$(UTILDIR) $(UTILFLAG)
	valgrind --tool=memcheck --leak-check=full --vgdb-error=0 ./queryengine ../indexer_dir/index.dat ../crawler_dir/data

$(UTILLIB): $(UTILC) $(UTILH)
	cd $(UTILDIR); make;

clean:
	rm -f *~
	rm -f *#
	rm -f ./prs
	rm -f *.o
	rm -f *.gch
	rm -f core.*
	rm -f vgcore.*
	rm -f queryengine
	rm -f queryengine_test
	rm -f .nfs*

cleanlog:
	rm -f *log.*


