#Use watcom to compile. This isn't really a fully formed makefile, but the source files here are mostly generated anyhow so this is only what we need to compile the app
# add /d3 for debugging symbols
OBJECTS = *.c
qrycsv16.exe: $(OBJECTS)
	wcl /dMICROSOFT=1 /dDOS_DAT=1 /ms /fpc /0 /s /fm=qrycsv16 /fe=qrycsv16 pager.asm libc.c
	#wdis -s -a -l dos.obj
	#c:\upx\upx --8086 .\qrycsv16.exe
