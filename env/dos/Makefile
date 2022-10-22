#Use watcom to compile. This isn't really a fully formed makefile, but the source files here are mostly generated anyhow so this is only what we need to compile the app
# add /d3 for debugging symbols
OBJECTS = *.c
qrycsv16.exe: $(OBJECTS)
	wcl @direct.cfg /dMICROSOFT=1 /ml /fpc /0 /os /zt2600 /fm=qrycsv16 /fe=qrycsv16 dos.c
	c:\upx\upx --8086 .\qrycsv16.exe
