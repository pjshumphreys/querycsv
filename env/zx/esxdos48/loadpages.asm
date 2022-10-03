  ;test to see if 128k of divmmc is available. Copy commonly used pages to it if it is
  di
  ld c, DIVMMC
  ld a, 10000001b ; eprom 0 0-0x2000, divmmc ram 0 0x2000-0x4000
  out (c), a
  ld a, (0x2000)
  ld b, a
  ld a, 10000101b ; eprom 0 0-0x2000, divmmc ram 4? 0x2000-0x4000
  out (c), a
  ld a, (0x2000)
  inc a
  ld (0x2000), a
  ld a, 10000001b ; eprom 0 0-0x2000, divmmc ram 0 0x2000-0x4000
  out (c), a
  ld a, (0x2000)
  xor b   ; iff is different then only 32k available
  jr z, has128k

  ; only 32k available
  ld a, b  ;put back the original value
  ld (0x2000), a
  xor a ; ld a, 0 ; put back regular speccy layout
  ;ld (basicBank), a  ; not needed as the values here will already be 0
  ;ld (defaultBank), a
  out (c), a
  ei
  jr startup3

has128k:
  xor a ; ld a, 0 ; put back regular speccy layout
  out (c), a
  ;ld (basicBank), a  ; not needed as the values here will already be 0
  ld a, 10000101b
  ld (defaultBank), a
  ei

  ; pre load the low bank numbers into the virtual pages table
  ld hl, pageLocations+4
  push hl
  ld a, 10000110b
  ld (hl), a
  add a, 2
  inc hl
  inc hl
  ld (hl), a
  add a, 2
  inc hl
  ld (hl), a
  add a, 2
  inc hl
  ld (hl), a
  add a, 2
  inc hl
  ld (hl), a
  pop hl
  ld de, 0x0004  ; start at page 4

  ; Copy the first 5 virtual pages into low banks
  call loadFromDisk2

startup3:
