DOS_OPEN equ 0x0106
DOS_READ equ 0x0112
DOS_CLOSE equ 0x0109
DOS_ABANDON equ 0x010c

include "equs.inc"

;--------------------------------
; dosload - used for both plus3 dos and residos

dosload2:
  exx
  push  hl      ; save BASIC's HL'
  exx

  ;convert 8 bit int to ascii.
  scf ; set carry flag
  ccf ; invert carry flag (to make it be not set)
  daa ; bcd correct register A
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

  ; filenames to load must be terminated by 0xff and not 0
  ld a, 255
  ld (pagename+12), a

  ; Open the file.
  ld bc, 0x0005  ; use file 0 ; access: shared-read
  ld de, 0x0001  ; create action: error ; open action: read header
  ld hl, pagename  ; filename
  ld iy, DOS_OPEN  ; +3DOS call ID
  call dodos
  push af
  jr nc, finish  ; exit if error
  pop af

  ; Get the address of the header in +3DOS memory.
  ld hl, destinationHighBank
  ld b, 0  ; file id 0
  ld c, (hl)  ; ram page that's specified in the page queue
  ld de, 16384  ; amount of data to load
  ld hl, 0xc000
  ld iy, DOS_READ  ; +3DOS call ID
  call dodos
  push af  ; close the file either way, but keep any error code for later

  ; Close the file
  ld b, 0  ; file 0
  ld iy, DOS_CLOSE  ; +3DOS call ID
  call dodos
  jr c, finish  ; exit if no error

  ; Otherwise, abandon file 0
  ld b, 0  ; file 0
  ld iy, DOS_ABANDON  ; +3DOS call ID
  call dodos

finish:
  ld a, 0
  ld (pagename+12), a

  pop af  ; restore error code

  exx
  pop hl      ; restore BASIC's HL'
  exx

  jr nc, next ; printNoFile
  ret

next: