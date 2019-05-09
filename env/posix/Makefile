
## Created by Anjuta

CC = gcc
CFLAGS = -g -Wall
SOURCES = querycsv.c lexer.c sql.c hash2.c hash3.c hash4a.c hash4b.c hash4c.c
OBJECTS = $(SOURCES:%.c=%.o)
INCFLAGS = -I/home/user/Projects/querycsv
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS =

WATCOM = ~/.wine/drive_c/WATCOM

all: querycsv

querycsv: $(OBJECTS)
	$(CC) -o querycsv $(OBJECTS) $(LDFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES: .c .cc .C .cpp .o

.c.o :
	$(CC) -o $@ -c $(CFLAGS) $< $(INCFLAGS)

querycsv.arm: $(SOURCES)
	rm -f $(OBJECTS) querycsv
	/opt/cross/bin/arm-linux-musleabi-gcc -o querycsv -static -Os -O2 -fmax-errors=20 -Wl,-O,-s,--gc-sections $(SOURCES)
	/opt/cross/bin/arm-linux-musleabi-strip --remove-section=.eh_frame --remove-section=.comment ./querycsv
	upx querycsv
	mv querycsv querycsv.arm

querycsv.x64: $(SOURCES)
	rm -f $(OBJECTS) querycsv
	/usr/local/musl/bin/musl-gcc -o querycsv -static -Os -O2 -fmax-errors=20 -Wl,-O,-s,--gc-sections $(SOURCES)
	strip --remove-section=.eh_frame --remove-section=.comment ./querycsv
	upx querycsv
	mv querycsv querycsv.x64

querycsv.386: $(SOURCES)
	rm -f $(OBJECTS) querycsv querycsv.i386
	sed -i -e 's/MAC_YIELD//g' querycsv.c
	export WATCOM=$(WATCOM); export PATH=$(WATCOM)/binl:$(PATH); export INCLUDE=$(WATCOM)/lh; wcl386 -os $(SOURCES)
	xxd -l128 -s 84 querycsv | xxd -r -s -32 - querycsv
	echo "000002c: 02" | xxd -r - querycsv
	strip querycsv
	upx querycsv --force-execve
	mv querycsv querycsv.386

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f $(OBJECTS) querycsv querycsv.i386

.PHONY: all
.PHONY: count
.PHONY: clean
