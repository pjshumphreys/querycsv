#   File:       Makefile
#   Target:     QueryCSV
#   Created:    Sunday, July 24, 2016 09:13:29 PM


MAKEFILE        = Makefile
¥MondoBuild¥    = {MAKEFILE}  # Make blank to avoid rebuilds when makefile is modified

ObjDir          = :
Includes        = 

Sym-PPC         = -sym off
Sym-68K         = -sym on

PPCCOptions     = {Includes} {Sym-PPC} -opt size -d TARGET_API_MAC_CARBON=1 

COptions        = {Includes} {Sym-68K} -model near -opt space

### Source Files ###

SrcFiles        =  ¶
				  macos.c

RezFiles        =  ¶
				  macos.r ¶
				  carbon.r ¶
				  size.r

### Object Files ###

ObjFiles-PPC    =  ¶
				  "{ObjDir}macos.c.x"

ObjFiles-68K    =  ¶
				  "{ObjDir}macos.c.o"

### Libraries ###

LibFiles-PPC    =  ¶
				  "{SharedLibraries}CarbonLib" ¶
				  "{SharedLibraries}StdCLib" ¶
				  "{PPCLibraries}StdCRuntime.o" ¶
				  "{PPCLibraries}PPCCRuntime.o" ¶
				  "{PPCLibraries}PPCToolLibs.o" ¶
				  "{PPCLibraries}ThreadLibrary-PPC.o" ¶
				  "{PPCLibraries}PPCSIOW.o"

LibFiles-68K    =  ¶
				  "{Libraries}MathLib.o" ¶
				  "{CLibraries}StdCLib.o" ¶
				  "{Libraries}MacRuntime.o" ¶
				  "{Libraries}IntEnv.o" ¶
				  "{Libraries}ToolLibs.o" ¶
				  "{Libraries}Interface.o" ¶
				  "{Libraries}AEObjectSupportLib.o" ¶
				  "{Libraries}ThreadLibrary-68K.o" ¶
				  "{Libraries}SIOW.o"

### Default Rules ###

.c.x  Ä  .c  {¥MondoBuild¥}
	{PPCC} {depDir}{default}.c -o {targDir}{default}.c.x {PPCCOptions}

.c.o  Ä  .c  {¥MondoBuild¥}
	{C} {depDir}{default}.c -o {targDir}{default}.c.o {COptions}


### Build Rules ###

QueryCSV  ÄÄ  {ObjFiles-PPC} {LibFiles-PPC} {¥MondoBuild¥}
	PPCLink ¶
		-o {Targ} ¶
		{ObjFiles-PPC} ¶
		{LibFiles-PPC} ¶
		{Sym-PPC} ¶
		-mf -d ¶
		-m __appstart ¶
		-t 'APPL' ¶
		-c 'ttxt'

QueryCSV  ÄÄ  {ObjFiles-68K} {LibFiles-68K} {¥MondoBuild¥}
	ILink ¶
		-o {Targ} ¶
		{ObjFiles-68K} ¶
		{LibFiles-68K} ¶
		{Sym-68K} ¶
		-mf -d ¶
		-t 'APPL' ¶
		-c 'ttxt' ¶
		-ra AEObjSuppt=resPreload,resLocked ¶
		-ra AEObjGlue=resPreload,resLocked ¶
		-ra AEObjPacking=resPreload,resLocked ¶
		-model near ¶
		-state rewrite ¶
		-compact -pad 0
	If "{Sym-68K}" =~ /-sym Å[nNuU]Å/
		ILinkToSYM {Targ}.NJ -mf -sym 3.2 -c 'sade'
	End

QueryCSV  ÄÄ  {¥MondoBuild¥}
	Rez -rd {RezFiles} -o {Targ} -append

### Required Dependencies ###

"{ObjDir}macos.c.x" "{ObjDir}macos.c.o"  Ä  macos.c

### Optional Dependencies ###
### Build this target to generate "include file" dependencies. ###

Dependencies  Ä  $OutOfDate
	MakeDepend ¶
		-append {MAKEFILE} ¶
		-ignore "{CIncludes}" ¶
		-objdir "{ObjDir}" ¶
		-objext .x ¶
		-objext .o ¶
		{Includes} ¶
		{SrcFiles}
