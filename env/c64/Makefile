
name = querycsv

$(name).crt: $(name).bin
	bin2efcrt $(name).bin $(name).crt

$(name).bin: linker.cfg $(name).o
	ld65 -C linker.cfg $(name).o

$(name).o: $(name).s
	ca65 $(name).s

.PHONY: clean
clean:
	rm -f $(name).bin $(name).o $(name).crt
