;SECTION code_compiler
  ;defs 0x8, 0 ; cpm
  ;defs 0x1e, 0 ; msx

  INCLUDE "defines.inc"

EXTERN _dosload
EXTERN l_jphl

funcstart:  ; the array of call xxxx instructions and page numbers
  INCLUDE "functions.inc"

datastart:
  BINARY "../data.bin"

PUBLIC _logNum
PUBLIC __sgoioblk
PUBLIC __sgoioblk_end
PUBLIC farcall
PUBLIC farcall2
EXTERN _versionMajor

EXTBIOS equ 0xFFCA
HOKVLD equ 0xFB20

;-----------------------------------------

farcall:
  ; backup registers
  push de
  push bc
  ld (hlBackup), hl
  pop hl
  ld (bcBackup), hl
  pop hl
  ld (deBackup), hl

  pop bc ; (hl) contains virtual page number to use

  ;push the virtual page to return to onto the stack
  ld hl, (currentVirtualPage)
  push hl

  push bc
  pop hl
  ld e, (hl)
  ;ld c, (hl)
  ;ld b, 0
  ;call serialLnBC

  ;push the far return loader onto the stack so we'll return to it rather than the original caller
  ld bc, farRet
  push bc

  push af
  ;calculate which value in the jump table to use
  or a ; clear carry bit
  ld bc, -(funcstart+3)
  add hl, bc

  ;shift right to divide by 2 (for groups of 2 rather than 4)
  xor a
  ld a,h
  rra
  ld h,a
  ld a,l
  rra
  ld l,a

  ld bc, lookupTable
  add hl, bc
  ld c, (hl)
  inc hl
  ld b, (hl)
  ;call serialLnBC
  ld a, e
  pop de

  push bc ; store the address of the function to call on the stack for later

  ;change to the appropriate page
  push de
  
  ld e, a
  ld a, (currentVirtualPage)
  ld d, a
  ld a, e 
  ld (currentVirtualPage), a
  ld a, d

  ; a = current, e = desired
  call changePage
  pop af

  ;restore all registers and jump to the function we want via ret
  ld hl, (deBackup)
  push hl
  ld hl, (bcBackup)
  push hl
  ld hl, (hlBackup)
  pop bc
  pop de
  ret

farcall2:
  ; backup registers
  push de
  push bc
  ld (hlBackup), hl
  pop hl
  ld (bcBackup), hl
  pop hl
  ld (deBackup), hl

  pop hl ; (hl) contains virtual page number to use
  ;ld c, (hl)
  ;ld b, 0
  ;call serialLnBC

  push af
  ;calculate which value in the jump table to use
  or a ; clear carry bit
  ld bc, -(funcstart+3)
  add hl, bc

  ;shift right to divide by 2 (for groups of 2 rather than 4)
  xor a
  ld a,h
  rra
  ld h,a
  ld a,l
  rra
  ld l,a

  ld bc, lookupTable
  add hl, bc
  ld c, (hl)
  inc hl
  ld b, (hl)
  ;call serialLnBC
  pop af

  push bc ; store the address of the function to call on the stack for later

  ;restore all registers and jump to the function we want via ret
  ld hl, (deBackup)
  push hl
  ld hl, (bcBackup)
  push hl
  ld hl, (hlBackup)
  pop bc
  pop de
_logNum:
found:
  ret

;INCLUDE "serialLnBC.asm"

;------------------------------------------------------

changePage:  ; is the virtual page currently in a ram page?
  cp e
  jr z, found
  
  ld h, 0
  ld l, e
  jmp _dosload

;---------------------------------------------------

farRet:
  ; backup registers
  push de
  push bc
  ld (hlBackup), hl
  pop hl
  ld (bcBackup), hl
  pop hl
  ld (deBackup), hl

  pop de  ; get the virtual page number to return to from the stack

  push af

  ld a, (currentVirtualPage)
  push af
  ld a, e
  ld (currentVirtualPage), a

  pop af
  call changePage
  pop af

  ld hl, (deBackup)
  push hl
  ld hl, (bcBackup)
  push hl
  ld hl, (hlBackup)
  pop bc
  pop de
  ret

;-----------------------------------------

hlBackup:
  defw 0

deBackup:
  defw 0

bcBackup:
  defw 0

spBackup:
  defw 0

currentVirtualPage: ; which virtual page currently is loaded into the memory at 0xc000-0xffff
  defb 0

;----------------------------------------------

lookupTable:
  INCLUDE "lookupTable.inc"
lookupTableEnd:

defc CLIB_FOPEN_MAX=8
PUBLIC CLIB_FOPEN_MAX

__sgoioblk:
  defs CLIB_FOPEN_MAX * 10      ;stdio control block
__sgoioblk_end:        ;end of stdio control block
