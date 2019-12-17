PROG equ 0x5c53
NEWPPC equ 0x5c42
NSPPC equ 0x5c44
VARS equ 0x5c4b
NXTLIN equ  0x5c55

org 0xc000
  jp hello

  ;set NEWPPC to $000a
  ld de, $000a
  ld (NEWPPC), de

  ;set NSPPC to 0
  ld a, 0
  ld (NSPPC), a

  ;set VARS to prog + (newProgEnd-newProg) - 1
  ld hl, (PROG)
  ld bc, newProgEnd-newProg
  add hl, bc
  dec hl
  ld (VARS), hl

  ;set NXTLIN to prog + (newProgEnd-newProg) - 3
  dec hl
  dec hl
  ld (NXTLIN), hl

  ;copy the new program
  ld hl, (PROG)
  ld de, newProg
  ;ld bc, newProgEnd-newProg
  ldir

  ret

newProg:
  ;10 CLEAR VAL "46000":PRINT"Filename?":INPUT LINE a$:RANDOMIZE USR VAL "49152"
  defb $00, $0a, $27, $00, $fd, $b0, $22, $34, $36, $30, $30, $30, $22, $3a
  defb $f5, $22, $46, $69, $6c, $65, $6e, $61, $6d, $65, $3f, $22, $3a
  defb $ee, $ca, $61, $24, $3a
  defb $f9, $c0, $b0, $22, $34, $39, $31, $35, $32, $22, $0d, $ff, $0d, $80
newProgEnd:

hello:
  ld a, 2  ; upper screen
  call 0x1601  ; open channel
  ld hl, msg
loop:
  ld a, (hl)
  or a
  jr z, exit
  cp 0x0d
  jr z, dee
  cp 0x0a
  jr z, ay
print:
  rst 16
  inc hl
  jr loop

exit:
  ret

dee:
  ld a, 0x0a
  jr print
ay:
  ld a, 0x0d
  jr print

msg:
  defb "Hello, World!\n", 0


