
name = banking-test

$(name).crt: $(name).bin
	bin2efcrt $(name).bin $(name).crt

$(name).bin: $(name).s
	acme -o $(name).bin $(name).s

.PHONY: clean
clean:
	rm -f $(name).bin $(name).crt
