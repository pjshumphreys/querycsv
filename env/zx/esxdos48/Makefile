qrycsv04.ovl: esxdos48.asm pager.bin
	(cat pager.inc page2page.inc | sort | uniq | grep -v -E '__head|__tail|__size') > esxdos48.inc
	z80asm -b -o=qrycsv04.ovl esxdos48.asm
	dd if=/dev/zero bs=1 count=`ls -nl qrycsv04.ovl | awk '{print 16384 - $$5}'` >> qrycsv04.ovl

pager.bin: pager.asm page2page.bin
	z80asm -b -r=49152 pager.asm
	z80asm -m -b -r=`ls -nl pager.bin | awk '{print 48384 - $$5}'` pager.asm
	-awk '{print $$1, "equ", $$3}' pager.map > pager.inc

page2page.bin: page2page.asm
	z80asm -m -b page2page.asm
	-awk '{print $$1, "equ", $$3}' page2page.map > page2page.inc

clean:
	rm -f *.inc *.o *.map *.bin qrycsv04.ovl
