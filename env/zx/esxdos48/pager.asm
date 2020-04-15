INCLUDE "../common/equs.inc"

PUBLIC bankmBackup
PUBLIC basicBank
PUBLIC spBackup
PUBLIC argName
PUBLIC jumptoit

PUBLIC destinationHighBank
PUBLIC dodos
PUBLIC argv
PUBLIC fputc_cons
PUBLIC atexit
PUBLIC isr
PUBLIC call_rom3
PUBLIC jp_rom3

farcall:
  ; backup registers
  ld (hlBackup), hl
  ld (bcBackup), bc
  ld (deBackup), de

  pop hl ; (hl) contains virtual page number to use

  ;push the virtual page to return to onto the stack
  ld de, (currentVirtualPage)
  push de

  ;push the far return loader onto the stack so we'll return to it rather than the original caller
  ld bc, farRet
  push bc

  push af
  ld a, (hl)
  ld (currentVirtualPage), a

  ;calculate which value in the jump table to use
  ld bc, funcstart+3
  sbc hl, bc

  ;shift right to divide by 2 (for groups of 2 rather than 4)
  srl h
  rr l

  ld bc, lookupTable
  add hl, bc
  pop af

  ld c, (hl)
  inc hl
  ld b, (hl)

  push bc ; store the address of the function to call on the stack for later

  ;change to the appropriate page
  call changePage

  ;restore all registers and jump to the function we want via ret
  ld de, (deBackup)
  ld bc, (bcBackup)
  ld hl, (hlBackup)
  ret

farcall2:
  ; backup registers
  ld (hlBackup), hl
  ld (bcBackup), bc
  ld (deBackup), de

  pop hl ; (hl) contains virtual page number to use
  ld e, (hl)

  ; backup the return address for later use
  pop bc
  ld (libcRet), bc

  ;push the far return loader onto the stack so we'll return to it rather than the original caller
  ld bc, farRet2
  push bc

  push af
  ;calculate which value in the jump table to use
  ld bc, funcstart+3
  sbc hl, bc

  ;shift right to divide by 2 (for groups of 2 rather than 4)
  srl h
  rr l

  ld bc, lookupTable
  add hl, bc
  pop af

  ld c, (hl)
  inc hl
  ld b, (hl)

  push bc ; store the address of the function to call on the stack for later

  ;change to the appropriate page
  call changePage

  ;restore all registers and jump to the function we want via ret
  ld de, (deBackup)
  ld bc, (bcBackup)
  ld hl, (hlBackup)
  ret

;------------------------------------------------

basicBank2:
  defb 0b00000000 ; -1 - stores the page that contains the basic rom

pageLocations:
  ; 255 to load from disk or whatever value resi_alloc gave us
  defb 0b00000000 ; 00 - stores the page that contains the interrupt code and extra storage ram
  INCLUDE "pages.inc"
pageLocationsEnd:

  defb 0x00 ; null terminator for the list of pages

;----------------------------------------------

lookupTable:
  INCLUDE "lookupTable.inc"
lookupTableEnd:

;------------------------------------------------------

changePage:  ; is the virtual page currently in a ram page?

  ; save bc as we'll be using lddr that corrupts it
  cp e
  jr z, found

notFound:
  ; if no, look up if the page is stored in rom overlay ram.
  ld hl, pageLocations
  ld d, 0
  add hl, de
  ld a, (hl)
  cp 255  ; 255 = load from disk
  jr nz, copyLoToHi
  call dosload
  ; jr found

found:
  ret

copyLoToHi:
  ;if it is in overlay ram, disable interupts, switch in the proper overlay ram and the least recently used page, copy the data, make it the most recently used, switch to it then jump to the proper location.
  di  ; disable interrupts
  ld a, (hl)  ; get low bank number
  call mypager  ; switch it in to $2000-$3fff

  ; copy memory from the range of 0x0000-0x1fff to the least recently used page
  ld hl, 0x2000  ; Pointer to the source
  ld de, 0xc000  ; Pointer to the destination
  ld bc, 0x2000  ; Number of bytes to move
  ldir

  pop hl
  ld a, (hl)  ; get low bank number
  inc a
  call mypager  ; switch it in to $2000-$3fff

  ld hl, 0x2000  ; Pointer to the source
  ld de, 0xe000  ; Pointer to the destination
  ld bc, 0x2000  ; Number of bytes to move
  ldir

  ld a, (defaultBank)  ; bank with our interupt code
  call mypager  ; switch it in to $0000-$3fff
  ei
  ret

;---------------------------------------------------

farRet:
  ; backup registers
  ld (hlBackup), hl
  ld (bcBackup), bc
  ld (deBackup), de

  pop bc  ; get the virtual page number to return to from the stack

  ld de, (currentVirtualPage)

  ld a, b
  ld (currentVirtualPage), a

farRet3:
  call changePage

  ld de, (deBackup)
  ld bc, (bcBackup)
  ld hl, (hlBackup)
  ret

farRet2:
  ; backup registers
  ld (hlBackup), hl
  ld (bcBackup), bc
  ld (deBackup), de

  ld bc, (libcRet)
  push bc  ; get the virtual page number to return to from the stack

  ld de, (currentVirtualPage)
  jr farRet3

;-----------------------------------------

atexit2:
  ld a, 4
  call dosload
  ld hl, 0
  jp 0xc002 ; atexit3

;-----------------------------------------

FRAMES equ 0x5c78

isr2:
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

;-----------------------------------

  BINARY "data.bin"

;------------------------------------
; array of function trampolines and virtual page numbers

funcstart:  ; the array of jp xxxx instructions and page numbers
  INCLUDE "functions.inc"
