gperf hash4.gperf > hash4.c
bison sql.y
flex sql.l
gcc -O1 -g sql.c lexer.c strcomp.c -o querycsv

