F_OPEN equ 0x9a
F_CLOSE equ 0x9b
F_READ equ 0x9d
FA_READ equ 0x01

include "equs.inc"

;-----------------------------
; esxload

esxload:
  ;convert 8 bit int to 2 ascii bytes and update filename.
  ld c, a  ; Original (hex) number
  ld b, 8  ; How many bits
  xor a   ; Output (BCD) number, starts at 0
bcdloop:
  sla c   ; shift c into carry
  adc a, a
  daa     ; Decimal adjust a, so shift = BCD x2 plus carry
  djnz bcdloop  ; Repeat for 8 bits
  ld b, a
  rra
  rra
  rra
  rra
  and 0b00001111
  or 0x30
  ld c, a
  ld a, b
  and 0b00001111
  or 0x30
  ld b, a
  ld (pagename+6), bc

  push ix

  ld hl, pagename
  push hl
  pop ix

  ld a, (defaultDrive)
  ld b, FA_READ  ; b = open mode
  rst RST_HOOK
  defb F_OPEN
  jr c, esxend
  push af  ; save file handle for later

  ; ld a, filehandle  ; a  = the file handler
  ld ix, 0xc000  ; ix = address where to store what is read
  ld bc, 0x4000  ; bc = bytes to read

  ;switch high page
  push af
  di
  ld a, (bankm)  ; RAM/ROM switching system variable
  ld (bankmBackup), a
  and 0xf8  ; reset bits for page 0
  ld hl, destinationHighBank
  or (hl)

  call switchPage
  ei
  pop af

  push ix
  pop hl

  rst RST_HOOK
  defb F_READ
  jr c, esxexit

  ;switch back high page
  di
  ld a, (bankmBackup)
  call switchPage
  ei

  pop af
  ;ld a, handle           ; a  = file handler
  rst RST_HOOK
  defb F_CLOSE
  pop ix
  ret

esxexit:
  ;switch back high page
  di
  ld a, (bankmBackup)
  call switchPage
  ei

  pop af
  ;ld a, handle           ; a  = file handler
  rst RST_HOOK
  defb F_CLOSE
  pop ix

esxend:
  ;jp printNoFile