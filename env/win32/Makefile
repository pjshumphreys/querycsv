#Use watcom to compile. This isn't really a fully formed makefile, but the source files here are mostly generated anyhow so this is only what we need to compile the app
OBJECTS = lexer.c sql.c hash1.c hash2.c hash3.c hash4.c querycsv.c win32.c
qcsv_w32.exe: $(OBJECTS)
	wcl386 -bcl=nt /dMICROSOFT=1 /dWINDOWS=1 /fe=qcsv_w32 /os $(OBJECTS)
