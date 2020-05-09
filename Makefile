CC = gcc
CFLAGS = -O2 -g -Wall -std=c99 -ansi
SOURCES = $(wildcard split/*.c)
OBJECTS = $(SOURCES:%.c=%.o)
INCFLAGS = -I/home/user/Projects/querycsv
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS =

all: querycsv

.SUFFIXES:
.SUFFIXES: .c .cc .C .cpp .o

.c.o :
	$(CC) -o $@ -c $(CFLAGS) $< $(INCFLAGS)

makeheaders: makeheaders.c
	$(CC) -o makeheaders makeheaders.c

gen.h: makeheaders $(SOURCES)
	./makeheaders $(SOURCES) -h > gen.h

hash3.h: ./generateMappings.js
	node ./generateMappings.js

querycsv.c: hash3.h $(SOURCES)
	printf "#include \"querycsv.h\"\n" > querycsv.c
	cat $(SOURCES) >> querycsv.c

hash4a.c: hash4a.gperf
	gperf hash4a.gperf > hash4a.c

hash4b.c: hash4b.gperf
	gperf hash4b.gperf > hash4b.c

hash4c.c: hash4c.gperf
	gperf hash4c.gperf > hash4c.c

hash2.c: UnicodeData.txt generate_hash2.js dcompose.json hash2iT.h hash2outT.h hash2T.h
	npm install graceful-fs match-all strip-json-comments readline walk shell-escape cheerio normalize-html-whitespace
	node ./generate_hash2.js
	cp hash2T.h hash2.c

lexer.c: sql.l querycsv.h
	flex sql.l
	$(CC) -fpreprocessed -dD -E lexer.c > lexer2.c
	sed "/^#line/d;/^[#] [0-9]*/d;s/^YY_DECL$$/int yylex (YYSTYPE * yylval_param , yyscan_t yyscanner)/g;" lexer2.c > lexer.c
	rm -rf lexer2.c

sql.c: sql.y lexer.c
	bison sql.y
#	sed -i.bak "/^#line/d" sql.c
#	rm sql.c.bak

hash2.o: gen.h en_gb.h querycsv.h
hash3.o: hash3.h gen.h en_gb.h querycsv.h
hash4a.o: gen.h en_gb.h querycsv.h
hash4b.o: gen.h en_gb.h querycsv.h
hash4c.o: gen.h en_gb.h querycsv.h
querycsv.o: gen.h en_gb.h querycsv.h
sql.o: lexer.c gen.h en_gb.h querycsv.h
lexer.o: gen.h en_gb.h querycsv.h

hash2: hash2.c
	mkdir -p env/bbcarm/c
	mkdir -p env/bbcarm/h
	mkdir -p env/bbcarm/o
	mkdir -p hash2
	cd hash2 && node ../generate_hash2.js 398 && echo ../env/powermac ../env/bbcarm/c ../env/c64 | xargs -n 1 cp *.c

dat/create: hash2.c dat/create.c gen.h en_gb.h querycsv.h split/variables.c dat/externs.c
	cd dat && $(CC) create.c externs.c -o create

dat/qrycsv00.ovl: dat/create dat/hash2dat.c
	cd dat && ./create && (echo ../env/zx ../env/cpm | xargs -n 1 cp hash2dat.h hash2dat.c qrycsv00.ovl)
	mv env/cpm/qrycsv00.ovl env/cpm/qrycsv00.bin
	mv env/zx/qrycsv00.ovl env/zx/qrycsv00.bin
	sed -i "s/qrycsv00/qcsv00zx/" env/zx/hash2dat.c
#	mv env/dos/qrycsv00.ovl env/dos/querycsv.ovl
#	sed -i "s/qrycsv00/querycsv/" env/dos/hash2dat.c

env/bbcarm/c: hash2
	find . -maxdepth 1 -type f -iname \*.c ! -name 'makeheaders.c' ! -name 'hash2*' -exec cp {} env/bbcarm/c/ \;
	find . -maxdepth 1 -type f -iname \*.h ! -name 'hash2*' -exec cp {} env/bbcarm/h/ \;
	cd env/bbcarm/c; sed -i -E 's/\/\*[^\*]+\*\//\/\* \*\//g' *
	cd env/bbcarm/c; sed -i -E 's/const char \*p;/char \*p;/g;s/short/long/g' *.c; cp ../bbcarm.c bbcarm; bash -c 'ls | cat -n | while read n f; do mv "$$f" $$(printf "\x$$(printf %x $$(($$n+96)))"); done';
	cd softfloat; bash -c 'ls *.c | cat -n | while read n f; do cp "$$f" ../env/bbcarm/c/$$(printf "\x$$(printf %x $$(($$n+47)))"); cp "$$f" ../env/riscos/c/; done';find . -maxdepth 1 -type f -iname \*.h -exec cp {} ../env/bbcarm/h/ \;
	mkdir -p env/riscos/h
	cd softfloat; find . -maxdepth 1 -type f -iname \*.h -exec cp {} ../env/riscos/h/ \;
	cd env/bbcarm/c; cp ../mode.c mode; sed -i -E '/#include <errno\.h>/d;s/#include( [<"])([^.]+)\.h([">])/#include\1h\.\2\3/g' *
	cd env/bbcarm/h; find . -name "*.h" | sed -e "p;s/\.h$$//" | xargs -n2 mv; sed -i -E '/#include <errno\.h>/d;s/#include( [<"])([^.]+)\.h([">])/#include\1h\.\2\3/g' *

querycsv: sql.o lexer.o hash2.o hash3.o hash4a.o hash4b.o hash4c.o querycsv.o env/bbcarm/c dat/qrycsv00.ovl
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/posix/ \;
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/html5/ \;
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/dos/ \;
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' ! -name 'hash2*.*' -exec cp {} env/zx/ \;
	cd env/dos; unix2dos *
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/win32/ \;
	cd env/win32; unix2dos *
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' -exec cp {} env/m68kmac/ \;
	cd env/m68kmac; unix2mac *
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' ! -name 'hash2*' -exec cp {} env/powermac/ \;
	cd env/powermac; unix2mac *
	find . -maxdepth 1 -type f -iname \*.c ! -name 'makeheaders.c' -exec cp {} env/amiga/ \;
	find . -maxdepth 1 -type f -iname \*.h -exec cp {} env/amiga/ \;
	find . -maxdepth 1 -type f -iname \*.c ! -name 'makeheaders.c' -exec cp {} env/atarist/ \;
	find . -maxdepth 1 -type f -iname \*.h -exec cp {} env/atarist/ \;
	find . -maxdepth 1 -type f -iname \*.c ! -name 'makeheaders.c' -exec cp {} env/riscos/c/ \;
	find . -maxdepth 1 -type f -iname \*.h -exec cp {} env/riscos/h/ \;
	cd env/riscos/c; find . -name "*.c" | sed -e "p;s/\.c$$//" | xargs -n2 mv
	cd env/riscos/h; find . -name "*.h" | sed -e "p;s/\.h$$//" | xargs -n2 mv
	find ./env/riscos/launcher/c -name "runimage.c" -exec mv {} env/riscos/launcher/c/runimage \;
	find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -name 'makeheaders.c' ! -name 'hash4*.c' ! -name 'hash2*.c' -exec cp {} env/c64/ \;
	cp hash4a.c env/c64/hash4a.h
	cp hash4b.c env/c64/hash4b.h
	cp hash4c.c env/c64/hash4c.h
	$(CC) -o querycsv sql.o lexer.o hash2.o hash3.o hash4a.o hash4b.o hash4c.o querycsv.o $(LDFLAGS) $(LIBS)
count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -rf makeheaders querycsv node_modules package-lock.json gen.h ansimap.h atarimap.h cmnmap.h hash3.h petmap.h zxmap.h querycsv.c hash4a.c hash4b.c hash4c.c hash2.c hash2in*.h hash2out.h hash2.c sql.c lexer.c sql.h lexer.h
	find . -maxdepth 1 -type f \( -iname \*.o \) -exec rm -rf {} \;
	cd env/html5 && find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h -o -iname \*.o \) ! -path './emcc.c' ! -path './helper.c' -exec rm -rf {} \;
	cd env/posix && rm -rf querycsv; find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h -o -iname \*.err -o -iname \*.o \) -exec rm -rf {} \;
	cd env/dos && find . -maxdepth 1 ! -path './dos.c' ! -path './direct.cfg' ! -path './Makefile' ! -path '..' ! -path '.' -exec rm -rf {} \;
	cd env/win32 && find . -maxdepth 1 ! -path './win32.c' ! -path './direct.cfg' ! -path './querycsv.ico' ! -path './querycsv.rc' ! -path './Makefile' ! -path '..' ! -path '.' -exec rm -rf {} \;
	cd env/m68kmac && find . -type f ! -path './.finf/TEGlue.a' ! -path './TEGlue.s' ! -path './TEGlue.a' ! -path './.finf/QueryCSV.make' ! -path './QueryCSV.make' ! -path './CMakeLists.txt' ! -path './mac.h' ! -path './mac.c' ! -path './mac.r' ! -path './size.r' ! -path './blank.zip' -exec rm {} \; && find . -maxdepth 1 -type d ! -path '..' ! -path '.' ! -path './.finf' -exec rm -rf {} \; && mac2unix *
	cd env/powermac && find . -type f ! -path './.finf/Makefile' ! -path './Makefile' ! -path './powermac.h' ! -path './powermac.c' ! -path './mac.c' ! -path './powermac.r' ! -path './size.r' ! -path './carbon.r' ! -path './blank.zip' -exec rm {} \; && find . -maxdepth 1 -type d ! -path '..' ! -path '.' ! -path './.finf' -exec rm -rf {} \; && mac2unix *
	rm -rf env/bbcarm/c env/bbcarm/h env/bbcarm/o hash2
	cd env/riscos && rm -rf o od \!QueryCSV/querycsv,ff8 \!QueryCSV/\!RunImage,ff8
	cd env/riscos/launcher && rm -rf o od
	cd env/riscos/c && find . -maxdepth 1 ! -path './riscos' ! -path './riscos.c' ! -path '..' ! -path '.' -exec rm -rf {} \;
	find ./env/riscos/c -name "riscos" -exec mv {} env/riscos/c/riscos.c \;
	find ./env/riscos/launcher/c -name "runimage" -exec mv {} env/riscos/launcher/c/runimage.c \;
	cd env/bbcarm && rm -rf c h o
	cd env/riscos/h && find . -maxdepth 1 ! -path '..' ! -path '.' -exec rm -rf {} \;
	cd env/amiga && rm -rf QueryCSV; find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -path './amiga.c' -exec rm -rf {} \;
	cd env/atarist && rm -rf querycsv.ttp; find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h \) ! -path './atarist.c' -exec rm -rf {} \;
	cd env/c64 && rm -rf build crt0.o floatlib/float.o querycsv.crt && find . -maxdepth 1 -type f -iname \*.c ! -name 'c64.c' ! -name 'floatlib.c' ! -name 'hash4a.c' ! -name 'hash4b.c' ! -name 'hash4c.c' ! -name 'lexer2.c' ! -name 'libc.c' ! -name 'sql2.c' -exec rm -rf {} \; && find . -maxdepth 1 -type f -iname \*.h ! -name 'cc65iso.h' -exec rm -rf {} \;
	cd env/zx && make clean && find . -maxdepth 1 -type f \( -iname \*.c -o -iname \*.h -o -iname \*.bin \) ! -path './lexer2.c' ! -path './sql2.c' ! -path './libc.c' -exec rm -rf {} \;

.PHONY: all
.PHONY: count
.PHONY: clean
