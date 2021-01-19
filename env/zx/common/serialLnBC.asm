serialLnBC:
  push af
  push hl
  push bc
  push de
  ld hl, bc
  jr serialLn2

serialLnHL:
  push af
  push hl
  push bc
  push de
serialLn2:
  ld de, numstr
  ; Get the number in hl as text in de
  ld bc, -10000
  call one
  ld bc, -1000
  call one
  ld bc, -100
  call one
  ld bc, -10
  call one
  ld c, -1
  call one
  ld de, numstr
  jp loop5

serialLn:
  push af
  push hl
  push bc
  push de
loop5:
  di
  push ix
  push de
  pop ix
  ld bc, 0
loop6:
  ld a, (de)
  or a
  jr z, exit5
  inc bc
  inc de
  jr loop6
exit5:
  push bc
  pop de
  ld a, 255
  scf
  call call_rom3
  defw 0x04c6
  pop ix
  ei
  pop de
  pop bc
  pop hl
  pop af
  ret

one:
    ld a, $2f

two:
    inc a
    add hl, bc
    jr c, two
    sbc hl, bc
    ld (de), a
    inc de
    ret

numstr:
  defb $30, $30, $30, $30, $30, $0a, $00
