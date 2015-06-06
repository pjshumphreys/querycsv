
## Created by Anjuta

CC = gcc
CFLAGS = -g -Wall
OBJECTS = refhash.o parser.o lexer.o sql.o hash4.o strcomp.o querycsv.o
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
	rm -f *.o

.PHONY: all
.PHONY: count
.PHONY: clean
