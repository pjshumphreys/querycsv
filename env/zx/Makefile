ROSIZE=$(shell sh -c "ls -nl build/rodata.bin | awk '{print 16384 - \$$5}'")

all: querycsv.tap qcsv00zx.ovl qcsv01zx.ovl qcsv02zx.ovl qcsv03zx.ovl qcsv04zx.ovl qcsv05zx.ovl

querycsv.tap:
	cd common && make
	cp common/querycsv.tap .

qcsv00zx.ovl: qrycsv00.bin
	cp qrycsv00.bin qcsv00zx.ovl

qcsv01zx.ovl: qcsv06zx.ovl
	cd plus3dos && make
	cp plus3dos/qcsv01zx.ovl .
	dd if=build/rodata.bin of=qcsv01zx.ovl bs=1 seek=$(ROSIZE) conv=notrunc

qcsv02zx.ovl: qcsv06zx.ovl
	cd residos48 && make
	cp residos48/qcsv02zx.ovl .
	dd if=build/rodata.bin of=qcsv02zx.ovl bs=1 seek=$(ROSIZE) conv=notrunc

qcsv03zx.ovl: querycsv.tap
	cd residos128 && make
	cp residos128/qcsv03zx.ovl .
	dd if=build/rodata.bin of=qcsv03zx.ovl bs=1 seek=$(ROSIZE) conv=notrunc

qcsv04zx.ovl: qcsv06zx.ovl
	cd esxdos48 && make
	cp esxdos48/qcsv04zx.ovl .
	dd if=build/rodata.bin of=qcsv04zx.ovl bs=1 seek=$(ROSIZE) conv=notrunc

qcsv05zx.ovl: qcsv06zx.ovl
	cd esxdos128 && make
	cp esxdos128/qcsv05zx.ovl .
	dd if=build/rodata.bin of=qcsv05zx.ovl bs=1 seek=$(ROSIZE) conv=notrunc

qcsv06zx.ovl: querycsv.tap libc.c lexer2.c sql2.c querycsv.c stdio.asm build-zx.js
	node ./build-zx.js
	cp build/obj2/*.ovl .

clean:
	cd common && make clean
	cd plus3dos && make clean
	cd residos48 && make clean
	cd residos128 && make clean
	cd esxdos48 && make clean
	cd esxdos128 && make clean
	rm -rf build querycsv.tap *.ovl
