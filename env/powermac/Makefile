#   File:       Makefile
#   Target:     QueryCSV
#   Created:    Sunday, September 25, 2016 11:29:18 PM


MAKEFILE        = Makefile
¥MondoBuild¥    = {MAKEFILE}  # Make blank to avoid rebuilds when makefile is modified

ObjDir          = :
Includes        = 

Sym-68K         = -sym on
Sym-PPC         = -sym on

COptions        = {Includes} {Sym-68K} -model far -opt space
PPCCOptions     = {Includes} {Sym-PPC} -d TARGET_API_MAC_CARBON


### Source Files ###

SrcFiles        =  ¶
					lexer.c ¶
					sql.c ¶
					hash2out.c ¶
					hash2in0.c ¶
					hash2in1.c ¶
					hash2in2.c ¶
					hash2in3.c ¶
					hash2in4.c ¶
					hash2in5.c ¶
					hash2in6.c ¶
					hash2in7.c ¶
					hash2in8.c ¶
					hash2in9.c ¶
					hash2in10.c ¶
					hash2in11.c ¶
					hash2in12.c ¶
					hash2in13.c ¶
					hash2in14.c ¶
					hash2in15.c ¶
					hash3.c ¶
					hash4a.c ¶
					hash4b.c ¶
					hash4c.c ¶
					querycsv.c ¶
					powermac.c

RezFiles        =  ¶
					powermac.r ¶
					carbon.r ¶
					size.r

### Object Files ###

ObjFiles-68K    =  ¶
					"{ObjDir}mac.c.o"

ObjFiles-PPC    =  ¶
					"{ObjDir}lexer.c.x" ¶
					"{ObjDir}sql.c.x" ¶
					"{ObjDir}hash2out.c.x" ¶
					"{ObjDir}hash2in0.c.x" ¶
					"{ObjDir}hash2in1.c.x" ¶
					"{ObjDir}hash2in2.c.x" ¶
					"{ObjDir}hash2in3.c.x" ¶
					"{ObjDir}hash2in4.c.x" ¶
					"{ObjDir}hash2in5.c.x" ¶
					"{ObjDir}hash2in6.c.x" ¶
					"{ObjDir}hash2in7.c.x" ¶
					"{ObjDir}hash2in8.c.x" ¶
					"{ObjDir}hash2in9.c.x" ¶
					"{ObjDir}hash2in10.c.x" ¶
					"{ObjDir}hash2in11.c.x" ¶
					"{ObjDir}hash2in12.c.x" ¶
					"{ObjDir}hash2in13.c.x" ¶
					"{ObjDir}hash2in14.c.x" ¶
					"{ObjDir}hash2in15.c.x" ¶
					"{ObjDir}hash3.c.x" ¶
					"{ObjDir}hash4a.c.x" ¶
					"{ObjDir}hash4b.c.x" ¶
					"{ObjDir}hash4c.c.x" ¶
					"{ObjDir}querycsv.c.x" ¶
					"{ObjDir}powermac.c.x"

### Libraries ###
LibFiles-68K    =  ¶
					"{Libraries}MathLib.o" ¶
					"{CLibraries}StdCLib.o" ¶
					"{Libraries}MacRuntime.o" ¶
					"{Libraries}ToolLibs.o" ¶
					"{Libraries}Interface.o"

LibFiles-PPC    =  ¶
					"{PPCLibraries}CarbonStdCLib.o" ¶
					"{PPCLibraries}StdCRuntime.o" ¶
					"{PPCLibraries}PPCCRuntime.o" ¶
					"{SharedLibraries}CarbonLib" ¶
					"{SharedLibraries}StdCLib"



### Default Rules ###

.c.o  Ä  .c  {¥MondoBuild¥}
	{C} {depDir}{default}.c -o {targDir}{default}.c.o {COptions}

.c.x  Ä  .c  {¥MondoBuild¥}
	{PPCC} {depDir}{default}.c -o {targDir}{default}.c.x {PPCCOptions}


### Build Rules ###
all Ä QueryCSV.PPC ¶
		QueryCSV.68k ¶
		QueryCSV

QueryCSV.68k  ÄÄ  {ObjFiles-68K} {LibFiles-68K} {¥MondoBuild¥}
	ILink ¶
		-o {Targ} ¶
		{ObjFiles-68K} ¶
		{LibFiles-68K} ¶
		{Sym-68K} ¶
		-mf -d ¶
		-t 'APPL' ¶
		-c 'qcsv' ¶
		-model far ¶
		-state rewrite ¶
		-compact -pad 0
	If "{Sym-68K}" =~ /-sym Å[nNuU]Å/
		ILinkToSYM {Targ}.NJ -mf -sym 3.2 -c 'sade'
	End

QueryCSV.68k  ÄÄ  {¥MondoBuild¥}
	Rez -rd {RezFiles} -o {Targ} -append


QueryCSV.PPC  ÄÄ  {ObjFiles-PPC} {LibFiles-PPC} {¥MondoBuild¥}
	PPCLink ¶
		-o {Targ} -m __appstart ¶
		{ObjFiles-PPC} ¶
		{LibFiles-PPC} ¶
		{Sym-PPC} ¶
		-mf -d ¶
		-t APPL ¶
		-c qcsv

QueryCSV.PPC  ÄÄ  {¥MondoBuild¥}
	Rez -rd {RezFiles} -o {Targ} -append

QueryCSV  ÄÄ  QueryCSV.PPC QueryCSV.68k
	Duplicate -y QueryCSV.68k QueryCSV
	Mergefragment QueryCSV.PPC QueryCSV

### Required Dependencies ###

"{ObjDir}mac.c.o"  Ä  mac.c
"{ObjDir}lexer.c.x"  Ä  lexer.c
"{ObjDir}sql.c.x"  Ä  sql.c
"{ObjDir}hash2out.c.x"  Ä  hash2out.c
"{ObjDir}hash2in0.c.x"  Ä  hash2in0.c
"{ObjDir}hash2in1.c.x"  Ä  hash2in1.c
"{ObjDir}hash2in2.c.x"  Ä  hash2in2.c
"{ObjDir}hash2in3.c.x"  Ä  hash2in3.c
"{ObjDir}hash2in4.c.x"  Ä  hash2in4.c
"{ObjDir}hash2in5.c.x"  Ä  hash2in5.c
"{ObjDir}hash2in6.c.x"  Ä  hash2in6.c
"{ObjDir}hash2in7.c.x"  Ä  hash2in7.c
"{ObjDir}hash2in8.c.x"  Ä  hash2in8.c
"{ObjDir}hash2in9.c.x"  Ä  hash2in9.c
"{ObjDir}hash2in10.c.x"  Ä  hash2in10.c
"{ObjDir}hash2in11.c.x"  Ä  hash2in11.c
"{ObjDir}hash2in12.c.x"  Ä  hash2in12.c
"{ObjDir}hash2in13.c.x"  Ä  hash2in13.c
"{ObjDir}hash2in14.c.x"  Ä  hash2in14.c
"{ObjDir}hash3.c.x"  Ä  hash3.c
"{ObjDir}hash4a.c.x"  Ä  hash4a.c
"{ObjDir}hash4b.c.x"  Ä  hash4b.c
"{ObjDir}hash4c.c.x"  Ä  hash4c.c
"{ObjDir}querycsv.c.x"  Ä  querycsv.c
"{ObjDir}powermac.c.x"  Ä  powermac.c


### Optional Dependencies ###
### Build this target to generate "include file" dependencies. ###

Dependencies  Ä  $OutOfDate
	MakeDepend ¶
		-append {MAKEFILE} ¶
		-ignore "{CIncludes}" ¶
		-objdir "{ObjDir}" ¶
		-objext .x ¶
		{Includes} ¶
		{SrcFiles}

Clean  Ä $OutOfDate
	Delete -i -y ¶
		{ObjFiles-PPC} ¶
		QueryCSV
