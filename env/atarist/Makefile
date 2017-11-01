VBCC = /opt/vbcc
CC = export VBCC=$(VBCC); export PATH=$(VBCC)/bin:$(PATH); vc
CFLAGS =
SOURCES = lexer.c sql.c hash2.c hash3.c hash4a.c hash4b.c hash4c.c querycsv.c atarist.c
OBJECTS = $(SOURCES:%.c=%.o)
INCFLAGS =
LDFLAGS = -DATARI
LIBS = +tos -lgem -lm

all: querycsv.ttp

querycsv.ttp: $(SOURCES)
	$(CC) $(INCFLAGS) $(LDFLAGS) $(LIBS) -o querycsv.ttp $(SOURCES)

clean:
	rm -f querycsv.ttp

.PHONY: all
.PHONY: clean
