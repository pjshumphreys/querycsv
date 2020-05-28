include "../common/equs.inc"

org 0xbd00
;---------------------------------------
; mypager2 - switch to the low bank specified in the accumulator.
; Interupts must be disabled before this function is called

mypager2:
  ret ; fake paging code to ensure data alignment stays the same
  defs 31, 0

;--------------------------------------------------------
; page2page - copy data pointed to by the stack pointer into a location that ends at the value pointed to by hl.
; The amount of bytes to copy is specified indirectly via the value in the bc register
  di
  push af
  ld a, (bankm)   ; get the current bankm state
  and 0xf8  ; filter out which RAM page is currently switched in at 0xc000-0xffff
  push hl   ; backup hl
  ld hl, bankmBackup  ; bankmBackup contains the number of the high bank (0-7) to return to after the data is copied (usually bank 0?)
  or (hl)   ; add the value in bankmBackup to the accumulator
  ld (bankmBackup), a   ; load the resultant acculumator value back into backmBackup. This will allow us to switch back to a particular high bank after the data copying has been completed
  and 0xf8
  ld hl, destinationHighBank ; which high bank to copy the data into (usually bank 7?)
  or (hl)
  pop hl  ; restore hl
  call switchPage
  di
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
  ld a, (bankmBackup)  ; get bankm state to restore
  call switchPage
  pop af
  jp jumptoit

;---------------------------------------
; pad the output binary out to the proper size.
; This is needed as the code above will be replaced by the interrupt mode 2 jump table after the program has started up.

defs 0x101 - ASMPC, 0xbf
