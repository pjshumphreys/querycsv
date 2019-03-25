#!/bin/sh
export PATH=${PATH}:${HOME}/z88dk/bin
export ZCCCFG=${HOME}/z88dk/lib/config
z80asm -b querycsv.asm
printf '1 REM' > querycsv.bas 
xxd -ps -u -c 100000 querycsv.bin | sed -e "s/\(..\)/\{\1\}/g" >> querycsv.bas
printf '2 CLEAR VAL "48896":RANDOMIZE USR VAL "{BE}23635+256*{BE}23636+7"\n' >> querycsv.bas
./bas2tap -a2 -squerycsv querycsv.bas querycsv.tap
