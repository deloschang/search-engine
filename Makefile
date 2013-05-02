CC = gcc
CFLAGS = -Wall -pedantic -std=c99
EXEC = crawler
LDFLAGS = 

# my project details
OBJS = crawler.o hash.o html.o
SRCS = crawler.c hash.c html.c crawler.h hash.h html.h

# Commands start with TAB not spaces
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS)
debug: $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -c $(SRCS)
	$(CC) $(CFLAGS) -g -ggdb -o $(EXEC) $(OBJS)
clean:
	rm -f *~
	rm -f *#
	rm -f ./prs
	rm -f *.o
	rm -f *.gch


