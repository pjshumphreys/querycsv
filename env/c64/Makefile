
# do not remove intermediate targets
.SECONDARY:

.PHONY: all
all: querycsv.crt

###############################################################################
# Poor man's dependencies: Let all files depend from all header files
#

###############################################################################
# create a crt image from a binary image
#
querycsv.crt: $(wildcard ./*.h) $(wildcard ./*.c) crt0.s
	node ./build-c64.js
	rm -f crt0.o floatlib/float.o

.PHONY: clean
clean:
	rm -rf build querycsv.crt crt0.o floatlib/float.o

