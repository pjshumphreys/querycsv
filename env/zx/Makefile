all: qcsv00zx.ovl qcsv01zx.ovl qcsv02zx.ovl qcsv03zx.ovl qcsv04zx.ovl qcsv05zx.ovl

#querycsv.tap: qcsv06zx.ovl
querycsv.tap:
	cd common && make
	cp common/querycsv.tap .

qcsv00zx.ovl:
	cp qrycsv00.bin qcsv00zx.ovl

qcsv01zx.ovl: qcsv06zx.ovl
	cd plus3dos && make
	cp plus3dos/qcsv01zx.ovl .

qcsv02zx.ovl: qcsv06zx.ovl
	cd residos48 && make
	cp residos48/qcsv02zx.ovl .

qcsv03zx.ovl: querycsv.tap
	cd residos128 && make
	cp residos128/qcsv03zx.ovl .

qcsv04zx.ovl: qcsv06zx.ovl
	cd esxdos48 && make
	cp esxdos48/qcsv04zx.ovl .

qcsv05zx.ovl: qcsv06zx.ovl
	cd esxdos128 && make
	cp esxdos128/qcsv05zx.ovl .

qcsv06zx.ovl: querycsv.tap libc.c lexer2.c sql2.c querycsv.c stdio.asm
	node ./build-zx.js

clean:
	cd common && make clean
	cd plus3dos && make clean
	cd residos48 && make clean
	cd residos128 && make clean
	cd esxdos48 && make clean
	cd esxdos128 && make clean
	rm -rf build querycsv.tap *.ovl
