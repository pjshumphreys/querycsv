include "page2page.sym"
include "pager.sym"

RESI_GETPAGER equ 0x031c
RESI_FINDBASIC equ 0x0322
RESI_ALLOC equ 0x0325

DOS_SET_1346 equ 0x013f

org 0xc000

; copy all the data from this page to elsewhere in the memory map
;copydata:
  ld hl, page2page
  ld de, 0xc000-(page2pageend-page2page)
  ld bc, page2pageend-page2page
  ldir

  ld hl, jumpback
  ld (lastcall), hl

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
  jp 0xc000-(page2pageend-page2page)
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

  exx
  ld (exhlBackup), hl      ; save BASIC's HL'
  ld (spBackup), sp
  exx

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

  di  ; disable interrupts
  ld a, (defaultBank)  ; bank obtained by RESI_ALLOC
  call mypager  ; switch it in to $0000-$3fff
  ei

  ld a, 1
  ld (usingBanks), a

  ;copy the pages into shadow ram until either we've done all banks or we can't allocate any more memory
  ld hl, pageLocations+6
  ld de, 0x0006  ; start at page 6
loopAlloc:
  ld a, (hl)
  push hl
  push de
  cp 0
  jp z, startup
  ld iy, RESI_ALLOC   ; get free bank
  call doresi
  jr nc, startup   ; call failed if Fc=0
  pop de
  pop hl
  push hl
  push de
  ld (hl), a  ; save for later

  ;load the data into the page
  call loadFromDisk

  pop de
  pop hl
  push hl
  push de

  di  ; disable interrupts
  ld a, (hl)  ; bank obtained by RESI_ALLOC
  call mypager  ; switch it in to $0000-$3fff

  ; copy the code to the right place
  ld hl, 0xc000
  ld de, 0
  ld bc, 16384
  ldir

  ld a, (defaultBank)  ; bank obtained by RESI_ALLOC
  call mypager  ; switch it in to $0000-$3fff
  ei

  pop de
  pop hl
  inc hl
  inc de
  jr loopAlloc

startup:
  pop de
  pop hl

failed:
  ld bc, 0x0c0c
  call _fputc_cons
  ld bc, 0x4141
  call _fputc_cons
  ld bc, 0x4242
  call _fputc_cons
  ld bc, 0x0707
  call _fputc_cons
  jr atexit

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

