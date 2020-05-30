include "../common/equs.inc"

org 0xbce0
;---------------------------------------
; mypager2 - switch to the low bank specified in the accumulator.
; Interupts must be disabled before this function is called

mypager2:
  ret ; just return until the proper paging code is installed into this location
  defs 31, 0

;--------------------------------------------------------
; page2page - copy data pointed to by the stack pointer into a location that ends at the value pointed to by hl.
; The amount of bytes to copy is specified indirectly via the value in the bc register
; page2page:
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

;---------------------------------------------------------------------------------------------------------
;loadFromDisk2 - loads virtual pages into low banks during program startup so that they can be quickly retrieved later

loadFromDisk2:
  di
  ld a, (bankm)
  and 0xf8
  or 1  ; high ram bank 1
  call switchPage
  di

  push de ; de contains the virtual page number we want to load into the low bank
  push hl ; (hl) contains the low bank number we obtained from calling RESI_ALLOC

  ld a, e
  call dosload  ; dosload re-enables interupts before it returns back to here...
  di ; ... so disable interrupts again

  pop hl
  push hl
  ld a, (hl) ; put the low bank number into the accumulator
  call mypager  ; switch it in to $0000-$3fff

  ; copy the code to the right place
  ld hl, 0xc000  ; hl = source address for ldir
  ld de, 0  ; de = destination address for ldir
  ld bc, 16384 ; bc = number of bytes to copy for ldir
  ldir

  pop hl
  pop de

  ld a, (bankm)
  and 0xf8  ; high ram page 0
  jp switchPage

;---------------------------------------
; pad the output binary out to the proper size.
; This is needed as the code above will be replaced by the interrupt mode 2 jump table after the program has started up.

defs 0x101 - ASMPC, 0xbf
