; 6 of the zx spectrum 128k pages are available, plus another 6 in shadow ram
; we test to see if more than 32k is available and assume 128k is if the test succeeds


include "page2page.map"
include "pager.map"

VARS equ 0x5c4b

DIVMMC equ 0xe3

org 0xc000

CRT_ENABLE_STDIO = 1
__CRT_KEY_CAPS_LOCK = 6
__CRT_KEY_DEL = 12

GLOBAL _heap
GLOBAL fputc_cons
GLOBAL dodos
GLOBAL __CRT_KEY_CAPS_LOCK
GLOBAL __CRT_KEY_DEL

include "globals.inc"

  ; check to see if divmmc 128k is available. Fail if it isn't
  di
  push af
  push bc
  ld c, DIVMMC
  ld a, 0b10000000 ; eprom 0 0-0x2000, divmmc ram 0 0x2000-0x4000
  out (c), a
  ld a, 0x2000
  ld b, a
  ld a, 0b10000100 ; eprom 0 0-0x2000, divmmc ram 4? 0x2000-0x4000
  out (c), a
  ld a, 0x2000
  inc a
  ld 0x2000, a
  ld a, 0b10000000 ; eprom 0 0-0x2000, divmmc ram 0 0x2000-0x4000
  out (c), a
  ld a, 0x2000
  xor b   ; iff is different then only 32k available
  jr z, has128k
  ld a, b  ;put back the original value
  ld 0x2000, a
  ld a, 0 ; regular speccy layout
  out (c), a
  pop bc
  pop af
  ei

  ; trigger an 'out of memory' error
  rst RST_HOOK
  defb 0x03 ; out of memory

has128k:
  ei

  ld a, 1
  ld (usingBanks), a

  ; copy page to page code to somewhere in ram
  ld hl, 0xc000  ; Pointer to the source
  ld de, 0x4000  ; Pointer to the destination
  ld bc, 0x1b00  ; Number of bytes to move
  ldir

  ;put the string constants at the top of ram (in each of pages 0,1,3,4,6,7 for esxdos 128k)

  ;pager and atexit code into ram somewhere
  ld hl, 0xc000  ; Pointer to the source
  ld de, 0x4000  ; Pointer to the destination
  ld bc, 0x1b00  ; Number of bytes to move
  ldir

  ;copy 6 16k pages into shadow ram ((128k-32k)/16k)
  push hl
  push de
  ld hl, pageLocations+6
  ld de, 0x0006  ; start at page 6
  ld a, 5
loopAlloc:
  ld (hl), a  ; save for later
  push af
  and 0b10000000
  out (c), a
  pop af

  ;load the data into the page
  call loadFromDisk2

  inc hl
  inc de
  inc a
  cp 15
  jr nz, loopAlloc

  pop de
  pop hl
  ld a, 0b10000100 ; go to the page we'll be using as extra heap space
  out (c), a
  pop bc
  pop af

  ; get the filename to load from basic variable a$
  ; zx_getstraddr:
  ld d, 'A'
  ld hl, (VARS)

loop:
  ld a, (hl)

  cp 128
  jr z, notFound2

  cp d
  jr z, found2

  push de
  call call_rom3
  defw 0x19b8  ; get the address of the next variable

  ex de, hl
  pop de
  jr loop

found2:
  inc hl
  ld c, (hl)
  xor a
  ld (hl), a  ; zero the first byte so we can find the start of the string later
  inc hl
  ld b, (hl)
  ld a, b
  or c
  jp z, notFound2
  ld (argName), hl
  push hl
  pop de
  inc hl
  ldir
  xor a
  ld (de), a  ; null terminate the string
  ld a, 3
  ld (currentVirtualPage), a
  ld bc, 2
  jr startup

notFound2:
  ld bc, 1
  ; ret

startup:
  ld hl, argv
  push hl  ; argv
  push bc  ; argc
  ld (spBackup), sp

  ; clear the second screen (and switch to it at the same time)
  ld bc, 0x0c0c
  push bc
  call fputc_cons
  pop bc
  pop bc
  pop hl
  push hl
  push bc

  call _main2

  jp atexit

mypager

bcBackup:
  defw 0x0000


start:
  INCLUDE "crt/classic/crt_section.asm"
