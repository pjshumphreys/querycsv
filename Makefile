CC = gcc
GPERF = gperf
CFLAGS = -O2 -g -Wall -std=c99 -ansi
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

hash4a.c: hash4a.gperf
	gperf hash4a.gperf > hash4a.c

hash4b.c: hash4b.gperf
	gperf hash4b.gperf > hash4b.c

hash4c.c: hash4c.gperf
	gperf hash4c.gperf > hash4c.c

hash2.c: UnicodeData.txt weired.json hash2iT.h hash2outT.h hash2T.h
	npm install graceful-fs strip-json-comments
	node ./generate_hash2.js
	cp hash2T.h hash2.c

sql.c: sql.y lexer.c querycsv.h
	bison sql.y

lexer.c: sql.l querycsv.h yyinput.h
	flex sql.l
	gcc -fpreprocessed -dD -E lexer.c > lexer2.c
	sed '/^[#] [0-9]*/d' lexer2.c > lexer.c
	rm -rf lexer2.c
	
hash2.o: gen.h en_gb.h querycsv.h
hash3.o: hash3.h gen.h en_gb.h querycsv.h
querycsv.o: gen.h en_gb.h querycsv.h
sql.o: lexer.c gen.h en_gb.h querycsv.h
lexer.o: gen.h en_gb.h querycsv.h

querycsv: sql.o lexer.o hash2.o hash3.o hash4a.o hash4b.o hash4c.o querycsv.o
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/posix/ \;
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/html5/ \;
	find . -maxdepth 1 -type f -iname \*.o ! -exec mv {} env/posix/ \;
	cd env/posix; $(CC) -o querycsv sql.o lexer.o hash2.o hash3.o hash4a.o hash4b.o hash4c.o querycsv.o $(LDFLAGS) $(LIBS)
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/dos/ \;
	cd env/dos; unix2dos *
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/win32/ \;
	cd env/win32; unix2dos *
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/m68kmac/ \;
	cd env/m68kmac; unix2mac *
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/powermac/ \;
	cd env/powermac; unix2mac *
	find . -maxdepth 1 -type f -iname \*.c ! -name 'makeheaders.c' -exec cp {} env/amiga/ \;
	find . -maxdepth 1 -type f -iname \*.h -exec cp {} env/amiga/ \;
	find . -maxdepth 1 -type f -iname \*.c ! -name 'makeheaders.c' -exec cp {} env/riscos/c/ \;
	mkdir -p env/riscos/h
	find . -maxdepth 1 -type f -iname \*.h -exec cp {} env/riscos/h/ \;
	cd env/riscos/c; find . -name "*.c" | sed -e "p;s/\.c$$//" | xargs -n2 mv
	cd env/riscos/h; find . -name "*.h" | sed -e "p;s/\.h$$//" | xargs -n2 mv
	find ./env/riscos/launcher/c -name "runimage.c" -exec mv {} env/riscos/launcher/c/runimage \;
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' ! -name 'hash4*.c' ! -name 'hash2*.c' -exec cp {} env/c64/ \;
	cp UnicodeData.txt env/c64/
	cp generate_hash2.js env/c64/
	cp weired.json env/c64/
	cp hash4a.c env/c64/hash4a.h
	cp hash4b.c env/c64/hash4b.h
	cp hash4c.c env/c64/hash4c.h
count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f makeheaders querycsv gen.h querycsv.c hash4a.c hash4b.c hash4c.c hash2.c hash2in*.h hash2out.h hash2.c sql.c lexer.c sql.h lexer.h
	cd env/html5; find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h -o -iname \*.o \) ! -path './emcc.c' ! -path './helper.c' -exec rm -rf {} \;
	cd env/posix; rm -rf querycsv; find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h -o -iname \*.o \) -exec rm -rf {} \;
	cd env/dos; find . -maxdepth 1 ! -path './Makefile' ! -path './DOSBox.exe' ! -path './SDL.dll' ! -path './SDL_net.dll' ! -path '..' ! -path '.' -exec rm -rf {} \;
	cd env/win32; find . -maxdepth 1 ! -path './win32.c' ! -path './Makefile' ! -path '..' ! -path '.' -exec rm -rf {} \;
	cd env/m68kmac; find . -type f ! -path './.finf/TEGlue.a' ! -path './TEGlue.a' ! -path './.finf/Makefile' ! -path './Makefile' ! -path './mac.h' ! -path './mac.c' ! -path './mac.r' ! -path './size.r' ! -path './blank.zip' -exec rm {} \;; find . -maxdepth 1 -type d ! -path '..' ! -path '.' ! -path './.finf' -exec rm -rf {} \;; mac2unix *
	cd env/powermac; find . -type f ! -path './.finf/Makefile' ! -path './Makefile' ! -path './powermac.h' ! -path './powermac.c' ! -path './powermac.r' ! -path './size.r' ! -path './carbon.r' ! -path './blank.zip' -exec rm {} \;; find . -maxdepth 1 -type d ! -path '..' ! -path '.' ! -path './.finf' -exec rm -rf {} \;; mac2unix *
	cd env/riscos; rm -rf o od \!QueryCSV/querycsv,ff8 \!QueryCSV/\!RunImage,ff8
	cd env/riscos/launcher; rm -rf o od
	cd env/riscos/c; find . -maxdepth 1 ! -path './riscos' ! -path './riscos.c' ! -path '..' ! -path '.' -exec rm -rf {} \;
	find ./env/riscos/c -name "riscos" -exec mv {} env/riscos/c/riscos.c \;
	find ./env/riscos/launcher/c -name "runimage" -exec mv {} env/riscos/launcher/c/runimage.c \;
	cd env/riscos/h; find . -maxdepth 1 ! -path '..' ! -path '.' -exec rm -rf {} \;
	cd env/amiga; rm -rf QueryCSV; find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -path './amiga.c' -exec rm -rf {} \;

.PHONY: all
.PHONY: count
.PHONY: clean
