export VBCC=/opt/vbcc
export PATH=$VBCC/bin:$PATH
vc +kick13 -lamiga -o QueryCSV lexer.c sql.c hash1.c hash2.c hash3.c hash4.c querycsv.c test.c
