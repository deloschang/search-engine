CC = gcc
CFLAGS = -Wall -pedantic -std=c99
CFLAGS1 = -Wall 

EXEC = indexer
LDFLAGS = 

# my project details
OBJS = indexer.o 

SRCS = indexer.c 

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
debug: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS) -L$(UTILDIR) $(UTILFLAG)
	gdb --args indexer ../crawler_dir/data/ index.dat
debug2: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS) -L$(UTILDIR) $(UTILFLAG)
	gdb --args indexer ../crawler_dir/data/ index.dat index.dat index_new.dat

valgrind: $(OBJS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS) -L$(UTILDIR) $(UTILFLAG)
	valgrind --tool=memcheck --leak-check=full ./indexer ../crawler_dir/data index.dat

clean:
	rm -f *~
	rm -f *#
	rm -f ./prs
	rm -f *.o
	rm -f *.gch
	rm -f core.*
	rm -f vgcore.*
	rm -f indexer

cleanlog:
	rm -f *log.*
	rm -f index.dat
	rm -f index_new.dat

$(UTILLIB): $(UTILC) $(UTILH)
	cd $(UTILDIR); make;


