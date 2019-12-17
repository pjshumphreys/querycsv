port1 equ 0x7ffd  ; address of ROM/RAM switching port in I/O map
bankm equ 0x5b5c  ; system variable that holds the last value output to 7FFDh
PROG equ 0x5c53
offset equ 49

;prevent assembly code from being listed
defb 0x0d
defb 0xff

;ensure bank 0 is selected
ld a, (bankm)  ; Previous value of port
and 0xf8  ; Select bank 0
ld bc, port1
di
ld (bankm), a
out (c), a
ei

;memcpy data
ld hl, (PROG)
ld de, dataStart+offset
add hl, de
ld de, 0xC000
ld bc, dataEnd-dataStart
ldir

;randomize usr 49155
jp 0xc003

dataStart:
 binary "hello.bin"
dataEnd: