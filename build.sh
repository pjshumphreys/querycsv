gperf hash4.gperf > hash4.c
bison sql.y
flex sql.l
gcc -O1 -g refhash.c parser.c lexer.c sql.c hash4.c strcomp.c querycsv.c -o querycsv

