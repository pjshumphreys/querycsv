include "../common/equs.inc"

DIVMMC equ 0xe3

org 0xbd00
;---------------------------------------------------------------------------------------------------------
;loadFromDisk2 - loads virtual pages into low banks during program startup so that they can be quickly retrieved later

loadFromDisk2: ;; 5 pages can be preloaded into low banks. unrolled for simplicity
  di
  call loadFromDisk3 ;4
  inc de  ;skip 5
  inc hl
  call loadFromDisk3 ;6
  call loadFromDisk3 ;7
  call loadFromDisk3 ;8
  call loadFromDisk3 ;9

  ld a, 4 ; esxdos48 startup code
  jp dosload  ; dosload re-enables interupts before it returns

loadFromDisk3:
  push de ; de contains the virtual page number we want to load into the low bank
  push hl ; (hl) contains the low bank number we obtained from calling RESI_ALLOC

  ;load the data into the low bank
  ld a, e
  call dosload  ; dosload re-enables interupts before it returns back to here...
  di ; ... so disable interrupts again

  pop hl
  push hl
  ld a, (hl) ; put the low bank number into the accumulator
  call mypager  ; switch it in to $2000-$3fff

  ; copy the code to the right place
  ld hl, 0xc000  ; hl = source address for ldir
  ld de, 0x2000  ; de = destination address for ldir
  ld bc, 8192 ; bc = number of bytes to copy for ldir
  ldir

  pop hl
  push hl
  ld a, (hl) ; put the low bank number into the accumulator
  inc a
  call mypager  ; switch it in to $2000-$3fff

  ; copy the code to the right place
  ld hl, 0xe000  ; hl = source address for ldir
  ld de, 0x2000  ; de = destination address for ldir
  ld bc, 8192 ; bc = number of bytes to copy for ldir
  ldir

  ld a, (basicBank) ; put the low bank number into the accumulator
  call mypager  ; switch it in to $2000-$3fff

  pop hl
  pop de
  inc de
  inc hl
  ret

;---------------------------------------
; pad the output binary out to the proper size.
; This is needed as the code above will be replaced by the interrupt mode 2 jump table after the program has started up.

defs 0x101 - ASMPC, 0xbf

;---------------------------------------
; mypager2 - switch to the low bank specified in the accumulator.
; Interupts must be disabled before this function is called

mypager2:
  push bc
  ld c, DIVMMC  ; port used for switching low rom banks
  out (c), a  ; do the switch
  pop bc
  or a ;cp 0
  jr nz, divmmcExit
divmmcDisable:
  push af
  ld a, (0x1ffa)
  cp 0xc9
  jr nz, divmmcSkip
  call 0x1ffa
divmmcSkip:
  pop af
divmmcExit:
  ret
  defs 10, 0  ; 32 bytes total
