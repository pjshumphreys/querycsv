SOURCES = $(wildcard *.c)
headers = $(wildcard *.h)

qrycsv16.exe: pager.asm $(SOURCES) $(HEADERS)
	node ./build-dos.js
	mv build/qcsv.exe output/
	cp qcsv00.bin output/qcsv00.ovl

.PHONY: clean
clean:
	rm -rf build output querycsv2.c *.o *.err *.bak
