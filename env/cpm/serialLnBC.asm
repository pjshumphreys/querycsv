serialLnBC:
  push af
  push hl
  push bc
  push de
  ld hl, bc
  jr serialLn2

_logNum:
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
  jr loop5

serialLn:
  push af
  push hl
  push bc
  push de
loop5:
  ld bc, 0
loop6:
  ld a, (de)
  or a
  jr z, exit5
  inc bc
  inc de
  jr loop6
exit5:
  ld hl, numstr
  ld b, 6
  ld c, 5
exit6:
  push hl
  push bc
  ld e, (hl)
  call 0x0005
  pop bc
  pop hl
  inc hl
  djnz exit6
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
