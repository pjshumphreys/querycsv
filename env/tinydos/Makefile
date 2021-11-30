#Use watcom to compile. This isn't really a fully formed makefile, but the source files here are mostly generated anyhow so this is only what we need to compile the app
# add /d3 for debugging symbols
OBJECTS = *.c
qrycsv16.exe: $(OBJECTS)
	wasm bounce.asm
	wcl /zm @direct.cfg /mm /zt1900 /dMICROSOFT=1 /dDOS_DAT=1 /fpc /0 /os /k3000 /ep2 /ee /s /fe=qrycsv16 $(OBJECTS) bounce.obj
	#wdis -s -a -l querycsv.obj
