CC = gcc
CFLAGS = -Wall -pedantic -std=c99
EXEC = crawler
LDFLAGS = 

# crawler project details
OBJS = crawler.o html.o
SRCS = crawler.c html.c html.h

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
	gdb --args crawler www.cs.dartmouth.edu ./data/ 1
valgrind: $(OBJS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS) -L$(UTILDIR) $(UTILFLAG)
	valgrind --tool=memcheck --leak-check=yes ./crawler www.cs.dartmouth.edu ./data 2

clean:
	rm -f *~
	rm -f *#
	rm -f ./prs
	rm -f *.o
	rm -f *.gch
	rm -f *.html.*
	rm -f index.html
	rm -f core.*
	rm -f crawler

# to clean both log and crawled data files
cleanlog:
	rm -f *log.*
	cd ./data/ && rm -f *

$(UTILLIB): $(UTILC) $(UTILH)
	cd $(UTILDIR); make;

