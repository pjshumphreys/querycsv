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
  ldir

  ld hl, pageLocationsEnd-1

  ; restore stack pointer and 'hl values
  exx
  ld hl, (exhlBackup)      ; restore BASIC's HL'
  ld sp, (spBackup)
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

  ;switch back to regular interrupts
  im 0

  ret  ; appx 113 bytes