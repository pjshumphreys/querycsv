port1a equ 0x7ffd  ; address of ROM/RAM switching port in I/O map
bankm2 equ 0x5b5c  ; system variable that holds the last value output to 7FFDh

org 0xbd00
  push bc
  push af
  di
  ld a, (bankm2)  ; get current switch state
  and 0xf8  ; also want RAM page
  ld b, a
  pop af
  add b
  ld (bankm2), a  ; update the system variable (very important)
  ld bc, port1a
  out (c), a
  pop bc

Loop2:
  pop de
  ld (hl), e
  dec hl
  ld (hl), d
  dec hl
  djnz Loop2
  dec c
  jr nz, Loop2

  ld a, (bankm2)  ; get current switch state
  and 0xf8  ; also want RAM page
  ld b, a
  pop af
  add b
  ld (bankm2), a  ; update the system variable (very important)
  ld bc, port1a
  out (c), a
  ei

lastCall:
  jp 0x0000
frund: