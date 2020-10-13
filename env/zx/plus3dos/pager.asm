include "../common/equs.inc"

PUBLIC bankm
PUBLIC bankmBackup
PUBLIC exhlBackup
PUBLIC spBackup
PUBLIC argName
PUBLIC jumptoit
PUBLIC mypager
PUBLIC defaultBank

PUBLIC destinationHighBank
PUBLIC dodos
PUBLIC argv
PUBLIC fputc_cons
PUBLIC _logNum
PUBLIC _toggleSpinner
PUBLIC _myexit
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
  ;ld c, (hl)
  ;ld b, 0
  ;call serialLnBC

  ;push the virtual page to return to onto the stack
  ld bc, (currentVirtualPage)
  push bc

  ;push the far return loader onto the stack so we'll return to it rather than the original caller
  ld bc, farRet
  push bc

  push af
  ; update the progress spinner
  push hl
  call updateSpinner
  pop hl
  ld e, (hl)

  ;calculate which value in the jump table to use
  or a ; clear carry bit
  ld bc, funcstart+3
  sbc hl, bc

  ;shift right to divide by 2 (for groups of 2 rather than 4)
  srl h
  rr l

  ld bc, lookupTable
  add hl, bc
  ld c, (hl)
  inc hl
  ld b, (hl)
  ;call serialLnBC
  pop af

  push bc ; store the address of the function to call on the stack for later

  ;change to the appropriate page
  push af

  ld a, (currentVirtualPage)
  push af
  ld a, e
  ld (currentVirtualPage), a
  pop af

  ; a = current, e = desired
  call changePage
  pop af

  ;restore all registers and jump to the function we want via ret
  ld de, (deBackup)
  ld bc, (bcBackup)
  ld hl, (hlBackup)
  ret

INCLUDE "../common/spinner.inc"

farcall2:
  ; backup registers
  ld (hlBackup), hl
  ld (bcBackup), bc
  ld (deBackup), de

  pop hl ; (hl) contains virtual page number to use
  ;ld c, (hl)
  ;ld b, 0
  ;call serialLnBC

  ; backup the return address for later use
  pop bc
  ld (libcRet), bc

  ;push the far return loader onto the stack so we'll return to it rather than the original caller
  ld bc, farRet2
  push bc

  push af
  ; update the progress spinner
  push hl
  call updateSpinner
  pop hl
  ld e, 1

  ;calculate which value in the jump table to use
  or a ; clear carry bit
  ld bc, funcstart+3
  sbc hl, bc

  ;shift right to divide by 2 (for groups of 2 rather than 4)
  srl h
  rr l

  ld bc, lookupTable
  add hl, bc

  ld c, (hl)
  inc hl
  ld b, (hl)
  ;call serialLnBC
  pop af

  push bc ; store the address of the function to call on the stack for later

  ;change to the appropriate page
  push af
  ld a, (currentVirtualPage)
  ;a = current, e = desired
  call changePage
  pop af

  ;restore all registers and jump to the function we want via ret
  ld de, (deBackup)
  ld bc, (bcBackup)
  ld hl, (hlBackup)

serialLnHL:
  ret

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
  ld a, (pageQueue)
  ld d, a
  ld a, (hl)
  cp e
  jr z, found4
  ; jr notFound

notFound:
  ld a, (pageQueue+6)
  ld (destinationHighBank), a
  ld a, e
  push de
  call dosload
  pop de
  ;update pageQueue to reference our newly loaded page
  ld a, e
  ld (pageQueue+7), a
  jr changePage

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
  ld a, (bankm)
  and 0b11111000
  or d
  call switchPage
  di
  ret

;---------------------------------------------------

farRet:
  ; backup registers
  ld (hlBackup), hl
  ld (bcBackup), bc
  ld (deBackup), de

  pop de  ; get the virtual page number to return to from the stack

  push af

  ld a, (currentVirtualPage)
  push af
  ld a, e
  ld (currentVirtualPage), a
  pop af

farRet3:
  call changePage
  ;call updateSpinner
  pop af

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

  push af
  ld a, (currentVirtualPage)
  ld e, a
  ld a, 3
  jr farRet3

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

funcstart:  ; the array of call xxxx instructions and page numbers
  INCLUDE "functions.inc"
