port1 equ 0x7ffd  ; address of ROM/RAM switching port in I/O map
bankm equ 0x5b5c  ; system variable that holds the last value output to 7FFDh

RST_HOOK equ 8

DOS_OPEN equ 0x0106
DOS_READ equ 0x0112
DOS_CLOSE equ 0x0109
DOS_ABANDON equ 0x010c

SECTION part1
include "part1.inc"
org origin

;------------------------------------------------

hlBackup:
  defw 0x0000

afBackup:
  defw 0x0000

deBackup:
  defw 0x0000

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
  include "pages.inc"
pageLocationsEnd:

  defb 0x00 ; null terminator for the list of pages

;----------------------------------------------

lookupTable:
  include "lookupTable.inc"
lookupTableEnd:

;--------------------------------------------------

dorom2:
  ld (jumptoit+1), iy
  ld iy, 23610
  push af
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

  ld (hlBackup), hl
  pop hl
  ld (deBackup), hl
  ld hl, (hlBackup)

  call jumptoit  ; go sub routine address in IY

  ld (hlBackup), hl
  ld hl, (deBackup)
  push hl
  ld hl, (hlBackup)
  di
  push af  ; return from JP (IY) will be to here
  push bc
  ld a, (bankmBackup)
  and 7
  ld b, a
  ld a, (bankm)
  and 0xf8
  or b
  ld (bankm), a
  ld bc, port1
  out (c), a  ; switch back to previous RAM page and 48 BASIC
  pop bc
  pop af
  ei
  ret

jumptoit:
  jp 0x0000

;---------------------------------------------------

funcstart:
  include "functions.inc"

call_rom3:
  ld (deBackup), de
  pop hl
  ld d, (hl)
  inc hl
  ld e, (hl)
  inc hl
  push hl
  push de
  pop iy
  ld de, (deBackup)
  jp dorom2

atexit:
  ld hl, 0
  jp atexit2

fputc_cons:
  ld iy, 0xec20  ; fputc_con location in page 7
  jp dorom2

_heap:
  defb 0, 0, 0, 0

;---------------------------------------------------

SECTION part2
org 0xbd00
  defs 257, 0xbe

bankmBackup:
  defb 0

usingBanks:
  defb 0

programName:
  defb "querycsv", 0

argv:
  defw programName
argName:
  defw 0x0000
  defw 0x0000

;---------------------------------------------------

;queue of high ram pages
pageQueue:
  defb 0x00
  defb 0x03

  defb 0x04
  defb 0xfc

  defb 0x03
  defb 0xfe

  defb 0x01
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
  ld de, funcstart+3
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
  ld e, (hl)  ; get the number of the virtual page we want to use
  call changePage

  ;restore all registers
  ld hl, (afBackup)
  push hl
  pop af
  ld de, (deBackup)
  ld hl, (hlBackup)

jumpPoint:
  jp 0x0000  ;self modifying code

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
  call changePage

  ld hl, (afBackup)
  push hl
  pop af
  ld de, (deBackup)
  ld hl, (hlBackup)
  ret

;-----------------------------------------

atexit2:
  ;switch in page 7
  ld de, 0x0700
  call switchPage

  call 0xe60e ; atexit3

  ; switch in page 0 and disable the secondary screen
  ld de, 0x0008
  call switchPage

  ; reload the startup page so we can jump to it directly from basic next time
  ;;xor a ;page 0
  ;;ld (pageQueue+6), a
  ;;ld (pageQueue+7), a

  ;;ld e, 3 ; or whenever virtual page number is the one to restart with
  ;;call loadFromDisk
  ret

;-----------------------------------------

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
  reti  ;21 bytes

;------------------------------------------------------
changePage:  ; is the virtual page currently in a ram page?

  ; save bc as we'll be using lddr that corrupts it
  push bc

found7:
  ld bc, 6
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
  ; if no, load the page from disk
  call loadFromDisk
  ; jr found

found:
  ; if yes, make it the most recently used, switch to it then jump to the proper location
  dec hl
  ld d, (hl)
  push de
  push hl
  pop de
  dec hl
  inc de
  lddr
  pop de
  inc hl
  ld (hl), d
  inc hl
  ld (hl), e
found4:
  ld e, 0
  call switchPage
  pop bc
  ret

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
  ret

;------------------------------------------------------

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

  ; Open the file.
  ld bc, 0x0005  ; use file 0 ; access: shared-read
  ld de, 0x0001  ; create action: error ; open action: read header
  ld hl, filename  ; filename
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
  ret c

  ld hl, invalidFile
  jp atexit

invalidFile:
  ; trigger an 'invalid file name' error
  rst RST_HOOK
  defb 0x0e ; invalid file name

;-----------------------------------------------

dodos:
;
;IY holds the address of the DOS routine to be run. All other registers are
;passed intact to the DOS routine and are returned from it.
;
;Stack is somewhere in central 32K (conforming to DOS requirements), so save AF
;and BC will not be switched out.
;
  push af
  push bc ;temp save registers while switching
  ld a, (bankm) ;RAM/ROM switching system variable
  or 7  ;want RAM page 7
  res 4, a  ;and DOS ROM
  ld bc, bank1  ;port used for horiz ROM switch and RAM paging
  di
  ld (bankm), a ;keep system variables up to date
  out (c), a  ;RAM page 7 to top and DOS ROM
  ei
  pop bc
  pop af

  call jumptoit ;go sub routine address in IY

  push af   ;return from JP (IY) will be to here
  push bc
  ld a, (bankm)
  and 0xF8  ;reset bits for page 0
  set 4, a  ;switch to ROM 3 (48 BASIC)
  ld bc, bank1
  di
  ld (bankm),a
  out (c), a  ;switch back to RAM page 0 and 48 BASIC
  ei
  pop bc
  pop af
  ret

jumptoit:
  jp (iy)

;----------------------------------------------

exhlBackup:
  defw 0x0000

spBackup:
  defw 0x0000

defs 0x2ff - ASMPC
if (ASMPC <> 0x2ff)
  defs CODE_ALIGNMENT_ERROR
endif

currentVirtualPage:
  defb 0x01