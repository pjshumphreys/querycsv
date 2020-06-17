#ROSIZE=$(shell sh -c "ls -nl build/rodata.bin | awk '{print 16128 - \$$5}'")

all: querycsv.com qrycsv00.ovl

querycsv.com: launcher2.bin launcher.c launcher.asm
	zcc +cpm -lndos -m -o querycsv.com launcher.asm launcher.c

launcher2.bin: launcher2.com
	z88dk-zx7 ./launcher2.com
	mv launcher2.com.zx7 launcher2.bin

launcher2.com: launcher2.c
	zcc +cpm -lm -pragma-define:CRT_ORG_CODE=16384 -m -o launcher2.com launcher2.c

qrycsv00.ovl: qrycsv00.bin
	cp qrycsv00.bin qrycsv00.ovl

#qrycsv01.ovl: qrycsv03.ovl
#	cd plus3dos && make
#	cp plus3dos/qcsv01zx.ovl .
#	dd if=build/rodata.bin of=qcsv01zx.ovl bs=1 seek=$(ROSIZE) conv=notrunc

#qrycsv02.ovl: qrycsv03.ovl
#	cd residos48 && make
#	cp residos48/qcsv02zx.ovl .
#	dd if=build/rodata.bin of=qcsv02zx.ovl bs=1 seek=$(ROSIZE) conv=notrunc

#qrycsv03.ovl: libc.c lexer2.c sql2.c querycsv.c stdio.asm build-cpm.js
#	node ./build-cpm.js
#	cp build/obj2/*.ovl .

clean:
	rm -rf build zcc_opt.def launcher2.bin launcher2.com querycsv.com *.ovl *.map
