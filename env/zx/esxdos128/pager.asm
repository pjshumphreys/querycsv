include "../common/equs.inc"

PUBLIC bankmBackup
PUBLIC basicBank
PUBLIC spBackup
PUBLIC argName
PUBLIC jumptoit

PUBLIC destinationHighBank
PUBLIC dodos
PUBLIC argv
PUBLIC myfputc_cons
PUBLIC atexit
PUBLIC isr
PUBLIC call_rom3
PUBLIC jp_rom3
PUBLIC __sgoioblk

farCall:
  ; backup registers
  ld (hlBackup), hl
  ld (bcBackup), bc
  ld (deBackup), de

  pop hl ; (hl) contains virtual page number to use

  ;push the virtual page to return to onto the stack
  ld bc, (currentVirtualPage)
  push bc

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

  push hl ; store the address of the function to call on the stack for later

  ;change to the appropriate page
  call changePage

  ;restore all registers and jump to the function we want via ret
  ld de, (deBackup)
  ld bc, (bcBackup)
  ld hl, (hlBackup)
  ret

;---------------------------------------------------

;queue of high ram pages
pageQueue:
  defb 0x00
  defb 0x05

  defb 0x04
  defb 0xfc

  defb 0x03
  defb 0xfe

  defb 0x01
  defb 0xff

;------------------------------------------------

basicBank2:
  defb 0b00000000 ; -1 - stores the page that contains the basic rom

pageLocations:
  ; 255 to load from disk or whatever value resi_alloc gave us
  defb 0b00000000 ; 00 - stores the page that contains the interrupt code and extra storage ram
  defb 0b11111111 ; 01
  defb 0b11111111 ; 02
  defb 0b11111111 ; 03
  defb 0b11111111 ; 04
  defb 0b11111111 ; 05
  defb 0b11111111 ; 06
  defb 0b11111111 ; 07
  defb 0b11111111 ; 08
  defb 0b11111111 ; 09
  defb 0b11111111 ; 10
  defb 0b11111111 ; 11
  ;include "pages.inc"
pageLocationsEnd:

  defb 0x00 ; null terminator for the list of pages

;----------------------------------------------

lookupTable:
  ;include "lookupTable.inc"
  defw $c000
lookupTableEnd:

;------------------------------------------------------

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
  di
  ret

changePage:  ; is the virtual page currently in a ram page?

  ; save bc as we'll be using lddr that corrupts it
  ;push bc
  ld a, (currentVirtualPage)
  ld e, a

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
  ; if no, look up if the page is stored in rom overlay ram.
  ld hl, pageLocations
  ld d, 0
  add hl, de
  ld a, (hl)
  cp 255  ; 255 = load from disk
  jr nz, copyLoToHi
  call dosload
  jr found


copyLoToHi:
  ;if it is in overlay ram, disable interupts, switch in the proper overlay ram and the least recently used page, copy the data, make it the most recently used, switch to it then jump to the proper location.
  di  ; disable interrupts
  ld a, (hl)  ; get low bank number
  call mypager  ; switch it in to $2000-$3fff

  push de
  push hl

  ;switch to the ram page we'll be loading into
  ld a, (pageQueue+6) ; which page to switch to
  ;and 0x07
  ld d, a
  ld a, (bankm)
  and 0b11111000
  or d
  call switchPage
  di

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

  pop de

  ;update pageQueue to reference our newly loaded page
  ld a, e
  ld (pageQueue+7), a

  ld a, (defaultBank)  ; bank with our interupt code
  call mypager  ; switch it in to $0000-$3fff
  ei

  jr found7

;---------------------------------------------------

farRet:
  ; backup registers
  ld (bcBackup), bc

  pop bc  ; get the virtual page number to return to from the stack

  push af
  ld a, b
  ld (currentVirtualPage), a
  pop af

  call changePage

  ld bc, (bcBackup)
  ret

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

__sgoioblk:
  BINARY "../build/data.bin"

;------------------------------------
; array of function trampolines and virtual page numbers

funcstart:  ; the array of jp xxxx instructions and page numbers
  ;include "functions.inc"
_main2:
  jp farcall
  defb 0x06
