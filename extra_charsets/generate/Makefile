
## Created by Anjuta

CC = gcc

all: sjis.dat eucjp.dat

sjis.dat: waitFor gen2 gensjis
	./gensjis
	exo-open sjis.html
	./waitFor sjis.txt
	sh -c "perl -pe 's/[^�\n]*�[^\n]*\n//sg' < sjis.txt | ./gen2 sjis.dat"

eucjp.dat: waitFor gen2 geneucjp
	./geneucjp
	exo-open eucjp.html
	./waitFor eucjp.txt
	sh -c "perl -pe 's/[^�\n]*�[^\n]*\n//sg' < eucjp.txt | ./gen2 eucjp.dat"

gen2: gen2.c
	$(CC) -o gen2 gen2.c

waitFor: waitFor.c
	$(CC) -o waitFor waitFor.c

gensjis: gensjis.c
	$(CC) -o gensjis gensjis.c

geneucjp: geneucjp.c
	$(CC) -o geneucjp geneucjp.c

clean:
	rm -f *.txt *.html *.dat gen2 waitFor gensjis geneucjp

.PHONY: all
.PHONY: count
.PHONY: clean
