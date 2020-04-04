#Use watcom to compile. This isn't really a fully formed makefile, but the source files here are mostly generated anyhow so this is only what we need to compile the app
OBJECTS = *.c
querycsv.exe: $(OBJECTS)
	wcl386 /zm /3 @direct.cfg -bcl=nt /dMICROSOFT=1 /dWINDOWS=1 /fe=querycsv /os $(OBJECTS)
	wrc -bt=nt querycsv.rc querycsv.exe
	c:\upx\upx .\querycsv.exe
