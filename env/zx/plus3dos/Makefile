qcsv01zx.ovl: plus3dos.asm atexit.bin fputc_cons_first.bin pager.bin
	(cat pager.inc page2page.inc | sort | uniq | grep -v -E '__head|__tail|__size') > plus3dos.inc
	zcc +zx -S --c-code-in-asm --no-crt -lm -lp3 -pragma-define:CRT_ORG_CODE=0xc000 -pragma-define:CRT_ORG_DATA=0 -pragma-define:CRT_ORG_BSS=0x8000 -pragma-define:CLIB_MALLOC_HEAP_SIZE=0 -pragma-define:CRT_ON_EXIT=0x10002 -pragma-define:CRT_ENABLE_COMMANDLINE=2 -D__DISABLE_BUILTIN -U__STDC_VERSION__ ../libc.c -m -o qcsv01zx.asm
	zcc +zx -m --no-crt -lm -lp3 -pragma-define:CRT_ORG_CODE=0xc000 -pragma-define:CRT_ORG_DATA=0 -pragma-define:CRT_ORG_BSS=0x8000 -pragma-define:CLIB_MALLOC_HEAP_SIZE=0 -pragma-define:CRT_ON_EXIT=0x10002 -pragma-define:CRT_ENABLE_COMMANDLINE=2 -D__DISABLE_BUILTIN -U__STDC_VERSION__ plus3dos.asm qcsv01zx.asm -o qcsv01zx.ovl
	mv qcsv01zx_code_compiler.bin qcsv01zx.ovl
	dd if=/dev/zero bs=1 count=`ls -nl qcsv01zx.ovl | awk '{print 16384 - $$5}'` >> qcsv01zx.ovl

fputc_cons_first.bin: fputc_cons.asm
	z88dk-z80asm -b -split-bin fputc_cons.asm

atexit.bin: atexit.asm pager.bin
	z88dk-z80asm -b atexit.asm

pager.bin: pager.asm page2page.bin pages.inc lookupTable.inc data.bin functions.inc defines.inc
	z88dk-z80asm -b -r=49152 pager.asm
	z88dk-z80asm -m -b -r=`ls -nl pager.bin | awk '{print 48352 - $$5}'` pager.asm
	-awk '{print $$1, "equ", $$3}' pager.map > pager.inc

page2page.bin: page2page.asm
	z88dk-z80asm -m -b page2page.asm
	-awk '{print $$1, "equ", $$3}' page2page.map > page2page.inc

pages.inc:
	cp ../common/pages.txt pages.inc

lookupTable.inc:
	cp ../common/lookupTable.txt lookupTable.inc

data.bin:
	z88dk-z80asm -b -o=data.bin ../stdio.asm

functions.inc:
	cp ../common/functions.txt functions.inc

defines.inc:
	cp ../common/defines.txt defines.inc

clean:
	rm -f *.inc *.o *.map *.bin zcc_opt.def qcsv01zx.asm qcsv01zx.ovl
