#!/bin/sh
export PATH=${PATH}:${HOME}/z88dk/bin
export ZCCCFG=${HOME}/z88dk/lib/config
z80asm -b --split-bin fputs_con.asm
z80asm -b -s pager.asm
z80asm -b -s page2page.asm
dd conv=notrunc if=page2page.bin of=pager_part2.bin
z80asm -b -o=qrycsv04.ovl residos128.asm
dd if=/dev/zero bs=1 count=$((16384-`wc -c < qrycsv04.ovl`)) >> qrycsv04.ovl