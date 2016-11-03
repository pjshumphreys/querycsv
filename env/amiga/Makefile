VBCC = /opt/vbcc
CC = export VBCC=$(VBCC); export PATH=$(VBCC)/bin:$(PATH); vc
CFLAGS =
SOURCES = lexer.c sql.c hash1.c hash2.c hash3.c hash4.c querycsv.c amiga.c
OBJECTS = $(SOURCES:%.c=%.o)
INCFLAGS =
LDFLAGS = 
LIBS = +kick13 -lamiga -lmsoft

all: QueryCSV

QueryCSV: $(SOURCES)
	$(CC) $(LIBS) -o QueryCSV $(SOURCES)

clean:
	rm -f QueryCSV

.PHONY: all
.PHONY: clean
