
## Created by Anjuta

CC = gcc
CFLAGS = -g -Wall -DNO_STRICMP -I/home/user/Projects/querycsv
SOURCES = lexer.c sql.c hash4.c $(wildcard split/*.c)
OBJECTS = $(SOURCES:%.c=%.o)
INCFLAGS = 
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = 

all: querycsv

querycsv: $(OBJECTS)
	$(CC) -o querycsv $(OBJECTS) $(LDFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

.c.o :
	$(CC) -o $@ -c $(CFLAGS) $< $(INCFLAGS)

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f $(OBJECTS)

.PHONY: all
.PHONY: count
.PHONY: clean
