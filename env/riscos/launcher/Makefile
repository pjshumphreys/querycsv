# Makefile for !QueryCSV

COMPONENT  = QueryCSV
TARGET     = ^.!QueryCSV.!RunImage
OBJS       = runimage
CINCLUDES  = -IRISCOSLib:,tbox:,C: -j
LIBS       = ${EVENTLIB} ${TBOXLIB} ${WIMPLIB} ${RLIB} ${UNIXLIB}
INSTDIR    ?= <Install$Dir>
INSTAPP_FILES = !Boot !Run ${TARGET} !Sprites !Sprites22 Messages res


include CApp

C_WARNINGS = -fa

clean_all:: clean
        ${WIPE} ${INSTAPP} ${WFLAGS}

# Dynamic dependencies:

