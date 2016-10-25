CC = gcc
GPERF = gperf
CFLAGS = -g -Wall -DNO_STRICMP 
SOURCES = $(wildcard split/*.c)
OBJECTS = $(SOURCES:%.c=%.o)
INCFLAGS = -I/home/user/Projects/querycsv
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = 

all: querycsv

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

.c.o :
	$(CC) -o $@ -c $(CFLAGS) $< $(INCFLAGS)

makeheaders: makeheaders.c
	$(CC) -o makeheaders makeheaders.c

gen.h: makeheaders $(SOURCES)
	./makeheaders $(SOURCES) -h > gen.h

querycsv.c: $(SOURCES)
	cat $(SOURCES) > querycsv.c

hash4.c: hash4.gperf
	gperf hash4.gperf > hash4.c

sql.c: sql.y lexer.c querycsv.h
	bison sql.y

lexer.c: sql.l querycsv.h
	flex sql.l

hash1.o: hash1.h gen.h en_gb.h querycsv.h
hash2.o: hash2.h gen.h en_gb.h querycsv.h
hash3.o: hash3.h gen.h en_gb.h querycsv.h
querycsv.o: gen.h en_gb.h querycsv.h
sql.o: lexer.c gen.h en_gb.h querycsv.h
lexer.o: gen.h en_gb.h querycsv.h

querycsv: sql.o lexer.o hash1.o hash2.o hash3.o hash4.o querycsv.o
	$(CC) -o querycsv sql.o lexer.o hash1.o hash2.o hash3.o hash4.o querycsv.o $(LDFLAGS) $(LIBS)

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f *.o makeheaders querycsv gen.h querycsv.c hash4.c sql.c lexer.c sql.h lexer.h

.PHONY: all
.PHONY: count
.PHONY: clean
