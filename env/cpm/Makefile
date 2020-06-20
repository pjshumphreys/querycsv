#ROSIZE=$(shell sh -c "ls -nl build/rodata.bin | awk '{print 16128 - \$$5}'")

all: querycsv.com qrycsv00.ovl qrycsv03.ovl

querycsv.com: launcher2.bin launcher.c launcher.asm
	zcc +cpm -lndos -m -o querycsv.com launcher.asm launcher.c

launcher2.bin: launcher2.com
	z88dk-zx7 ./launcher2.com
	mv launcher2.com.zx7 launcher2.bin

launcher2.com: launcher2.c
	zcc +cpm -lm -pragma-define:CRT_ORG_CODE=16384 -m -o launcher2.com launcher2.c

qrycsv00.ovl: qrycsv00.bin
	cp qrycsv00.bin qrycsv00.ovl

qrycsv01.ovl: pager.asm libc.c
	zcc +cpm -lm -m -o qrycsv01.ovl pager.asm libc.c
	cp qrycsv01.ovl ../obj2/

qrycsv02.ovl: pager.asm libc.c
	zcc +msx -subtype=msxdos2 -lm -m -D__Z88DK -o qrycsv02.ovl pager.asm libc.c
	cp qrycsv01.ovl ../obj2/

qrycsv03.ovl: pager.asm libc.c lexer2.c sql2.c querycsv.c build-cpm.js
	node ./build-cpm.js
	cp build/obj2/*.ovl .

clean:
	rm -rf build zcc_opt.def launcher2.bin launcher2.com querycsv.com *.ovl *.map
