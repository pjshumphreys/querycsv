#Use watcom to compile. This isn't really a fully formed makefile, but the source files here are mostly generated anyhow so this is only what we need to compile the app
# add /d3 for debugging symbols
OBJECTS = lexer.c sql.c hash1.c hash2.c hash3.c hash4.c querycsv.c
querycsv.exe: $(OBJECTS)
	wcl /ml /zt4000 /dMICROSOFT=1 /fpc /0 /os /fe=querycsv $(OBJECTS)
