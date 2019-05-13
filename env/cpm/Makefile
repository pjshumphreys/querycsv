SHELL := /bin/bash
export PATH:=$(PATH):$(HOME)/z88dk/bin
export ZCCCFG:=$(HOME)/z88dk/lib/config

# do not remove intermediate targets
.SECONDARY:

.PHONY: all
all: querycsv.com

###############################################################################
# Poor man's dependencies: Let all files depend from all header files
#

###############################################################################
# create a crt image from a binary image
#

querycsv.com: querycsv.asm
	z80asm -b -o=querycsv.com querycsv.asm

.PHONY: clean
clean:
	rm -rf querycsv.o querycsv.bin querycsv.com