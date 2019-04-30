#!/bin/sh
export PATH=${PATH}:${HOME}/z88dk/bin
export ZCCCFG=${HOME}/z88dk/lib/config
z80asm -b querycsv.asm
printf '1 CLEAR VAL "48047":RANDOMIZE USR VAL "{BE}23635+256*{BE}23636+47"\n2 REM' > querycsv.bas
xxd -ps -u -c 100000 querycsv.bin | sed -e "s/\(..\)/\{\1\}/g" >> querycsv.bas
./bas2tap -a1 -squerycsv querycsv.bas querycsv.tap
