#!/bin/sh
export PATH=${PATH}:${HOME}/z88dk/bin
export ZCCCFG=${HOME}/z88dk/lib/config
z80asm -b --split-bin fputs_con.asm
printf 'org 0xc000' > part1.inc
z80asm -b -s pager.asm
printf 'org 0xbd00-0x' > part1.inc
printf "%x\n" `stat -c "%s" pager_part1.bin` >> part1.inc
z80asm -b -s pager.asm
z80asm -b -s page2page.asm
dd conv=notrunc if=page2page.bin of=pager_part2.bin
z80asm -b -o=qrycsv05.ovl residos128.asm
dd if=/dev/zero bs=1 count=$((16384-`wc -c < qrycsv05.ovl`)) >> qrycsv05.ovl