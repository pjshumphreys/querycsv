all: querycsv.tap qrycsv01.ovl qrycsv03.ovl qrycsv05.ovl

#querycsv.tap: qrycsv06.ovl
querycsv.tap:
	cd common && make
	cp common/querycsv.tap .

#qrycsv00.ovl:
#	cp qrycsv00.bin qrycsv00.ovl

qrycsv01.ovl:
	cd plus3dos && make
	cp plus3dos/qrycsv01.ovl .

#qrycsv02.ovl:
#	cd residos48 && make
#	cp residos48/qrycsv02.ovl .

qrycsv03.ovl: querycsv.tap
	cd residos128 && make
	cp residos128/qrycsv03.ovl .

#qrycsv04.ovl:
#	cd esxdos48 && make
#	cp esxdos48/qrycsv04.ovl .

qrycsv05.ovl:
	cd esxdos128 && make
	cp esxdos128/qrycsv05.ovl .

#qrycsv06.ovl: libc.c lexer2.c sql2.c
#	node ./build-zx.js

clean:
	cd common && make clean
	cd plus3dos && make clean
#	cd residos48 && make clean
	cd residos128 && make clean
#	cd esxdos48 && make clean
	cd esxdos128 && make clean
	rm -rf querycsv.tap *.ovl
