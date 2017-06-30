#   File:       Makefile
#   Target:     QueryCSV
#   Created:    Sunday, September 25, 2016 11:29:18 PM


MAKEFILE        = Makefile
�MondoBuild�    = {MAKEFILE}  # Make blank to avoid rebuilds when makefile is modified

ObjDir          = :
Includes        = 

Sym-PPC         = -sym on

PPCCOptions     = {Includes} {Sym-PPC} -w 18 -d TARGET_API_MAC_CARBON


### Source Files ###

SrcFiles        =  �
				  lexer.c �
				  sql.c �
				  hash1.c �
				  hash2.c �
				  hash3.c �
				  hash4a.c �
				  hash4b.c �
				  hash4c.c �
				  querycsv.c �
				  powermac.c

RezFiles        =  �
				  powermac.r �
				  carbon.r �
				  size.r

### Object Files ###

ObjFiles-PPC    =  �
				  "{ObjDir}lexer.c.x" �
				  "{ObjDir}sql.c.x" �
				  "{ObjDir}hash1.c.x" �
				  "{ObjDir}hash2.c.x" �
				  "{ObjDir}hash3.c.x" �
				  "{ObjDir}hash4a.c.x" �
				  "{ObjDir}hash4b.c.x" �
				  "{ObjDir}hash4c.c.x" �
				  "{ObjDir}querycsv.c.x" �
				  "{ObjDir}powermac.c.x"


### Libraries ###

LibFiles-PPC    =  �
				  "{PPCLibraries}CarbonStdCLib.o" �
				  "{PPCLibraries}StdCRuntime.o" �
				  "{PPCLibraries}PPCCRuntime.o" �
				  "{PPCLibraries}PPCToolLibs.o" �
				  "{SharedLibraries}CarbonLib" �
				  "{SharedLibraries}StdCLib" �
				  "{SharedLibraries}MathLib"


### Default Rules ###

.c.x  �  .c  {�MondoBuild�}
	{PPCC} {depDir}{default}.c -o {targDir}{default}.c.x {PPCCOptions}


### Build Rules ###
all � QueryCSV

QueryCSV  ��  {ObjFiles-PPC} {LibFiles-PPC} {�MondoBuild�}
	PPCLink �
		-o {Targ} �
		{ObjFiles-PPC} �
		{LibFiles-PPC} �
		{Sym-PPC} �
		-mf -d �
		-t APPL �
		-c qcsv

QueryCSV  ��  {�MondoBuild�}
	Rez -rd {RezFiles} -o {Targ} -append


### Required Dependencies ###

"{ObjDir}lexer.c.x"  �  lexer.c
"{ObjDir}sql.c.x"  �  sql.c
"{ObjDir}hash1.c.x"  �  hash1.c
"{ObjDir}hash2.c.x"  �  hash2.c
"{ObjDir}hash3.c.x"  �  hash3.c
"{ObjDir}hash4a.c.x"  �  hash4a.c
"{ObjDir}hash4b.c.x"  �  hash4b.c
"{ObjDir}hash4c.c.x"  �  hash4c.c
"{ObjDir}querycsv.c.x"  �  querycsv.c
"{ObjDir}powermac.c.x"  �  powermac.c


### Optional Dependencies ###
### Build this target to generate "include file" dependencies. ###

Dependencies  �  $OutOfDate
	MakeDepend �
		-append {MAKEFILE} �
		-ignore "{CIncludes}" �
		-objdir "{ObjDir}" �
		-objext .x �
		{Includes} �
		{SrcFiles}

Clean		�	$OutOfDate
	Delete -i -y �
		{ObjFiles-PPC} �
		QueryCSV
