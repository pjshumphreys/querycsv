include "page2page.map"
include "pager.map"

RESI_GETPAGER equ 0x031c
RESI_FINDBASIC equ 0x0322
RESI_ALLOC equ 0x0325

DOS_SET_1346 equ 0x013f

org 0xc000

; copy all the data from this page to elsewhere in the memory map
;copydata:
  ld hl, page2page
  ld de, origin
  ld bc, page2pageend-page2page
  ldir

  di
  exx
  ld (exhlBackup), hl      ; save BASIC's HL'
  ld (spBackup), sp
  exx
  ei

  ld hl, jumpback
  ld (lastCall+1), hl

  ld a, 3  ; how many loads to do
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
  cp 2
  jr z, secondcopy
  cp 1
  jr z, thirdcopy

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

inf:
  ; switch to interrupt mode 2 so we can use the iy register and
  ; ram at 0x0000-0x2000 with interrupts enabled

  ld b, 255
  ld hl, 0xbd00
intSetup:
  ld (hl), 0xbe
  inc hl
  djnz intSetup

  di
  ld a, 0xbd
  ld i, a
  im 2  ; Set Interrupt Mode 2
  ei

  ; shrink the workspaces to only use page 6
  ld de, 0x601c ; just first half of page 6
  ld hl, 0x7c04 ; just second half of page 6
  ld iy, DOS_SET_1346
  call dodos

  ; setup the residos pager
  ld de, mypager  ; location for paging routine
  ld iy, RESI_GETPAGER  ; +3DOS call ID
  call doresi
  jr nc, failed  ; call failed if Fc=0

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

failed:
startup:
  ld bc, 0x000c
  push bc
  call _fputc_cons
  pop bc

  ld bc, 0x0041
  push bc
  call _fputc_cons
  pop bc

  ld bc, 0x0042
  push bc
  call _fputc_cons
  pop bc

  ld bc, 0x0007
  push bc
  call _fputc_cons
  pop bc

  jp atexit

bcBackup:
  defw 0x0000

first:
  binary "fputs_con_first.bin"
  defb 0

second:
  binary "fputs_con_second.bin"

third:
  binary "fputs_con_third.bin"

page2page:
  binary "pager_part1.bin"
  binary "pager_part2.bin"
page2pageend:

