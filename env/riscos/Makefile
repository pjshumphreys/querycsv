# Makefile for !QueryCSV

COMPONENT  = QueryCSV
TARGET     = !RunImage
OBJS       = querycsv
CINCLUDES  = -Itbox:,C:
LIBS       = ${EVENTLIB} ${TBOXLIB} ${WIMPLIB}
INSTDIR    ?= <Install$Dir>
INSTAPP_FILES = !Boot !Run ${TARGET} !Sprites !Sprites11 !Sprites22 Messages res

include CApp

C_WARNINGS = -fa

clean_all:: clean
        ${WIPE} ${INSTAPP} ${WFLAGS}

# Dynamic dependencies:
o.querycsv:	c.querycsv
#o.querycsv:	C:h.wimp
#o.querycsv:	C:h.toolbox
#o.querycsv:	C:h.kernel
#o.querycsv:	C:h.event
#o.querycsv:	C:h.wimplib
