INCLUDE "../common/equs.inc"

PUBLIC bankmBackup
PUBLIC basicBank
PUBLIC doresi
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

attrFlash equ 0x581f

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

spinner:
  defb "---ooOOoo"
spinnerEnd:

setSpinner:
  ld a, l
  ld (spinnerEnabled), a
  or a  ; cp 0
  jr nz, skipBackspace
printBackspace:
  ld a, (cursorOutput)
  or a  ; cp 0
  jr z, skipBackspace
  xor a ;ld a, 0
  ld (cursorOutput), a
  ld hl, 8  ; backspace
  push hl
  call fputc_cons
  pop hl
skipBackspace:
  ret

updateSpinner:
  ld a, (spinnerEnabled)
  or a  ; cp 0
  jr z, skipBackspace
  ld a, (cursorOutput)
  or a  ; cp 0
  jr z, skipBackspace2
  ld hl, 8  ; backspace
  push hl
  call fputc_cons
  pop hl
skipBackspace2:
  ld hl, (currentWaitCursor)
  ld de, spinnerEnd
  xor a
  sbc hl, de
  add hl, de
  jp nz, skipReset
  ld hl, spinner
skipReset:
  inc hl
  ld (currentWaitCursor), hl
  dec hl
  ld l, (hl)
  ld h, 0
  push hl
  call fputc_cons
  pop hl
  ld a, 1
  ld (cursorOutput), a
  ret

spinnerEnabled:
  defb 1

cursorOutput:
  defb 0

currentWaitCursor:
  defw spinner

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
  ld e, 2

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
  ld a, e
  push de
  call dosload
  pop de
  ; jr found

found:
  ld a, (defaultBank)
  call mypager  ; switch it in to $0000-$3fff. always do it here as some c lib functions can page in the esxdos buffer
  ret

copyLoToHi:
  ;if it is in overlay ram, disable interupts, switch in the proper overlay ram and the least recently used page, copy the data, make it the most recently used, switch to it then jump to the proper location.
  di  ; disable interrupts
  ld a, (hl)  ; bank obtained by RESI_ALLOC

  push de
  call mypager  ; switch it in to $0000-$3fff

  ; copy memory from the range of 0x0000-0x1fff to the least recently used page
  ld hl, 0x0  ; Pointer to the source
  ld de, 0xc000  ; Pointer to the destination
  ld bc, 0x4000  ; Number of bytes to move
  ldir

  pop de
  ld a, e
  ei

  jp changePage

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
  ld a, 2
  jr farRet3

;-----------------------------------------

atexit2:
  ld a, 2
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

funcstart:  ; the array of call xxxx instructions and page numbers
  INCLUDE "functions.inc"
