
# do not remove intermediate targets
.SECONDARY:

name := querycsv

ld_config := linker.cfg

obj :=
obj += querycsv.o
obj += obj/c64.o

INCLUDE  := include

DEFINE   := -DEFVERSION=\"${version}\"

.PHONY: all
all: $(name).crt

###############################################################################
# Poor men's dependencies: Let all files depend from all header files
#
headers := $(wildcard $(INCLUDE)/*.h)

obj/%.s: %.c obj $(headers)
	cc65 -t c64 -T -O --static-locals -I $(INCLUDE) $(DEFINE) -o $@ $<

###############################################################################
obj/%.o: obj/%.s obj
	ca65 -t c64 -o $@ $<

###############################################################################
crt0.o: querycsv.s obj
	ca65 -t c64 -o $@ $<

###############################################################################
# create a crt image from a binary image
#
%.crt: %.bin
	bin2efcrt $< $@

###############################################################################
obj:
	mkdir -p $@

$(name).bin: $(obj) $(ld_config)
	ld65 -o $@ -m $@.map -C $(ld_config) $(obj) \
		-L /usr/local/lib/cc65/lib --lib c64.lib

.PHONY: clean
clean:
	rm -f $(name) crt0.o $(name).map $(name).bin $(name).crt
	rm -rf obj
