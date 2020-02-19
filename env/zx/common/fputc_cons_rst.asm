include "equs.inc"

; (char to print) this function is written to only use jr instructions to make it be relocatable code
fputc_cons_rom_rst:
  ld hl, 2
  add hl, sp
  ld b, (hl)
  ld hl, skip_count
  ld a, (hl)
  and a
  ld a, b
  jr nz, not_cont
  cp 10
  jr z, lf
  cp 22    ;move to
  jr z, posn
  cp 07
  jr z, beep
not_beep:
  push af
direct:
  ld a, 255
  ld hl, SCR_CT  ;disable the scroll? prompt
  ld (hl), a
  pop af
  push iy    ;save callers iy
  ld iy, ERR_NR
  call call_rom3
  defw 16
  pop iy
  ret
not_cont:
  dec (hl)
  jr not_beep
posn:
  push af
  ld a, 2
  ld (hl), a
  jr direct
lf:
  ld a, 13
  jr not_beep
beep:
  push hl
  push de
  ld hl, $0300 ; parameters for the beep
  ld de, $0030
  call call_rom3
  defw 0x03b5 ; call BEEPER
  pop de
  pop hl
  ret
