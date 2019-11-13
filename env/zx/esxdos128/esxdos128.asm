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
  ld a, 10000000b ; eprom 0 0-0x2000, divmmc ram 0 0x2000-0x4000
  out (c), a
  ld a, 0x2000
  ld b, a
  ld a, 10000100b ; eprom 0 0-0x2000, divmmc ram 4? 0x2000-0x4000
  out (c), a
  ld a, 0x2000
  inc a
  ld 0x2000, a
  ld a, 10000000b ; eprom 0 0-0x2000, divmmc ram 0 0x2000-0x4000
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
  pop bc
  pop af
  ei

  ld a, 1
  ld (usingBanks), a

; copy all the data from this page to elsewhere in the memory map
;copydata:
  ld hl, page2page
  ld de, origin
  ld bc, page2pageend-page2page
  ldir

  di
  exx
  ld (exhlBackup), hl      ; save BASIC's HL'
  exx
  ei

  ld hl, jumpback
  ld (lastCall+1), hl

  ld a, 4  ; how many loads to do
  push af

  ld a, 0  ; which page to go back to
  push af
  ld b, 112  ; ceil(223/2)
  ld c, 1
  ld (bcBackup), bc
  ld hl, 0xec20+223
  ld (hlBackup), hl
  ld hl, first

Loop:
  ld d, (hl)
  inc hl
  ld e, (hl)
  push de
  inc hl
  djnz Loop
  dec c
  jr nz, Loop
  ld a, 7  ; which page to go to
  ld bc, (bcBackup)
  ld hl, (hlBackup)
  jp 0xbd00
jumpback:
  pop af
  dec a
  push af
  cp 3
  jr z, secondcopy
  cp 2
  jr z, thirdcopy
  cp 1
  jr z, fourthcopy

  pop af
  jr inf

secondcopy:
  ld a, 0  ; which page to go back to
  push af
  ld b, 228
  ld c, 1
  ld (bcBackup), bc
  ld hl, 0xf511+455
  ld (hlBackup), hl
  ld hl, second
  jr Loop

thirdcopy:
  ld a, 0  ; which page to go back to
  push af
  ld b, 192
  ld c, 1
  ld (bcBackup), bc
  ld hl, 0xe438+383
  ld (hlBackup), hl
  ld hl, third
  jr Loop

fourthcopy:
  ld a, 0  ; which page to go back to
  push af
  ld b, 57
  ld c, 1
  ld (bcBackup), bc
  ld hl, 0xe60e+113
  ld (hlBackup), hl
  ld hl, fourth
  jr Loop

inf:
  ; restore the interrupt mode 2 bytes
  ld b, 255
  ld hl, 0xbd00
intSetup:
  ld (hl), 0xbe
  inc hl
  djnz intSetup

  ; switch to interrupt mode 2 so we can use the iy register and
  ; ram at 0x0000-0x2000 with interrupts enabled
  di
  ld a, 0xbd
  ld i, a
  im 2  ; Set Interrupt Mode 2
  ei

  ;copy 6 16k pages into shadow ram ((128k-32k)/16k) until either we've done all banks or we can't allocate any more memory
  ld hl, pageLocations+6
  ld de, 0x0006  ; start at page 6
loopAlloc:
  ld a, (hl)
  push de
  push hl
  cp 0
  jr z, startup3
  ld iy, RESI_ALLOC   ; get free bank
  call doresi
  jr nc, startup2   ; call failed if Fc=0
  pop hl
  pop de
  ld (hl), a  ; save for later

  ;load the data into the page
  call loadFromDisk2

  inc hl
  inc de
  jr loopAlloc

startup3:
  ld a, (defaultBank)  ; bank obtained by RESI_ALLOC
  call mypager  ; switch it in to $0000-$3fff

startup2:
  pop de
  pop hl

failed:
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

bcBackup:
  defw 0x0000

first:
  binary "fputc_cons_first.bin"
  defb 0

second:
  binary "fputc_cons_second.bin"

third:
  binary "fputc_cons_third.bin"

fourth:
  binary "atexit.bin"

page2page:
  binary "pager_part1.bin"
  binary "pager_part2.bin"
page2pageend:

start:
  INCLUDE "crt/classic/crt_section.asm"
