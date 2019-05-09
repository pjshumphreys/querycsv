org 0xc000
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
  defb "Hello, World!\n",0

backup:
  defb 0x00
