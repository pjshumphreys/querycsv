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
PUBLIC _tryAllocate
PUBLIC _putP1
PUBLIC _hasMapper
PUBLIC _defaultBank
PUBLIC _mapperJumpTable
PUBLIC _loadPageStatus
PUBLIC _initMapper
PUBLIC _cleanup_z80
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
  ret

;INCLUDE "serialLnBC.asm"

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

_initMapper: ; detect if a msx2 compatible mem mapper is present
  ; call CPM_VER. msx computers always return 0x22 but still implement more bdos calls than real cp/m 2.2 does
  ld c, 0x0c
  call 0x0005
  cp 0x22
  jr nz, noMapper

  ; test for whether this code is running on an MSX computer by calling MSX_DOSVER
  ld a, 1
  ld c, 0x6f
  ld de, writeBlkEmpty
  call 0x0005
  or a
  jr nz, noMapper

  ;store major msx version
  ld a, b
  ld (_versionMajor), a

  ; test for presence of extended bios
  ld a, (HOKVLD)
  rra
  jr nc, noMapper  ; no extended bios

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

writeBlkEmpty:
  dw 0
  dw 0

_tryAllocate:
  xor a
  ld b,a
  ld hl,(_mapperJumpTable)
  ;ld de, 0   ; ALL_SEG
  ;add hl, de
  call l_jphl
  ld h,0
  jp nc,success
  ld a,(_defaultBank)
  ld l,a
  ret
success:
  ld (_loadPageStatus),a
  ld l,a
  ret

_putP1:
  ld a,l
  ld hl,(_mapperJumpTable)
  ld de,0x1e   ; PUT_P1
  add hl,de
  jp l_jphl  ; put the default bank back onto page 1 (0x4000 - 0x7fff)

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

defc CLIB_FOPEN_MAX=8
PUBLIC CLIB_FOPEN_MAX

__sgoioblk:
  defs CLIB_FOPEN_MAX * 10      ;stdio control block
__sgoioblk_end:        ;end of stdio control block
