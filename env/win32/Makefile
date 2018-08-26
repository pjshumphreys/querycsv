#Use watcom to compile. This isn't really a fully formed makefile, but the source files here are mostly generated anyhow so this is only what we need to compile the app
OBJECTS = *.c
querycsv.exe: $(OBJECTS)
	wcl386 -bcl=nt /dMICROSOFT=1 /dWINDOWS=1 /fe=querycsv /os $(OBJECTS)
	c:\upx\upx .\querycsv.exe
