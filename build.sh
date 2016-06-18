gcc makeheaders.c -o makeheaders
./makeheaders ./split/*.c -h > gen.h
cat split/*.c >querycsv.c
gperf hash4.gperf > hash4.c
bison sql.y
flex sql.l
cc -Os sql.c lexer.c hash4.c querycsv.c -o querycsv

