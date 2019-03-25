port1 equ 0x7ffd  ; address of ROM/RAM switching port in I/O map
bankm equ 0x5b5c  ; system variable that holds the last value output to 7FFDh

org 0xc000-52
  push bc
  push af
  di
  ld a, (bankm)  ; get current switch state
  and 0xf8  ; also want RAM page 0
  ld b, a
  pop af
  add b
  ld (bankm), a  ; update the system variable (very important)
  ld bc, port1
  out (c), a
  ei
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

  di
  ld a, (bankm)  ; get current switch state
  and 0xf8  ; also want RAM page 0
  ld b, a
  pop af
  add b
  ld (bankm), a  ; update the system variable (very important)
  ld bc, port1
  out (c), a
  ei
  jp 0x0000
