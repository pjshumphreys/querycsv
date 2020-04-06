qcsv04zx.ovl: esxdos48.asm pager.bin
	(cat pager.inc page2page.inc | sort | uniq | grep -v -E '__head|__tail|__size') > esxdos48.inc
	z80asm -b -o=qcsv04zx.ovl esxdos48.asm
	mv qcsv04zx_code_compiler.bin qcsv04zx.ovl
	dd if=/dev/zero bs=1 count=`ls -nl qcsv04zx.ovl | awk '{print 16384 - $$5}'` >> qcsv04zx.ovl

pager.bin: pager.asm page2page.bin pages.inc lookupTable.inc data.bin functions.inc
	z80asm -b -r=49152 pager.asm
	z80asm -m -b -r=`ls -nl pager.bin | awk '{print 48384 - $$5}'` pager.asm
	-awk '{print $$1, "equ", $$3}' pager.map > pager.inc

page2page.bin: page2page.asm
	z80asm -m -b page2page.asm
	-awk '{print $$1, "equ", $$3}' page2page.map > page2page.inc

pages.inc:
	cp ../common/pages.txt pages.inc

lookupTable.inc:
	cp ../common/lookupTable.txt lookupTable.inc

data.bin:
	z80asm -b -o=data.bin ../stdio.asm

functions.inc:
	cp ../common/functions.txt functions.inc

clean:
	rm -f *.inc *.o *.map *.bin qcsv04zx.ovl
