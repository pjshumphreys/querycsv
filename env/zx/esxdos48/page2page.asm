include "../common/equs.inc"

DIVMMC equ 0xe3

HOOK_PACKAGE equ 0xfb
PKG_RESIDOS equ 0
RESI_ALLOC equ 0x0325

org 0xbd00
loadFromDisk2: ;; 5 pages can be preloaded into low banks. unrolled for simplicity
  di
  call loadFromDisk3
  call loadFromDisk3
  call loadFromDisk3
  call loadFromDisk3
  call loadFromDisk3

  ld a, 4 ; esxdos48 startup code
  jp dosload  ; dosload re-enables interupts before it returns

loadFromDisk3:
  push de
  push hl

  ;load the data into the low bank
  ld a, e
  call dosload  ; dosload re-enables interupts before it returns back to here...
  di ; ... so disable interrupts again

  pop hl
  push hl
  ld (hl), a  ; save the page number that was returned to us for later
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

  ; do the next iteration of the loop
  pop hl
  pop de
  inc hl
  inc de
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
  or a ; cp 0
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
