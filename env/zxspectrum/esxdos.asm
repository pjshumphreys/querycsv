port1 equ 0x7ffd  ; address of ROM/RAM switching port in I/O map
bankm equ 0x5b5c  ; system variable that holds the last value output to 7FFDh


org 0xc000
  
  di
  ld c, 0xe3
  ld a, 0b10000010
  out (c), a
  ld a, (0x2000)
  ld (backup), a
  ld a, 0b10001010
  out (c), a
  ld a, (backup)
  inc a
  ld b, a
  ld (0x2000), a
  ld a, 0b10000010
  out (c), a   
  ld a, (0x2000)
  cp b
  jp nz, copydata
  ld a, (backup)
  ld (0x2000), a
  xor a
  out (c), a
  ld a, (bankm)
  ld bc, port1
  out (c), a
  ei
  ret
  
backup:
  defw 0x0000

hlBackup:
  defw 0x0000

copydata:
  ld a, (backup)
  ld (0x2000), a
  xor a
  out (c), a
  ld a, (bankm)
  ld bc, port1
  out (c), a
  ei
  
  ld hl, page2page
  ld de, 0xc000-(page2pageend-page2page)
  ld bc, page2pageend-page2page
  ldir
  
  ld hl, jumpback
  ld (0xbffe), hl
  
  ld a, 3          ; how many loads to do
  push af

  ld a, 0          ; which page to go back to
  push af
  ld b, 112 ; ceil(223/2)
  ld c, 1
  ld (backup), bc  
  ld hl, 0xec20+223
  ld (hlBackup), hl
  ld hl, first

Loop:
  ld d, (hl)
  inc hl
  ld e, (hl)
  push de
  inc hl
  djnz Loop
  dec c
  jr nz, Loop
  ld a, 7          ; which page to go to
  ld bc, (backup)
  ld hl, (hlBackup)
  jp 0xc000-(page2pageend-page2page)
jumpback:
  pop af
  dec a
  push af
  cp 2
  jp z, secondcopy
  cp 1
  jp z, thirdcopy

  pop af

inf:
  ret

secondcopy:
  ld a, 0          ; which page to go back to
  push af
  ld b, 228
  ld c, 1
  ld (backup), bc  
  ld hl, 0xf511+455
  ld (hlBackup), hl
  ld hl, second
  jr Loop

thirdcopy:
  ld a, 0          ; which page to go back to
  push af
  ld b, 192
  ld c, 1
  ld (backup), bc  
  ld hl, 0xe438+383
  ld (hlBackup), hl
  ld hl, third
  jr Loop

page2page:
  binary "page2page.bin"
page2pageend:

first:
  binary "fputs_con_first.bin"
  defb 0x00

second:
  binary "fputs_con_second.bin"

third:
  binary "fputs_con_third.bin"
