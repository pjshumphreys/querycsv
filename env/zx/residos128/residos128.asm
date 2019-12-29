include "residos128.inc"

VARS equ 0x5c4b

DOS_SET_1346 equ 0x013f
RESI_GETPAGER equ 0x031c
RESI_FINDBASIC equ 0x0322
RESI_ALLOC equ 0x0325

org 0xc000
; copy all the data from this page to elsewhere in the memory map
;copydata:
  ld hl, page2page ; source
  ld de, farCall ; destination
  ld (farcall+1), de ; update the farcall address
  ld bc, page2pageend-page2page ; size

  di
  ldir
  exx
  ld (exhlBackup), hl      ; save BASIC's HL'
  exx
  ei

  ;update the mypager jump
  ld hl, mypager2
  ld (mypager+1), hl

  ; update atexit jump
  ld hl, 0x2100 ; ld hl, $00...
  ld (atexit), hl
  ld hl, 0x00cd ; ...00; call
  ld a, l
  ld (atexit+2), hl
  ld hl, jp_rom3
  ld (atexit+4), hl

  ;update fputc_cons jump
  ld (fputc_cons), a ; put 0xcd (call) into the fputc_cons location
  ld (fputc_cons+1), hl ; put jp_rom3 address here

  ld hl, jumpback
  ld (jumptoit+1), hl

  ld a, 4  ; how many loads to do
  push af

  xor a
  ld (destinationHighBank), a  ; which page to go to
  ld b, 110  ; ceil(220/2)
  ld c, 1
  ld (bcBackup), bc
  ld hl, 0xec20+220
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
  ld a, 7  ; which page to go back to
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
  ld b, 231
  ld c, 1
  ld (bcBackup), bc
  ld hl, 0xf511+461
  ld (hlBackup), hl
  ld hl, second
  jr Loop

thirdcopy:
  ld b, 192
  ld c, 1
  ld (bcBackup), bc
  ld hl, 0xe438+383
  ld (hlBackup), hl
  ld hl, third
  jr Loop

fourthcopy:
  ld b, 59
  ld c, 1
  ld (bcBackup), bc
  ld hl, 0xe60e+118
  ld (hlBackup), hl
  ld hl, fourth
  jr Loop

inf:
  ; restore the interrupt mode 2 bytes
  ld b, 255
  ld hl, 0xbd00
intSetup:
  ld (hl), 0xbf
  inc hl
  djnz intSetup

  ld (hl), 0xbf ; unroll the last 2 loop iterations
  inc hl
  ld (hl), 0xbf

  ; switch to interrupt mode 2 so we can use the iy register and
  ; ram at 0x0000-0x2000 with interrupts enabled
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
  ld de, 0xbd00 - 32; mypager  ; location for paging routine
  ld (mypager+1), de  ; update the jump table record
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

  ;copy the pages into shadow ram until either we've done all banks or we can't allocate any more memory
  ld hl, pageLocations+6
  ld de, 0x0006  ; start at page 6
loopAlloc:
  ld a, (hl)
  push de
  push hl
  cp 0  ; Exit the loop if all pages could be stored in lo ram
  jr z, startup3
  ld iy, RESI_ALLOC   ; get free bank
  call doresi
  jr nc, startup2   ; call failed if Fc=0
  pop hl
  pop de
  ld (hl), a  ; save the page number that was returned to us for later

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

  ;setup standard streams
  ld hl, __sgoioblk + 2
  ld (hl), 19 ;stdin
  ld hl, __sgoioblk + 12
  ld (hl), 21 ;stdout
  ld hl, __sgoioblk + 22
  ld (hl), 21 ;stderr

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

first:
  binary "fputc_cons_first.bin"

second:
  binary "fputc_cons_second.bin"
  defb 0

third:
  binary "fputc_cons_third.bin"

fourth:
  binary "atexit.bin"

page2page:
  binary "pager.bin"
  binary "page2page.bin"
page2pageend: