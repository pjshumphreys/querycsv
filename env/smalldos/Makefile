SOURCES = $(wildcard *.c)
headers = $(wildcard *.h)

qrycsv16.exe: pager.asm $(SOURCES) $(HEADERS)
	node ./build-dos.js
	mv build/qrycsv.exe output/
	cp qrycsv00.bin output/qcsv00pc.ovl

.PHONY: clean
clean:
	rm -rf build output querycsv2.c *.o *.err *.bak
