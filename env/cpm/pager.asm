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
PUBLIC _hasMapper
PUBLIC _defaultBank
PUBLIC _mapperJumpTable
PUBLIC _loadPageStatus
PUBLIC _initMapper
PUBLIC _cleanup_z80
PUBLIC __sgoioblk
PUBLIC __sgoioblk_end

EXTBIOS equ 0xFFCA
HOKVLD equ 0xFB20

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

_logNum:
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
  jr loop5

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

  ld h, 0
  ld l, e

  ld a, (_hasMapper)
  or a
  jr z, notFound
  ld bc, pageLocations-1
  xor a
  add hl, bc
  ld a, (hl)
  ld (_loadPageStatus), a
  push hl

  ld h, 0
  ld l, e

notFound:
  push de
  call _dosload
  pop de

  ld a, (_hasMapper)
  or a
  jr z, found
  pop hl
  ld a, (_loadPageStatus)
  ld (hl), a
  ; jr found

found:
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

_initMapper: ; detect if a msx2 compatible mem mapper is present
  ; call CPM_VER. msx computers always return 0x22 but still implement more bdos calls than real cp/m 2.2 does
  ld c, 0x0c
  call 0x0005
  cp 0x22
  jr nz, noMapper

  ; test for whether this code is running on an MSX computer by calling MSX_DOSVER
  ld a, 1
  ld c, 0x6f
  call 0x0005
  or a
  jr nz, noMapper

  ; test for presence of extended bios
  ld a, (HOKVLD)
  bit 0, a
  jr z, noMapper  ; no extended bios

  ; call GET_VARTAB to test for msx2 mapper support
  xor a
  ld de, 0x0401
  call EXTBIOS
  or a
  jr z, noMapper  ; no mapper support if a = 0

  ; call GET_JMPTAB and store the resultant address
  ld de, 0x0402
  call EXTBIOS
  ld (_mapperJumpTable), hl

  ; store that the mapper was detected
  ld a, 1
  ld (_hasMapper), a

  ;store the default page segment number
  ld de, 0x0021 ; get the segment number selected on second page (GET_P1)
  xor a
  add hl, de
  call l_jphl
  ld (_defaultBank), a

noMapper:
  ret

_cleanup_z80:
  ld a, (_hasMapper)
  or a
  jr z, freeExit
  ld a, (_defaultBank)
  ld b, 0
  ld c, a
  ld hl, (_mapperJumpTable)
  push hl
  ld de, 0x1e   ; PUT_P1
  add hl, de
  call l_jphl  ; put the default bank back onto page 1 (0x4000 - 0x7fff)
  pop hl
  inc hl
  inc hl
  inc hl ; ld de, 0x03 FRE_SEG
  ld d, h
  ld e, l
  ld hl, pageLocationsEnd-1
freeLoop:
  ld a, (hl)
  cp 255 ; special code that indicates to always load from disk
  jr z, freeSkip
  cp c ; if the current bank is the basic bank then exit the loop
  jr z, freeExit
  ex de, hl
  push bc
  call l_jphl
  pop bc
  ex de, hl
freeSkip:
  dec hl
  jr freeLoop
freeExit:
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

_hasMapper:
  defb 0

_loadPageStatus:
  defb 0

_mapperJumpTable:
  defw 0

;------------------------------------------------

_defaultBank:
  defb 0b00000000 ; stores the page that is the default for 0x4000-0x7fff

pageLocations:
  INCLUDE "pages.inc"
pageLocationsEnd:

  defb 0x00 ; null terminator for the list of pages

;----------------------------------------------

lookupTable:
  INCLUDE "lookupTable.inc"
lookupTableEnd:

DEFC    CLIB_FOPEN_MAX = 10
__sgoioblk:
  defs CLIB_FOPEN_MAX * 10      ;stdio control block
__sgoioblk_end:        ;end of stdio control block
