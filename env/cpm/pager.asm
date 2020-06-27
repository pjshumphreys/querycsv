;SECTION code_compiler
  ;defs 0x8, 0 ; cpm
  ;defs 0x1e, 0 ; msx

  INCLUDE "defines.inc"

EXTERN _dosload

funcstart:  ; the array of call xxxx instructions and page numbers
  INCLUDE "functions.inc"

datastart:
  BINARY "../data.bin"

;-----------------------------------------

farcall:
  ; backup registers
  ld (hlBackup), hl
  ld (bcBackup), bc
  ld (deBackup), de

  pop hl ; (hl) contains virtual page number to use
  ld e, (hl)
  ;ld c, (hl)
  ;ld b, 0
  ;call serialLnBC

  ;push the virtual page to return to onto the stack
  ld bc, (currentVirtualPage)
  push bc

  ;push the far return loader onto the stack so we'll return to it rather than the original caller
  ld bc, farRet
  push bc

  push af
  ;calculate which value in the jump table to use
  or a ; clear carry bit
  ld bc, funcstart+3
  sbc hl, bc

  ;shift right to divide by 2 (for groups of 2 rather than 4)
  srl h
  rr l

  ld bc, lookupTable
  add hl, bc
  ld c, (hl)
  inc hl
  ld b, (hl)
  ;call serialLnBC
  pop af

  push bc ; store the address of the function to call on the stack for later

  ;change to the appropriate page
  push af

  ld a, (currentVirtualPage)
  push af
  ld a, e
  ld (currentVirtualPage), a
  pop af

  ; a = current, e = desired
  call changePage
  pop af

  ;restore all registers and jump to the function we want via ret
  ld de, (deBackup)
  ld bc, (bcBackup)
  ld hl, (hlBackup)
  ret

farcall2:
  ; backup registers
  ld (hlBackup), hl
  ld (bcBackup), bc
  ld (deBackup), de

  pop hl ; (hl) contains virtual page number to use
  ;ld c, (hl)
  ;ld b, 0
  ;call serialLnBC

  push af
  ;calculate which value in the jump table to use
  or a ; clear carry bit
  ld bc, funcstart+3
  sbc hl, bc

  ;shift right to divide by 2 (for groups of 2 rather than 4)
  srl h
  rr l

  ld bc, lookupTable
  add hl, bc
  ld c, (hl)
  inc hl
  ld b, (hl)
  ;call serialLnBC
  pop af

  push bc ; store the address of the function to call on the stack for later

  ;restore all registers and jump to the function we want via ret
  ld de, (deBackup)
  ld bc, (bcBackup)
  ld hl, (hlBackup)
  ret

serialLnBC:
  push af
  push hl
  push bc
  push de
  ld hl, bc
  jr serialLn2

serialLnHL:
  push af
  push hl
  push bc
  push de
serialLn2:
  ld de, numstr
  ; Get the number in hl as text in de
  ld bc, -10000
  call one
  ld bc, -1000
  call one
  ld bc, -100
  call one
  ld bc, -10
  call one
  ld c, -1
  call one
  ld de, numstr
  jp loop5

serialLn:
  push af
  push hl
  push bc
  push de
loop5:
  ld bc, 0
loop6:
  ld a, (de)
  or a
  jr z, exit5
  inc bc
  inc de
  jr loop6
exit5:
  ld hl, numstr
  ld b, 6
  ld c, 5
exit6:
  push hl
  push bc
  ld e, (hl)
  call 0x0005
  pop bc
  pop hl
  inc hl
  djnz exit6
  pop de
  pop bc
  pop hl
  pop af
  ret

one:
    ld a, $2f

two:
    inc a
    add hl, bc
    jr c, two
    sbc hl, bc
    ld (de), a
    inc de
    ret

numstr:
  defb $30, $30, $30, $30, $30, $0a, $00

;------------------------------------------------------

changePage:  ; is the virtual page currently in a ram page?
  cp e
  jr z, found

notFound:
  ld h, 0
  ld l, e
  push de
  call _dosload
  pop de
  ; jr found

found:
_logNum:
  ret

;---------------------------------------------------

farRet:
  ; backup registers
  ld (hlBackup), hl
  ld (bcBackup), bc
  ld (deBackup), de

  pop de  ; get the virtual page number to return to from the stack

  push af

  ld a, (currentVirtualPage)
  push af
  ld a, e
  ld (currentVirtualPage), a

  pop af
  call changePage
  pop af

  ld de, (deBackup)
  ld bc, (bcBackup)
  ld hl, (hlBackup)
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

DEFC    CLIB_FOPEN_MAX = 10
__sgoioblk:
  defs CLIB_FOPEN_MAX * 10      ;stdio control block
__sgoioblk_end:        ;end of stdio control block
