#   File:       QueryCSV.make
#   Target:     QueryCSV
#   Created:    Sunday, September 25, 2016 11:29:18 PM


MAKEFILE        = QueryCSV.make
�MondoBuild�    = {MAKEFILE}  # Make blank to avoid rebuilds when makefile is modified

ObjDir          = :
Includes        =

Sym-68K         = -sym off

AOptions        = {Includes} {Sym-68K} -model far
COptions        = {Includes} {Sym-68K} -model far -opt space


### Source Files ###

SrcFiles        =  �
          hash2.c �
          hash3.c �
          hash4a.c �
          hash4b.c �
          hash4c.c �
          lexer.c �
          sql.c �
          querycsv.c �
          mac.c �
          TEGlue.a

RezFiles        =  �
          mac.r �
          size.r


### Object Files ###

ObjFiles-68K    =  �
          "{ObjDir}hash2.c.a.o" �
          "{ObjDir}hash3.c.b.o" �
          "{ObjDir}hash4a.c.c.o" �
          "{ObjDir}hash4b.c.d.o" �
          "{ObjDir}hash4c.c.e.o" �
          "{ObjDir}lexer.c.f.o" �
          "{ObjDir}sql.c.g.o" �
          "{ObjDir}querycsv.c.h.o" �
          "{ObjDir}mac.c.o" �
          "{ObjDir}TEGlue.a.o"


### Libraries ###

LibFiles-68K    =  �
          "{Libraries}MathLib.far.o" �
          "{CLibraries}StdCLib.far.o" �
          "{Libraries}MacRuntime.o" �
          "{Libraries}IntEnv.far.o" �
          "{Libraries}ToolLibs.far.o" �
          "{Libraries}Interface.o" �
          "{Libraries}AEObjectSupportLib.o"


### Default Rules ###

.a.o  �  .a  {�MondoBuild�}
  {Asm} {depDir}{default}.a -o {targDir}{default}.a.o {AOptions}

.c.o  �  .c  {�MondoBuild�}
  {C} {depDir}{default}.c -o {targDir}{default}.c.o {COptions}

.c.a.o  �  .c  {�MondoBuild�}
  {C} {depDir}{default}.c -o {targDir}{default}.c.a.o {COptions} -seg sega

.c.b.o  �  .c  {�MondoBuild�}
  {C} {depDir}{default}.c -o {targDir}{default}.c.b.o {COptions} -seg segb

.c.c.o  �  .c  {�MondoBuild�}
  {C} {depDir}{default}.c -o {targDir}{default}.c.c.o {COptions} -seg segc

.c.d.o  �  .c  {�MondoBuild�}
  {C} {depDir}{default}.c -o {targDir}{default}.c.d.o {COptions} -seg segd

.c.e.o  �  .c  {�MondoBuild�}
  {C} {depDir}{default}.c -o {targDir}{default}.c.e.o {COptions} -seg sege

.c.f.o  �  .c  {�MondoBuild�}
  {C} {depDir}{default}.c -o {targDir}{default}.c.f.o {COptions} -seg segf

.c.g.o  �  .c  {�MondoBuild�}
  {C} {depDir}{default}.c -o {targDir}{default}.c.g.o {COptions} -seg segg

.c.h.o  �  .c  {�MondoBuild�}
  {C} {depDir}{default}.c -o {targDir}{default}.c.h.o {COptions} -seg segh


### Build Rules ###

QueryCSV  ��  {ObjFiles-68K} {LibFiles-68K} {�MondoBuild�}
  ILink �
    -o {Targ} �
    {ObjFiles-68K} �
    {LibFiles-68K} �
    {Sym-68K} �
    -mf -d �
    -t 'APPL' �
    -c 'qcsv' �
    -model far �
    -state rewrite �
    -compact -pad 0
  If "{Sym-68K}" =~ /-sym �[nNuU]�/
    ILinkToSYM {Targ}.NJ -mf -sym 3.2 -c 'sade'
  End
  SetFile {Targ} -a b

QueryCSV  ��  {�MondoBuild�}
  Rez -rd {RezFiles} -o {Targ} -append



### Required Dependencies ###

"{ObjDir}hash2.c.a.o"  �  hash2.c
"{ObjDir}hash3.c.b.o"  �  hash3.c
"{ObjDir}hash4a.c.c.o"  �  hash4a.c
"{ObjDir}hash4b.c.d.o"  �  hash4b.c
"{ObjDir}hash4c.c.e.o"  �  hash4c.c
"{ObjDir}lexer.c.f.o"  �  lexer.c
"{ObjDir}sql.c.g.o"  �  sql.c
"{ObjDir}querycsv.c.h.o"  �  querycsv.c
"{ObjDir}mac.c.o"  �  mac.c
"{ObjDir}TEGlue.a.o"  �  TEGlue.a


### Optional Dependencies ###
### Build this target to generate "include file" dependencies. ###

Dependencies  �  $OutOfDate
  MakeDepend �
    -append {MAKEFILE} �
    -ignore "{CIncludes}" �
    -ignore "{AIncludes}" �
    -objdir "{ObjDir}" �
    -objext .o �
    {Includes} �
    {SrcFiles}