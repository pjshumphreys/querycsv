SECTION code_compiler
org 0xc000

include "defines.inc"
include "plus3dos.inc"
PUBLIC fputc_cons

VARS equ 0x5c4b

DOS_SET_1346 equ 0x013f
ERR_NR equ 0x5c3a   ; BASIC system variables

; copy all the data from this page to elsewhere in the memory map
;copydata:
  ld hl, page2page ; hl = source address for ldir
  ld de, farcall2 ; de = destination address for ldir
  ld (farcall+1), de ; update the farcall address
  ld bc, page2pageend-page2page ; bc = number of bytes to copy for ldir

  di
  ldir
  exx
  ld (exhlBackup), hl      ; save BASIC's HL'
  exx
  ei

  ld a, 7
  ld (destinationHighBank), a  ; which high bank to go to (bank 7)

  ld hl, jumpback
  ld (jumptoit+1), hl

  ld a, 4  ; how many loads to do
  push af
  ld b, [[firstEnd - first] % 512] / 2
  ld c, [[firstEnd - first] / 512] + 1
  ld (bcBackup), bc
  ld hl, 0xec20 - first + firstEnd - 1
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
  xor a  ; zero out the accumulator
  ld (bankmBackup), a ; jump back to page 0
  ld bc, (bcBackup) ; restore bc
  ld hl, (hlBackup) ; restore hl
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
  ld b, [[secondEnd - second] % 512] / 2
  ld c, [[secondEnd - second] / 512] + 1
  ld (bcBackup), bc
  ld hl, 0xf511 - second + secondEnd - 1
  ld (hlBackup), hl
  ld hl, second
  jr Loop

thirdcopy:
  ld b, [[thirdEnd - third] % 512] / 2
  ld c, [[thirdEnd - third] / 512] + 1
  ld (bcBackup), bc
  ld hl, 0xe438 - third + thirdEnd - 1
  ld (hlBackup), hl
  ld hl, third
  jr Loop

fourthcopy:
  ld b, [[fourthEnd - fourth] % 512] / 2
  ld c, [[fourthEnd - fourth] / 512] + 1
  ld (bcBackup), bc
  ld hl, 0xe60e - fourth + fourthEnd - 1
  ld (hlBackup), hl
  ld hl, fourth
  jr Loop

inf:
  ld a, 1
  ld (destinationHighBank), a  ; which high bank to go to (bank 1)

  ; shrink the workspaces to only use page 6
  ld de, 0x601c ; just first half of page 6
  ld hl, 0x7c04 ; just second half of page 6
  ld iy, DOS_SET_1346
  call dodos

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

  ;update the isr jump
  ld hl, isr2
  ld (isr+1), hl

  ; switch to interrupt mode 2 so we can use the iy register and
  ; ram at 0x0000-0x2000 with interrupts enabled
  di
  ld a, 0xbd
  ld i, a
  im 2  ; Set Interrupt Mode 2
  ei

  ; update atexit jump
  ld hl, 0x0021 ; ld hl, $00...
  ld (atexit), hl
  ld hl, 0xcd00 ; ...00; call
  ld a, h
  ld (atexit+2), hl
  ld hl, jp_rom3
  ld (atexit+4), hl

  ;update fputc_cons jump
  ld (fputc_cons), a ; put instruction into the fputc_cons location
  ld (fputc_cons+1), hl ; put jp_rom3 address here

  ;setup standard streams
  ld hl, __sgoioblk + 2
  ld (hl), 19 ;stdin
  ld hl, __sgoioblk + 12
  ld (hl), 21 ;stdout
  ld hl, __sgoioblk + 22
  ld (hl), 21 ;stderr

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

; convert the Pascal string into a C string
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
  di
  ldir
  ei
  xor a
  ld (de), a  ; null terminate the string
  ld bc, 2
  jr startup

notFound2:
  ld bc, 1

startup:
  ld hl, argv
  push hl  ; argv
  push bc  ; argc
  ld (spBackup), sp

  ; clear the second screen (and switch to it at the same time)
  push bc
  push de
  push hl
  call call_rom3
  defw 0xf511
  pop hl
  pop de
  pop bc

  ld bc, 0x0707
  push bc
  call fputc_cons
  pop bc

  ld bc, 0x4141
  push bc
  call fputc_cons
  pop bc

  ld bc, 0x4242
  push bc
  call fputc_cons
  pop bc

  ;start running main function
  ;push atexit ; return to the atexit function
  ;ld a, 6
  ;ld (currentVirtualPage), a  ; update the current virtual page number to be that of the main function
  ;jp _main2

  jp atexit

first:
  binary "fputc_cons_first.bin"
firstEnd:

second:
  binary "fputc_cons_second.bin"
secondEnd:

third:
  binary "fputc_cons_third.bin"
thirdEnd:

fourth:
  binary "atexit.bin"
fourthEnd:

page2page:
  binary "pager.bin"
  binary "page2page.bin"
page2pageend:
