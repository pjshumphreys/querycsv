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
di
ldir
ei

;identify whether 128k of main memory is present and store the result for later
ld hl, (PROG)
ld de, checkmem+offset
add hl, de
call 0xc007 ; jp (hl)
ld (0xc006), a ; memoryType

;randomize usr 49155
jp 0xc003

checkmem:
  ld bc, port1  ; the horizontal ROM switch/RAM switch I/O address

  di
  ld a, (bankm)  ; system variable that holds current switch state
  or 23
  ld (bankm), a  ; must keep system variable up to date (very important)
  out (c), a
  ei

  ld a, (0xc000)
  cp 0xc3
  jr z, noram

  ; copy the character set up into page 7 so we can print using page 7 and use page 5 for data
  ld hl, 0x3d00  ; Pointer to the source
  ld de, 0xe800  ; Pointer to the destination
  ld bc, 0x2ff  ; Number of bytes to move
  di
  ldir
  ei

  ld bc, port1  ; the horizontal ROM switch/RAM switch I/O address

  di
  ld a, (bankm)  ; system variable that holds current switch state
  res 4, a  ; move right to left in horizontal ROM switch (3 to 2)
  ld (bankm), a  ; must keep system variable up to date (very important)
  out (c), a

  ld a, (0x0008)
  cp 0x50  ; 'P' of 'PLUS3DOS'

  ld a, (bankm)  ; get current switch state
  set 4, a  ; move left to right (ROM 2 to ROM 3)
  jr z, plus3
  and 0xf8  ; also want RAM page 0
  ld (bankm), a  ; update the system variable (very important)
  out (c), a
  ei
  ld a, 1  ; 128k, but no plus3 dos
  ret

noram:
  ld a, 0  ; 48k only
  ret

plus3:
  and 0xf8  ; also want RAM page 0
  ld (bankm), a  ; update the system variable (very important)
  out (c), a
  ei
  ld a, 2  ; 128k with plus3dos
  ret

dataStart:
  binary "querycsv2.bin"
dataEnd: