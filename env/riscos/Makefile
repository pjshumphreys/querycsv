# Makefile for !QueryCSV

COMPONENT  = QueryCSV
TARGET     = !QueryCSV.querycsv
OBJS       = lexer sql hash2 hash3 hash4a hash4b hash4c querycsv riscos softfloat pow10 log10 finite dtoa strtod
CINCLUDES  = -ITCPIPLibs:,RISCOSLib:,tbox:,C: -j
LIBS       = ${EVENTLIB} ${TBOXLIB} ${WIMPLIB} ${RLIB} ${UNIXLIB}
INSTDIR    ?= <Install$Dir>
INSTAPP_FILES = !Boot !Run ${TARGET} !Sprites !Sprites22 Messages res


include CApp

C_WARNINGS = -fa

clean_all:: clean
        ${WIPE} ${INSTAPP} ${WFLAGS}

# Dynamic dependencies:
