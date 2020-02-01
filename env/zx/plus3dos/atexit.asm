include "pager.map"

RESI_DEALLOC equ 0x0328

org 0xe60e
  ld (hlBackup), hl

  ; restore stack pointer
  ld sp, (spBackup)

  ; copy page 7 screen ram to screen 5
  ld hl, 0xc000  ; Pointer to the source
  ld de, 0x4000  ; Pointer to the destination
  ld bc, 0x1b00  ; Number of bytes to move
  di
  ldir

  ;switch back to interrupt mode 0
  im 0
  ei

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
  ;pop hl
  ;ld (deBackup), hl

  ; if hlBackup is non zero, push it onto the stack
  ld hl, (hlBackup)
  ld a, h
  or l
  jp z, nopush
  push hl

nopush:
  ;restore return location
  ;ld hl, (deBackup)
  ;push hl

  ; reload virtual page 3 back into high bank 0 so we can easily run the program again
  xor a ; load into page 0
  ld (destinationHighBank), a
  ld a, 1 ; load virtual page 1
  call dosload

  ; disable second screen, switch to high bank 0 then exit
  di
  ld bc, 0  ; return 0 to basic
  ld a, (bankm)
  and 0xf0  ;disable second screen and go to page 0
  jp switchPage
