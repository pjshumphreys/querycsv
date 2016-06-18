rem wcl /ml /zt4000 /dMICROSOFT=1 /fe=querycsv /fpc /0 /os lexer.c sql.c hash4.c querycsv.c
rem wcl386 -bcl=nt /dMICROSOFT=1 /dWINDOWS=1 /fe=querycsv /os lexer.c sql.c hash4.c querycsv.c win32.c
wcl /ml /zt4000 /dMICROSOFT=1 /d3 /fe=querycsv /fpc /0 lexer.c sql.c hash4.c querycsv.c
rem wcl386 -bcl=nt /dWINDOWS=1 /fe=wintest /os win32.c wintest2.c
rem wcl386 -bcl=nt /fe=cmdtest cmdtest.c win32.c
