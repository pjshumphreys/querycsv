rem wcl /ml /zt4000 /dMICROSOFT=1 /fe=querycsv /fpc /0 /os lexer.c sql.c strcomp.c
wcl386 -bcl=nt /dMICROSOFT=1 /dWINDOWS=1 /fe=wquerycs /os lexer.c sql.c hash4.c querycsv.c win32.c
rem wcl /ml /zt4000 /dMICROSOFT=1 /d3 /fe=querycsv /fpc /0 strcomp.c
rem wcl386 -bcl=nt /dWINDOWS=1 /fe=wintest /os win32.c wintest2.c
rem wcl386 -bcl=nt /fe=cmdtest cmdtest.c win32.c