include "pager.map"

RESI_DEALLOC equ 0x0328

org 0xe60e
  ld (hlBackup), hl

  ;save return location for later
  pop hl
  ld (deBackup), hl

  ; copy page 7 screen ram to screen 5
  ld hl, 0xc000  ; Pointer to the source
  ld de, 0x4000  ; Pointer to the destination
  ld bc, 0x1b00  ; Number of bytes to move
  di
  ldir

  ; switch back to the basic bank and disable the extra memory
  ld a, (basicBank)
  ld (defaultBank), a   ; disable the extra memory
  ld b, a  ; keep the value of the basic bank so we can determine when to quit the deallocation loop
  call mypager

  ;switch back to interrupt mode 0
  im 0
  ei

; free all allocated ram
  ld hl, pageLocationsEnd-1
freeLoop:
  ld a, (hl)
  cp 255 ; special code that indicates to always load from disk
  jr z, freeSkip
  cp b ; if the current bank is the basic bank then exit the loop
  jr z, freeExit
  push bc
  push hl
  ld iy, RESI_DEALLOC
  call doresi
  pop hl
  pop bc
freeSkip:
  dec hl
  jr freeLoop
freeExit:

  ; restore stack pointer
  ld sp, (spBackup)

  ; restore 'hl values
  exx
  ld hl, (exhlBackup)      ; restore BASIC's HL'
  exx

  pop bc  ; get argc
  ld a, c
  pop bc  ; remove argv
  cp 2
  jr nz, skipMoveBack

; restore the a$ basic variable size
  ld bc, 0
  ld hl, (argName)

loopMoveBack:
  ld a, (hl)
  cp 0
  jr z, exitMoveBack
  inc bc
  inc hl
  jr loopMoveBack

exitMoveBack:
  push bc
  push hl
  pop de
  dec hl
  lddr
  pop bc
  ld (hl), c
  inc hl
  ld (hl), b

skipMoveBack:
  ; if hlBackup is non zero, push it onto the stack
  ld hl, (hlBackup)
  ld a, h
  or l
  jp z, nopush
  push hl

nopush:
  ;restore return location
  ld hl, (deBackup)
  push hl

  ; reload virtual page 3 back into high bank 0 so we can easily run the program again
  ld a, 3 ; load virtual page 3
  xor a ; load into page 0
  ld (destinationHighBank), a
  call dosload

  ; disable second screen, switch to high bank 0 then exit
  di
  ld a, (bankm)
  and 0xf0  ;disable second screen and go to page 0
  ld (bankm), a  ; keep system variables up to date
  ld bc, 0x7ffd  ; port used for horiz ROM switch and RAM paging
  out (c), a  ; RAM page 7 to top and DOS ROM
  ei
  ld bc, 0  ; return 0 to basic
  ret
