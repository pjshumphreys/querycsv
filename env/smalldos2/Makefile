#Use watcom to compile. This isn't really a fully formed makefile, but the source files here are mostly generated anyhow so this is only what we need to compile the app
# add /d3 for debugging symbols
OBJECTS = *.c
qrycsv16.exe: pager.asm libc.c dos.asm
	wcl /dMICROSOFT=1 /dDOS_DAT=1 /ms /fpc /0 /s /fm=qrycsv16 /fe=qrycsv16 pager.asm libc.c

dos.asm: hash2dat.asm hash3.asm hash4a.asm hash4b.asm hash4c.asm lexer.asm querycsv.asm sql.asm
	wcc /dMICROSOFT=1 /dDOS_DAT=1 /ms /fpc /0 /ot dos.c
	wdis -s -a -l=dos.asm dos.obj
	#c:\upx\upx --8086 .\qrycsv16.exe

hash2dat.asm: $(OBJECTS)
	wcc /dMICROSOFT=1 /dDOS_DAT=1 /ms /fpc /0 /ot /ob hash2dat.c
	wdis -s -a -l=hash2dat.asm hash2dat.obj
	#c:\upx\upx --8086 .\qrycsv16.exe

hash3.asm: $(OBJECTS)
	wcc /dMICROSOFT=1 /dDOS_DAT=1 /ms /fpc /0 /ot /ob hash3.c
	wdis -s -a -l=hash3.asm hash3.obj
	#c:\upx\upx --8086 .\qrycsv16.exe

hash4a.asm: $(OBJECTS)
	wcc /dMICROSOFT=1 /dDOS_DAT=1 /ms /fpc /0 /ot /ob hash4a.c
	wdis -s -a -l=hash4a.asm hash4a.obj
	#c:\upx\upx --8086 .\qrycsv16.exe

hash4b.asm: $(OBJECTS)
	wcc /dMICROSOFT=1 /dDOS_DAT=1 /ms /fpc /0 /ot /ob hash4b.c
	wdis -s -a -l=hash4b.asm hash4b.obj
	#c:\upx\upx --8086 .\qrycsv16.exe

hash4c.asm: $(OBJECTS)
	wcc /dMICROSOFT=1 /dDOS_DAT=1 /ms /fpc /0 /ot /ob hash4c.c
	wdis -s -a -l=hash4c.asm hash4c.obj
	#c:\upx\upx --8086 .\qrycsv16.exe

lexer.asm: $(OBJECTS)
	wcc /dMICROSOFT=1 /dDOS_DAT=1 /ms /fpc /0 /ot /ob lexer.c
	wdis -s -a -l=lexer.asm lexer.obj
	#c:\upx\upx --8086 .\qrycsv16.exe

sql.asm: $(OBJECTS)
	wcc /dMICROSOFT=1 /dDOS_DAT=1 /ms /fpc /0 /ot /ob sql.c
	wdis -s -a -l=sql.asm sql.obj
	#c:\upx\upx --8086 .\qrycsv16.exe

querycsv.asm: $(OBJECTS)
	wcc /dMICROSOFT=1 /dDOS_DAT=1 /ms /fpc /0 /ot /ob querycsv.c
	wdis -s -a -l=querycsv.asm querycsv.obj
	#c:\upx\upx --8086 .\qrycsv16.exe

