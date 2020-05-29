include "pager.map"

ERR_NR equ 0x5c3a

org 0xe60e
  dec l
  ld (hlBackup), hl

  ; restore stack pointer
  ld sp, (spBackup)

  ; copy page 7 screen ram to screen 5
  ld hl, 0xc000  ; Pointer to the source
  ld de, 0x4000  ; Pointer to the destination
  ld bc, 0x1b00  ; Number of bytes to move
  di
  ldir

  ; reload virtual page 5 back into high bank 0 so we can easily run the program again
  xor a ; load into page 0
  ld (destinationHighBank), a
  ld a, 5 ; load virtual page 5
  call dosload

  ; switch back to the basic bank and disable the extra memory
  ld a, (basicBank)
  ld (defaultBank), a   ; disable the extra memory
  call mypager

  ;switch back to interrupt mode 0
  im 0
  ei

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
  ; set the error code to return
  ld a, (hlBackup)
  ld (ERR_NR), a
  ld bc, 0x0058 ; ERROR-3 + 3
  push bc ; reset the stack after we exit

  ; disable second screen, switch to high bank 0 then exit
  di
  ld a, (bankm)
  and 0xf0  ;disable second screen and go to page 0
  jp switchPage

if ASMPC % 2 != 0
  defb 0
endif
