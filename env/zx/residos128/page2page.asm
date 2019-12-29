include "../common/equs.inc"

org 0xbd00
  di
  push af
  ld a, (bankm)  ; get current switch state
  and 0xf8  ; also want RAM page
  push hl
  ld hl, bankmBackup
  or (hl)
  ld (bankmBackup), a
  and 0xf8
  ld hl, destinationHighBank
  or (hl)
  pop hl
  call switchPage
  pop af

Loop2:
  pop de
  ld (hl), e
  dec hl
  ld (hl), d
  dec hl
  djnz Loop2
  dec c
  jr nz, Loop2

  push af
  ld a, (bankmBackup)  ; get old switch state
  call switchPage
  pop af
  ei
  jp jumptoit

;------------------------------------------------

loadFromDisk2:
  push de ; de contains the virtual page number
  push hl ; (hl) contains the lo-ram bank number we obtained by calling RESI_ALLOC
  di
  ld a, (bankm)
  ld (bankmBackup), a
  ;and 0xf8
  or 1
  call switchPage

  ld a, d
  call dosload  ;does ei

  di
  pop hl
  push hl
  ld a, (hl)
  call mypager  ; switch it in to $0000-$3fff

  ; copy the code to the right place
  ld hl, 0xc000
  ld de, 0
  ld bc, 16384
  ldir

  ld a, (bankmBackup)
  call switchPage
  ei

  pop hl
  pop de
  ret

defs 0x101 - ASMPC, 0xbf

mypager2:
  ret ; just return until the proper paging code is installed into this location
  defs 31, 0xaa
