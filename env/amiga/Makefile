VBCC = /opt/vbcc
CC = export VBCC=$(VBCC); export PATH=$(VBCC)/bin:$(PATH); vc
CFLAGS =
SOURCES = lexer.c sql.c hash2.c hash3.c hash4a.c hash4b.c hash4c.c querycsv.c amiga.c
OBJECTS = $(SOURCES:%.c=%.o)
INCFLAGS = -I$(VBCC)/NDK_3.9/Include/include_h
LDFLAGS = -L$(VBCC)/NDK_3.9/Include/linker_libs -DAMIGA
LIBS = +kick13 -lamiga -lm13

all: QueryCSV

QueryCSV: $(SOURCES)
	$(CC) $(INCFLAGS) $(LDFLAGS) $(LIBS) -o QueryCSV $(SOURCES)

clean:
	rm -f QueryCSV

.PHONY: all
.PHONY: clean
