#Use watcom to compile. This isn't really a fully formed makefile, but the source files here are mostly generated anyhow so this is only what we need to compile the app
# add /d3 for debugging symbols
OBJECTS = *.c
qrycsv16.exe: $(OBJECTS)
	wcl /zm @direct.cfg /ml /zt1900 /dMICROSOFT=1 /fpc /0 /os /k3000 /fe=qrycsv16 $(OBJECTS)
	c:\upx\upx --8086 .\qrycsv16.exe
