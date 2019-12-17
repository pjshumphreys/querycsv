SHELL := /bin/bash
export PATH:=$(PATH):$(HOME)/z88dk/bin
export ZCCCFG:=$(HOME)/z88dk/lib/config

# do not remove intermediate targets
.SECONDARY:

.PHONY: all
all: querycsv.tap qrycsv03.ovl

###############################################################################
# Poor man's dependencies: Let all files depend from all header files
#

###############################################################################
# create a crt image from a binary image
#

querycsv.bin: querycsv.asm
	z80asm -b querycsv.asm

querycsv.bas: querycsv.bin
	printf '10 CLEAR VAL "46000"\n20 RANDOMIZE USR VAL "{BE}23635+256*{BE}23636+51"\n30 REM' > querycsv.bas
	xxd -ps -u -c 100000 querycsv.bin | sed -e "s/\(..\)/\{\1\}/g" >> querycsv.bas

querycsv.tap: querycsv.bas
	./bas2tap -a10 -squerycsv querycsv.bas querycsv.tap

#qrycsv01.ovl:

#qrycsv02.ovl:

qrycsv03.ovl: libc.c crt0.asm residos128/atexit.asm residos128/fputc_cons.asm residos128/page2page.asm residos128/residos128.asm residos128/pager.asm
	z80asm -b --split-bin residos128/fputc_cons.asm
	printf 'origin equ 0xc000' > residos128/part1.inc
	z80asm -b residos128/pager.asm
	printf 'origin equ 0xbd00-0x' > residos128/part1.inc
	printf "%x\n" `stat -c "%s" residos128/pager_part1.bin` >> residos128/part1.inc
	z80asm -b -m residos128/pager.asm
	z80asm -b -m residos128/page2page.asm
	z80asm -b -m residos128/atexit.asm
	sed -i '/^__/d' residos128/page2page.map
	dd conv=notrunc if=residos128/page2page.bin of=residos128/pager_part2.bin
	zcc +zx -DRESIDOS -lp3 -lm -m --no-crt residos128/residos128.asm libc.c -bn qrycsv03.ovl
	dd if=/dev/zero bs=1 count=$$((16384-`wc -c < qrycsv03.ovl`)) >> qrycsv03.ovl

#qrycsv04.ovl:
#  zcc +zx -DRESIDOS -U__STDC_VERSION__ -lp3 -lm -m --no-crt querycsv.c strtod.c -S

#qrycsv05.ovl:

#qrycsv06.ovl:

.PHONY: clean
clean:
	cd residos128 && find -not -name "*.asm" -delete
	rm -rf querycsv.o querycsv.bin querycsv.bas querycsv.tap qrycsv03.ovl

