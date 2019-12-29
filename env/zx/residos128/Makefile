qrycsv03.ovl: residos128.asm atexit.bin fputc_cons_first.bin pager.bin
	(cat pager.inc page2page.inc | sort |uniq | grep -v -E '__head|__tail|__size') > residos128.inc
	z80asm -b -o=qrycsv03.ovl residos128.asm
	dd if=/dev/zero bs=1 count=`ls -nl qrycsv03.ovl | awk '{print 16384 - $$5}'` >> qrycsv03.ovl


atexit.bin: atexit.asm pager.bin
	z80asm -b atexit.asm

fputc_cons_first.bin: fputc_cons.asm
	z80asm -b --split-bin fputc_cons.asm

pager.bin: pager.asm page2page.bin
	z80asm -b -r=49152 pager.asm
	z80asm -m -b -r=`ls -nl pager.bin | awk '{print 48384 - $$5}'` pager.asm
	-awk '{print $$1, "equ", $$3}' pager.map > pager.inc

page2page.bin: page2page.asm
	z80asm -m -b page2page.asm
	-awk '{print $$1, "equ", $$3}' page2page.map > page2page.inc

clean:
	rm -f *.inc *.o *.map *.bin qrycsv03.ovl