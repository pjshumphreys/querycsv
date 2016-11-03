CC = gcc
GPERF = gperf
CFLAGS = -g -Wall
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
	gcc -fpreprocessed -dD -E lexer.c > lexer2.c
	sed '/^[#] [0-9]*/d' lexer2.c > lexer.c
	rm -rf lexer2.c
	
hash1.o: hash1.h gen.h en_gb.h querycsv.h
hash2.o: hash2.h gen.h en_gb.h querycsv.h
hash3.o: hash3.h gen.h en_gb.h querycsv.h
querycsv.o: gen.h en_gb.h querycsv.h
sql.o: lexer.c gen.h en_gb.h querycsv.h
lexer.o: gen.h en_gb.h querycsv.h

querycsv: sql.o lexer.o hash1.o hash2.o hash3.o hash4.o querycsv.o
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/posix/ \;
	find . -maxdepth 1 -type f -iname \*.o ! -exec mv {} env/posix/ \;
	cd env/posix; $(CC) -o querycsv sql.o lexer.o hash1.o hash2.o hash3.o hash4.o querycsv.o $(LDFLAGS) $(LIBS)
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/dos/ \;
	cd env/dos; unix2dos *
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/win32/ \;
	cd env/win32; unix2dos *
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/m68kmac/ \;
	cd env/m68kmac; unix2mac *
	find . -maxdepth 1 -type f -iname \*.c ! -name 'makeheaders.c' -exec cp {} env/amiga/ \;
	find . -maxdepth 1 -type f -iname \*.h -exec cp {} env/amiga/ \;
	find . -maxdepth 1 -type f -iname \*.c ! -name 'makeheaders.c' -exec cp {} env/riscos/c/ \;
	find . -maxdepth 1 -type f -iname \*.h -exec cp {} env/riscos/h/ \;
	cd env/riscos/c; find . -name "*.c" | sed -e "p;s/\.c$$//" | xargs -n2 mv
	cd env/riscos/h; find . -name "*.h" | sed -e "p;s/\.h$$//" | xargs -n2 mv
  
count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f *.o makeheaders querycsv gen.h querycsv.c hash4.c sql.c lexer.c sql.h lexer.h
	cd env/dos; find . -maxdepth 1 ! -path './dbuild.bat' ! -path './DOSBox.exe' ! -path './SDL.dll' ! -path './SDL_net.dll' ! -path '..' ! -path '.' -exec rm -rf {} \;
	cd env/win32; find . -maxdepth 1 ! -path './win32.c' ! -path './win32.h' ! -path './wbuild.bat' ! -path '..' ! -path '.' -exec rm -rf {} \;
	cd env/m68kmac; find . -type f ! -path './.finf/TEGlue.a' ! -path './TEGlue.a' ! -path './.finf/Makefile' ! -path './Makefile' ! -path './mac.h' ! -path './mac.c' ! -path './mac.r' ! -path './size.r' ! -path './blank.zip' -exec rm {} \;; find . -maxdepth 1 -type d ! -path '..' ! -path '.' ! -path './.finf' -exec rm -rf {} \;

.PHONY: all
.PHONY: count
.PHONY: clean
