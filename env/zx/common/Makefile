all: querycsv archiver residos.tap plus3dos.tap esxdos.tap

querycsv: querycsv.bas
	./zmakebas -a 10 -n querycsv -p -o querycsv querycsv.bas

querycsv.tap: querycsv.bas
	./zmakebas -a 10 -n querycsv -o querycsv.tap querycsv.bas

printLn.bin: printLn.asm
	z88dk-z80asm -b -r=49152 printLn.asm
	z88dk-z80asm -m -b -r=`ls -nl printLn.bin | awk '{print 49152 - $$5}'` printLn.asm
	-awk '{print $$1, "equ", $$3}' printLn.map > equs.inc

fputc_cons_rst.bin: printLn.bin fputc_cons_rst.asm
	z88dk-z80asm -b fputc_cons_rst.asm

dodos_plus3.bin: printLn.bin dodos_plus3.asm
	z88dk-z80asm -b dodos_plus3.asm

dodos_residos.bin: printLn.bin dodos_residos.asm
	z88dk-z80asm -b dodos_residos.asm

dosload.bin: printLn.bin dosload.asm
	z88dk-z80asm -b dosload.asm

dosload_esxdos.bin: printLn.bin dosload_esxdos.asm
	z88dk-z80asm -b dosload_esxdos.asm

querycsv2.bin: querycsv2.asm fputc_cons_rst.bin dodos_plus3.bin dodos_residos.bin dosload.bin dosload_esxdos.bin
	z88dk-z80asm -b querycsv2.asm

querycsv.bin: querycsv2.bin querycsv.asm
	z88dk-z80asm -b querycsv.asm

querycsv.bas: querycsv.bin
	printf '10 CLEAR VAL "46000"\n20 RANDOMIZE USR VAL "\\{0xBE}23635+256*\\{0xBE}23636+51"\n30 REM' > querycsv.bas
	xxd -ps -u -c 100000 querycsv.bin | sed -e "s/\(..\)/\\\{0x\1\}/g" >> querycsv.bas

archiver: archiver.c
	gcc -o archiver archiver.c

residos.tap: unarchiver.c
	zcc +zx -DRESIDOS -lp3 -pragma-define:CRT_ON_EXIT=0x10002 -DAMALLOC2 -o residos -create-app unarchiver.c

plus3dos.tap: unarchiver.c
	zcc +zx -O0 -lp3 -pragma-define:CRT_ON_EXIT=0x10002 -DAMALLOC2 -o plus3dos -create-app unarchiver.c

esxdos.tap: unarchiver.c
	zcc +zx -lesxdos -pragma-define:CRT_ON_EXIT=0x10002 -DAMALLOC2 -o esxdos -create-app unarchiver.c

clean:
	rm -f querycsv.bas querycsv querycsv.tap residos.tap plus3dos.tap esxdos.tap archiver *.bin *.o *.inc *.map
