
## Created by Anjuta

CC = gcc
CFLAGS = -O2 -g -Wall
SOURCES = lexer.c sql.c hash2.c hash3.c hash4a.c hash4b.c hash4c.c querycsv.c
OBJECTS = $(SOURCES:%.c=%.o)
INCFLAGS = -I/home/user/Projects/querycsv
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = 

all: querycsv

querycsv: $(OBJECTS)
	$(CC) -o querycsv $(OBJECTS) $(LDFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

.c.o :
	$(CC) -o $@ -c $(CFLAGS) $< $(INCFLAGS)

musl:
	rm -f $(OBJECTS) querycsv
	/usr/local/musl/bin/musl-gcc -o querycsv -static -Os -O2 $(SOURCES)
	strip querycsv
	upx querycsv

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f $(OBJECTS) querycsv

.PHONY: all
.PHONY: count
.PHONY: clean
