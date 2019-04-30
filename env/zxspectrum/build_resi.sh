#!/bin/sh
export PATH=${PATH}:${HOME}/z88dk/bin
export ZCCCFG=${HOME}/z88dk/lib/config
z80asm -b --split-bin fputs_con.asm
printf 'origin equ 0xc000' > part1.inc
z80asm -b pager.asm
printf 'origin equ 0xbd00-0x' > part1.inc
printf "%x\n" `stat -c "%s" pager_part1.bin` >> part1.inc
z80asm -b -m pager.asm
z80asm -b -m page2page.asm
sed -i '/^__/d' page2page.map
dd conv=notrunc if=page2page.bin of=pager_part2.bin
z80asm -b -m -o=qrycsv03.ovl residos128.asm
dd if=/dev/zero bs=1 count=$((16384-`wc -c < qrycsv03.ovl`)) >> qrycsv03.ovl