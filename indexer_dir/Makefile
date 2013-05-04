CC = gcc
CFLAGS = -Wall -pedantic -std=c99
EXEC = crawler
LDFLAGS = 
TEST = ./crawler_test.sh

# my project details
OBJS = crawler.o hash.o html.o
SRCS = crawler.c hash.c html.c crawler.h hash.h html.h

# Commands start with TAB not spaces
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
	$(TEST) # test the crawler once made
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)
debug: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)
	gdb --args crawler www.cs.dartmouth.edu ./data/ 1
valgrind: $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
	valgrind --tool=memcheck --leak-check=yes ./crawler www.cs.dartmouth.edu ./data 2

clean:
	rm -f *~
	rm -f *#
	rm -f ./prs
	rm -f *.o
	rm -f *.gch
	rm -f *.html.*
	rm -f core.*

# to clean both log and crawled data files
cleanlog:
	rm -f *log.*
	cd ./data/ && rm -f *


