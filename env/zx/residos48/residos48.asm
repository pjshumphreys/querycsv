include "page2page.map"
include "pager.map"

RESI_GETPAGER equ 0x031c
RESI_FINDBASIC equ 0x0322
RESI_ALLOC equ 0x0325

DOS_SET_1346 equ 0x013f

VARS equ 0x5c4b

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

; copy all the data from this page to elsewhere in the memory map
;copydata:
  di
  exx
  ld (exhlBackup), hl      ; save BASIC's HL'
  exx
  ei

  ; setup the residos pager
  ld de, mypager  ; location for paging routine
  ld iy, RESI_GETPAGER  ; +3DOS call ID
  call doresi
  jr c, suceeded  ; call failed if Fc=0

  di
  exx
  ld hl, (exhlBackup)      ; restore BASIC's HL'
  exx
  ei

  ; trigger an 'out of memory' error
  rst RST_HOOK
  defb 0x03 ; out of memory

suceeded:
  ; get the number of the basic rom
  ld iy, RESI_FINDBASIC
  call doresi
  jr nc, failed  ; call failed if Fc=0
  ld (basicBank), a  ; save for later

  ld iy, RESI_ALLOC  ; get free bank
  call doresi
  jr nc, failed  ; call failed if Fc=0
  ld (defaultBank), a  ; save for later

  ld a, 1
  ld (usingBanks), a

  ;copy the pages into shadow ram until either we've done all banks or we can't allocate any more memory
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
