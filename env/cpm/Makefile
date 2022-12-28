all: querycsv.com output output8080

querycsv.com: launcher2.bin launcher.c launcher2.map
	printf "PUBLIC _chooseCLib\n_chooseCLib:\nBINARY \"launcher2.bin\"\nPUBLIC _newStack\n" > launcher2.asm
	grep _newStack launcher2.map >> launcher2.asm
	zcc +cpm -lndos -m -o querycsv.com launcher.c launcher2.asm

launcher2.bin: launcher2.com
	z88dk-zx7 ./launcher2.com
	mv launcher2.com.zx7 launcher2.bin

launcher2.com: launcher2.c
	zcc +cpm -lm -pragma-define:CRT_ORG_CODE=16384 -m -o launcher2.com launcher2.c

qrycsv1a.ovl: pager8080.asm libc.c
	zcc +cpm -clib=8080 --math-mbf32_8080 -m -D__8080__ -U__STDC_VERSION__ -pragma-define:CRT_ENABLE_COMMANDLINE=0 -pragma-define:CLIB_EXIT_STACK_SIZE=3 -pragma-define:CLIB_OPEN_MAX=8 -o qrycsv1a.ovl pager8080.asm libc.c
# zcc +cpm -clib=8080 --math-mbf32_8080 -S -D__8080__ -U__STDC_VERSION__ -pragma-define:CRT_ENABLE_COMMANDLINE=0 -pragma-define:CLIB_EXIT_STACK_SIZE=3 -pragma-define:CLIB_OPEN_MAX=8 --c-code-in-asm -o qrycsv1a.asm pager8080.asm libc.c

qrycsv01.ovl: pager.asm libc.c
	zcc +cpm -lm -m -D__DISABLE_BUILTIN -U__STDC_VERSION__ -pragma-define:CRT_ENABLE_COMMANDLINE=0 -pragma-define:CLIB_EXIT_STACK_SIZE=3 -pragma-define:CLIB_OPEN_MAX=8 -o qrycsv01.ovl pager.asm libc.c
# zcc +cpm -lm -S -D__DISABLE_BUILTIN -U__STDC_VERSION__ -pragma-define:CRT_ENABLE_COMMANDLINE=0 -pragma-define:CLIB_EXIT_STACK_SIZE=3 -pragma-define:CLIB_OPEN_MAX=8 --c-code-in-asm -o qrycsv01.asm pager.asm libc.c

qrycsv02.ovl: pager.asm libc.c
	zcc +msx -subtype=msxdos2 -lm -m -D__DISABLE_BUILTIN -U__STDC_VERSION__ -pragma-define:CRT_ENABLE_COMMANDLINE=0 -pragma-define:CLIB_EXIT_STACK_SIZE=3 -pragma-define:CLIB_OPEN_MAX=8 -D__Z88DK -o qrycsv02.ovl pager.asm libc.c
# zcc +msx -subtype=msxdos2 -lm -S -D__DISABLE_BUILTIN -U__STDC_VERSION__ -pragma-define:CRT_ENABLE_COMMANDLINE=0 -pragma-define:CLIB_EXIT_STACK_SIZE=3 -pragma-define:CLIB_OPEN_MAX=8 -D__Z88DK --c-code-in-asm -o qrycsv02.asm pager.asm libc.c

output: querycsv.com pager.asm libc.c lexer2.c sql2.c querycsv.c build-cpm.js
	node ./build-cpm.js
	cp qrycsv00.bin ./output/qrycsv00.ovl
	cp querycsv.com build/obj2/*.ovl ./output

output8080: querycsv.com pager.asm libc.c lexer2.c sql2.c querycsv.c build-cpm8080.js
	node ./build-cpm8080.js
	cp qrycsv00.bin ./output8080/qrycsv00.ovl
	cp build/8080/obj2/*.ovl ./output8080
	mv ./output8080/qrycsv1a.ovl ./output8080/querycsv.com

clean:
	rm -rf build output output8080 zcc_opt.def launcher2.bin launcher2.com launcher2.asm querycsv.com *.map *.sym *.ovl
