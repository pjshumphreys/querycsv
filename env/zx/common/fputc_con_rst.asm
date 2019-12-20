include "equs.inc"

; (char to print)
fputc_cons_rom_rst:
  ld hl, 2
  add hl, sp

  ld b, (hl)
  ld hl, skip_count
  ld a, (hl)
  and a
  ld a, b
  jr z, continue
  dec (hl)
  jr not_lf
continue:
  cp 22    ;move to
  jr nz, not_posn
  push af
  ld a, 2
  ld (hl), a
  jr direct
not_posn:
  cp 10
  jr nz, not_lf
  ld a, 13
not_lf:
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