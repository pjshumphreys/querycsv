include "defines.inc"
include "residos128.inc"
PUBLIC dodos
PUBLIC mypager
PUBLIC defaultBank
PUBLIC fputc_cons
PUBLIC _logNum
PUBLIC _toggleSpinner
PUBLIC _myexit

VARS equ 0x5c4b

DOS_SET_1346 equ 0x013f
RESI_GETPAGER equ 0x031c
RESI_FINDBASIC equ 0x0322
RESI_ALLOC equ 0x0325

HOOK_PACKAGE equ 0xfb
PKG_RESIDOS equ 0
ERR_NR equ 0x5c3a   ; BASIC system variables
RST_HOOK equ 8

; copy all the data from this page to elsewhere in the memory map
;copydata:
  ld hl, fifth ; hl = source address for ldir
  ld de, farcall ; de = destination address for ldir
  ld bc, fifthEnd-fifth ; bc = number of bytes to copy for ldir

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
  ld hl, 0xe440 - first + firstEnd - 1
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
  jp page2page
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
  ld hl, 0xf700 - second + secondEnd - 1
  ld (hlBackup), hl
  ld hl, second
  jr Loop

thirdcopy:
  ld b, [[thirdEnd - third] % 512] / 2
  ld c, [[thirdEnd - third] / 512] + 1
  ld (bcBackup), bc
  ld hl, 0xfa00 - third + thirdEnd - 1
  ld (hlBackup), hl
  ld hl, third
  jr Loop

fourthcopy:
  ld b, [[fourthEnd - fourth] % 512] / 2
  ld c, [[fourthEnd - fourth] / 512] + 1
  ld (bcBackup), bc
  ld hl, 0xe4c0 - fourth + fourthEnd - 1
  ld (hlBackup), hl
  ld hl, fourth
  jr Loop

inf:
  ld a, 1
  ld (destinationHighBank), a  ; which high bank to go to (bank 1)

  ; setup the residos pager
  ld de, mypager2 ; location for paging routine
  ld (mypager+1), de  ; update the jump table record
  ld iy, RESI_GETPAGER  ; +3DOS call ID
  call doresi3
  jr c, suceeded  ; call failed if Fc=0

failed:
  di
  exx
  ld hl, (exhlBackup)      ; restore BASIC's HL'
  exx
  ei

  ; trigger an 'out of memory' error
  rst RST_HOOK
  defb 0x03 ; out of memory

suceeded:
  ; get the low bank number of the basic rom
  ld iy, RESI_FINDBASIC
  call doresi3
  jr nc, failed  ; call failed if Fc=0
  ld (basicBank), a  ; save for later
  ld (basicBank2), a ; copy used to terminate the unload loop run at exit
  ld (defaultBank), a  ; save for later

  ld iy, RESI_ALLOC  ; get free low bank
  call doresi3
  jr nc, failed2  ; call failed if Fc=0
  ld (pageLocations), a ; ensure the default bank memory is freed at exit

failed2:
  ; shrink the workspaces to only use page 6
  ld de, 0x601c ; just first half of page 6
  ld hl, 0x7c04 ; just second half of page 6
  ld iy, DOS_SET_1346
  call dodos

  ; Copy virtual pages into low banks until either we've done them all or we can't allocate any more memory
  ld a, 0xfe
  ld (pageLocations+4), a ; mark page 3 specially so we know to skip to page 6
  ld hl, pageLocations+3
  ld de, 3  ; start at page 3
loopAlloc:
  ld a, (hl)
  push de
  push hl
  cp 0xfe  ;skip to page 6
  jr z, skipTo6
  cp 0  ; Exit the loop if all pages could be stored in low banks
  jr z, startup3
  ld iy, RESI_ALLOC   ; get free bank
  call doresi
  jr nc, startup2   ; call failed if Fc=0
  pop hl
  pop de
  ld (hl), a  ; save the page number that was returned to us for later

  ;load the data into the low bank
  call loadFromDisk2

  inc hl
  inc de
  jr loopAlloc

skipTo6:
  ld a, 0xff
  ld (hl), a
  pop hl
  pop de
  inc hl
  inc hl
  inc de
  inc de
  jr loopAlloc

startup2:
  pop hl
  pop de

startup3:
  ld a, 3
  ld (currentVirtualPage), a  ; update the current virtual page number to be that of the main function
  ld (_libCPage), a

  ; update atexit jump
  ld a, 0xcd  ; call instruction
  ld (atexit), a ; put instruction into the fputc_cons location

  ;update fputc_cons jump
  ld (fputc_cons), a ; put instruction into the fputc_cons location
  ld hl, jp_rom3
  ld (fputc_cons+1), hl ; put jp_rom3 address here

  ld a, 0xc3
  ld (_toggleSpinner), a ; put jp instruction into the _toggleSpinner location
  ld hl, setSpinner
  ld (_toggleSpinner+1), hl

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

  ;set up the code to go back to the default bank
  ld a, (pageLocations)
  ld (defaultBank), a ; bank obtained by RESI_ALLOC
  call mypager  ; switch it in to $0000-$3fff

  ; clear the second screen (and switch to it at the same time)
  push bc
  push de
  push hl
  call call_rom3
  defw 0xf700 ; cls
  pop hl
  pop de
  pop bc

  ;ld bc, 0x0707
  ;push bc
  ;call fputc_cons
  ;pop bc

  ;ld bc, 0x4141
  ;push bc
  ;call fputc_cons
  ;pop bc

  ;ld bc, 0x4242
  ;push bc
  ;call fputc_cons
  ;pop bc

  ;start running main function
  call _realmain

  jp atexit

doresi3:  ; special startup version that doesn't swap out memory until the relevant page numbers have been obtained
  exx
  ld b, PKG_RESIDOS
  push iy
  pop hl
  rst RST_HOOK
  defb HOOK_PACKAGE
  ld iy, ERR_NR
  ret

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

fifth:
  binary "pager.bin"
  binary "page2page.bin"
fifthEnd:
