include "../common/equs.inc"

HOOK_PACKAGE equ 0xfb
PKG_RESIDOS equ 0
RESI_ALLOC equ 0x0325

org 0xbd00
loadFromDisk2:
  ld a, (hl)
  push de
  push hl
  cp 0  ; Exit the loop if all pages could be stored in low banks
  jr z, exitLFD

  ;load the data into the low bank
  ld a, e
  call dosload  ; dosload re-enables interupts before it returns back to here...
  di ; ... so disable interrupts again

  ld iy, RESI_ALLOC   ; get free bank
  call doresi3
  jr nc, exitLFD   ; call failed if Fc=0
  pop hl
  push hl
  ld (hl), a  ; save the page number that was returned to us for later
  call mypager  ; switch it in to $2000-$3fff

  ; copy the code to the right place
  ld hl, 0xc000  ; hl = source address for ldir
  ld de, 0x2000  ; de = destination address for ldir
  ld bc, 8192 ; bc = number of bytes to copy for ldir
  ldir

  ; do the next iteration of the loop
  pop hl
  pop de
  inc hl
  inc de
  jr loadFromDisk2

exitLFD:
  pop hl
  pop de
  
  ld a, 2 ; residos48 startup code
  jp dosload  ; dosload re-enables interupts before it returns

doresi3:  ; special startup version that doesn't swap out memory until the relevant page numbers have been obtained
  exx
  ld b, PKG_RESIDOS
  push iy
  pop hl
  rst RST_HOOK
  defb HOOK_PACKAGE
  ld iy, ERR_NR
  ret

;---------------------------------------
; pad the output binary out to the proper size.
; This is needed as the code above will be replaced by the interrupt mode 2 jump table after the program has started up.

defs 0x101 - ASMPC, 0xbf 

;---------------------------------------
; mypager2 - switch to the low bank specified in the accumulator.
; Interupts must be disabled before this function is called

mypager2:
  ret ; just return until the proper paging code is installed into this location
  defs 31, 0
