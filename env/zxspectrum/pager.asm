port1 equ 0x7ffd  ; address of ROM/RAM switching port in I/O map
bankm equ 0x5b5c  ; system variable that holds the last value output to 7FFDh

HOOK_PACKAGE equ 0xfb
RST_HOOK equ 8

PKG_RESIDOS equ 0
PKG_IDEDOS equ 0x01

DOS_OPEN equ 0x0106
DOS_READ equ 0x0112
DOS_CLOSE equ 0x0109
DOS_ABANDON equ 0x010c

RESI_DEALLOC equ 0x0328

SECTION part1
include ""

;------------------------------------------------

mypager:
  defs 32, 0x00

;------------------------------------------------

basicBank:
  defb 0

defaultBank:
pageLocations:
  ; 255 to load from disk or whatever value resi_alloc gave us
  defb 0b00000000 ; 00 - stores the page that contains the interrupt code and extra storage ram
  defb 0b11111111 ; 01
  defb 0b11111111 ; 02
  defb 0b11111111 ; 03
  defb 0b11111111 ; 04
  defb 0b11111111 ; 05
pageLocationsEnd:

;----------------------------------------------

lookupTable:
  defw 0xc000 ; _fprintf_real
lookupTableEnd:

;----------------------------------------------

atexit2:
  ld (hlBackup), hl

  ; switch back to the basic bank
  di
  ld a, (basicBank)
  ld b, a  ; keep the value of the basic bank so we can determine when to quit the deallocation loop
  ld hl, pageLocationsEnd-1
  call mypager
  ei

  ; free all allocated ram
freeLoop:
  ld a, (hl)
  cp 255 ; special code that indicates to always load from disk
  jr z, freeSkip
  cp b ; if the current bank is the basic bank then exit the loop
  jr z, freeExit
  push bc
  push hl
  ld iy, RESI_DEALLOC
  call doresi
  pop hl
  pop bc
freeSkip:
  dec hl
  jr freeLoop
freeExit:

  ; reload the startup page so we can jump to it directly from basic next time
  ld de, 0x0000  ; page 0
  call switchPage

  push af
  xor a ;page 0
  ld (pageQueue+6), a
  pop af
  ld e, 5 ; or whenever virtual page number is the one to restart with
  call loadFromDisk

  ; restore stack pointer and 'hl values
  exx
  ld hl, (exhlBackup)      ; restore BASIC's HL'
  ld sp, (spBackup)
  exx

  ; if hlBackup is non zero, push it onto the stack
  ld hl, (hlBackup)
  ld a, h
  or l
  jr z, nopush
  push hl

nopush:
  ;switch in page 7
  ld de, 0x0700
  call switchPage

  ; copy page 7 screen ram to screen 5
  ld hl, 0xc000  ; Pointer to the source
  ld de, 0x4000  ; Pointer to the destination
  ld bc, 0x1b00  ; Number of bytes to move
  ldir

  ; switch in page 0 and disable secondary screen
  ld de, 0x0010
  call switchPage
  ret

;---------------------------------------------------

farRet:
  ; backup registers
  ld (hlBackup), hl
  ld (deBackup), de
  push af
  pop hl
  ld (afBackup), hl

  pop af
  ld e, a
  ld d, 0
  call changePage

  ld hl, (afBackup)
  push hl
  pop af
  ld de, (deBackup)
  ld hl, (hlBackup)
  ret

;---------------------------------------------------
dorom:
  jp dorom2

dodos:
  jp dodos2

atexit:
  ld hl, 0
  jp atexit2

_fputc_cons:
  ld iy, 0xec20  ; fputc_con location in page 7
  jp dorom2

funcstart:
_fprintf:
  call farCall
  defb 0x09; virtual page number

;---------------------------------------------------

SECTION part2
org 0xbd00
  defs 257, 0xbe

;---------------------------------------------------

afBackup:
  defw 0x0000

hlBackup:
  defw 0x0000

deBackup:
  defw 0x0000

bankmBackup:
  defb 0

usingBanks:
  defb 0

;---------------------------------------------------

;queue of high ram pages
pageQueue:
  defb 0x04
  defb 0xff

  defb 0x03
  defb 0xff

  defb 0x01
  defb 0xff

  defb 0x00
  defb 0xff

filename:
  defb "qrycsv00.ovl", 255

;-------------------------------------------------------

farCall:
  ; backup registers
  ld (hlBackup), hl
  ld (deBackup), de
  push af
  pop hl
  ld (afBackup), hl

  ; remove the top value from the stack so we can use it for a lookup instead
  pop hl

  ;push the virtual page to return to onto the stack
  ld a, (currentVirtualPage)
  push af

  ;push the far return loader onto the stack so we'll return to it rather than the original caller
  ld de, farRet
  push de

  ;save hl for later
  push hl

  ;calculate which value in the jump table to use
  ld de, funcstart-3
  sbc hl, de

  ;shift right to divide by 2 (for groups of 2 rather than 4)
  srl h
  rr l

  ld de, lookupTable
  add hl, de
  ex de, hl

  ;modify the jp statement to go to the place we want
  ld de, jumpPoint+1
  ldi
  ldi
  inc bc ; fix the value of bc
  inc bc

  ;restore hl as it references which page to switch to
  pop hl

  ;change to the appropriate page
  call changePage

  ;restore all registers
  ld hl, (afBackup)
  push hl
  pop af
  ld de, (deBackup)
  ld hl, (hlBackup)

jumpPoint:
  jp 0x0000  ;self modifying code

;--------------------------------------------------

dorom2:
  push af
  ld a, (usingBanks)

  ;test for 0
  inc a
  dec a
  jr z, doromnopage
  di
  ld a, (basicBank)
  call mypager

  ld a, (bankm)  ; RAM/ROM switching system variable
  ld (bankmBackup), a
  or 7  ; want RAM page 7
  ld (bankm), a  ; keep system variables up to date
  push bc
  ld bc, port1  ; port used for horiz ROM switch and RAM paging
  out (c), a  ; RAM page 7 to top and DOS ROM
  pop bc
  ei
  pop af

  call jumptoit  ; go sub routine address in IY

  di
  push af  ; return from JP (IY) will be to here
  ld a, (defaultBank) ; page 0
  call mypager  ; switch it in to $0000-$3fff

doromdone:
  ld a, (bankmBackup)
  ld (bankm), a
  push bc
  ld bc, port1
  out (c), a  ; switch back to previous RAM page and 48 BASIC
  pop bc
  pop af
  ei
  ld iy, 23610
  ret

doromnopage:
  di
  ld a, (bankm)  ; RAM/ROM switching system variable
  ld (bankmBackup), a
  or 7  ; want RAM page 7
  ld (bankm), a  ; keep system variables up to date
  push bc
  ld bc, port1  ; port used for horiz ROM switch and RAM paging
  out (c), a  ; RAM page 7 to top and DOS ROM
  pop bc
  ei
  pop af

  call jumptoit  ; go sub routine address in IY

  di
  push af  ; return from JP (IY) will be to here
  jr doromdone

jumptoit:
  jp (iy)

;--------------------------------------------------

defs 0x1be - ASMPC
if (ASMPC <> 0x1be)
  defs CODE_ALIGNMENT_ERROR
endif

FRAMES equ 0x5c78

isr:
  push af
  push hl
  ld hl, (FRAMES)
  inc hl
  ld (FRAMES), hl
  ld a, h
  or l
  jr nz, keyInt
  ld hl, FRAMES+2
  inc (hl)
keyInt:
  pop hl
  pop af
  ei
  ret  ;21 bytes

;------------------------------------------------------

found:
  ; if yes, make it the most recently used, switch to it then jump to the proper location
  dec hl
  ld d, (hl)
  push de
  push hl
  pop de
  dec hl
  dec hl
  lddr
  pop de
  dec hl
  ld (hl), e
  dec hl
  ld (hl), d
found4:
  ld e, 0
switchPage:
  di
  ld a, d
  ld (currentVirtualPage), a
  ld a, (bankm)  ; system variable that holds current switch state
  and 0xf8
  or d
  xor e
  ld (bankm), a  ; must keep system variable up to date (very important)
  ld bc, port1  ; the horizontal ROM switch/RAM switch I/O address
  out (c), a
  ei
  pop bc
  ret

changePage:  ; is the virtual page currently in a ram page?

  ; save bc as we'll be using lddr that corrupts it
  push bc

  ld bc, 6
  ld e, (hl)  ; get the number of the virtual page we want to use
  ld hl, pageQueue+7
  ld a, (hl)
  cp e
  jr z, found
  dec hl
  dec hl
  dec bc
  dec bc
  ld a, (hl)
  cp e
  jr z, found
  dec hl
  dec hl
  dec bc
  dec bc
  ld a, (hl)
  cp e
  jr z, found
  dec hl
  dec hl
  ld a, (hl)
  cp e
  jr z, found4
  ; jr notFound

notFound:
  ; if no, look up if the page is stored in rom overlay ram.
  ld hl, pageLocations
  ld d, 0
  add hl, de
  ld a, (hl)
  cp 255  ; 255 = load from disk
  jr z, loadFromDisk
  ; jp copyLoToHi

copyLoToHi:
  ;if it is in overlay ram, disable interupts, switch in the proper overlay ram and the least recently used page, copy the data, make it the most recently used, switch to it then jump to the proper location.
  di  ; disable interrupts
  ld a, (hl)  ; bank obtained by RESI_ALLOC
  call mypager  ; switch it in to $0000-$3fff

  ;switch to the ram page we'll be loading into
  push bc  ; purely for stack accounting
  ld a, (pageQueue+6) ; which page to switch to
  ld d, a
  ld e, 0
  call switchPage

  ; copy memory from the range of 0x0000-0x1fff to the least recently used page
  ld hl, 0x0  ; Pointer to the source
  ld de, 0xc000  ; Pointer to the destination
  ld bc, 0x3fff  ; Number of bytes to move
  ldir

  ;update pageQueue to reference our newly loaded page
  ld a, e
  ld (pageQueue), a
  ld hl, pageQueue+7
  ld bc, 6

  ld a, (defaultBank)  ; bank with our interupt code
  call mypager  ; switch it in to $0000-$3fff
  ei
  jp found

loadFromDisk:
  ; if it isn't in rom overlay ram, load it from disk, make it the most recently used, switch to it then jump to the proper location.

  ;update the filename we want to use
  ld hl, filename+6
  ld d, e
  ld e, 0
  push de
  pop af
  daa
  push af
  sra a
  sra a
  sra a
  sra a
  and 0b00001111
  add 0x30
  ld (hl), a
  inc hl
  pop af
  and 0b00001111
  add 0x30
  ld (hl), a
  ld hl, filename

  ; Open the file.
  ld bc, 0x0005  ; use file 0 ; access: shared-read
  ld de, 0x0001  ; create action: error ; open action: read header
  ;ld hl,filename  ; filename
  ld iy, DOS_OPEN  ; +3DOS call ID
  call dodos
  push af
  jr nc, finish  ; exit if error
  pop af

  ; Get the address of the header in +3DOS memory.
  ld hl, pageQueue+6
  ld b, 0  ; file id 0
  ld c, (hl)  ; ram page that's specified in the page queue
  ld de, 16384  ; amount of data to load
  ld hl, 0xc000
  ld iy, DOS_READ  ; +3DOS call ID
  call dodos
  push af  ; close the file either way, but keep any error code for later

  ; Close the file
  ld b, 0  ; file 0
  ld iy, DOS_CLOSE  ; +3DOS call ID
  call dodos
  jr c, finish  ; exit if no error

  ; Otherwise, abandon file 0
  ld b, 0  ; file 0
  ld iy, DOS_ABANDON  ; +3DOS call ID
  call dodos

finish:
  pop af  ; restore error code

  jp c, found

  ld hl, invalidFile
  jp atexit

invalidFile:
  ; trigger an 'invalid file name' error
  rst RST_HOOK
  defb 0x0e ; invalid file name

;-----------------------------------------------

doresi:
  exx
  ld b, PKG_RESIDOS
  jr testbanks

dodos2:
  exx
  ld b, PKG_IDEDOS
testbanks:
  push af
  ld a, (usingBanks)

  ;test for 0
  inc a
  dec a
  jr z, contnopage
  di
  ld a, (basicBank)
  call mypager
  ei
  pop af
  push iy
  pop hl
  rst RST_HOOK
  defb HOOK_PACKAGE
  di
  push af
  ld a, (defaultBank) ; page 0
  call mypager
  pop af
  ei
  ld iy, 23610
  ret

contnopage:
  pop af
  push iy
  pop hl
  rst RST_HOOK
  defb HOOK_PACKAGE
  ld iy, 23610
  ret

;----------------------------------------------

currentVirtualPage:
  defb 0x05

exhlBackup:
  defw 0x0000

spBackup:
  defw 0x0000
