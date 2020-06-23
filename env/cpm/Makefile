#ROSIZE=$(shell sh -c "ls -nl build/rodata.bin | awk '{print 16128 - \$$5}'")

all: querycsv.com output

querycsv.com: launcher2.bin launcher.c launcher.asm
	zcc +cpm -lndos -m -o querycsv.com launcher.c launcher.asm

launcher2.bin: launcher2.com
	z88dk-zx7 ./launcher2.com
	mv launcher2.com.zx7 launcher2.bin

launcher2.com: launcher2.c
	zcc +cpm -lm -pragma-define:CRT_ORG_CODE=16384 -m -o launcher2.com launcher2.c

qrycsv01.ovl: pager.asm libc.c
	zcc +cpm -lm -m -D__DISABLE_BUILTIN -U__STDC_VERSION__ -o qrycsv01.ovl pager.asm libc.c

qrycsv02.ovl: pager.asm libc.c
	zcc +msx -subtype=msxdos2 -lm -m -D__DISABLE_BUILTIN -U__STDC_VERSION__ -D__Z88DK -o qrycsv02.ovl pager.asm libc.c

output: pager.asm libc.c lexer2.c sql2.c querycsv.c build-cpm.js
	node ./build-cpm.js
	cp qrycsv00.bin ./output/qrycsv00.ovl
	cp querycsv.com build/obj2/*.ovl ./output

clean:
	rm -rf build output zcc_opt.def launcher2.bin launcher2.com querycsv.com *.map
