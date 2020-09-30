include "defines.inc"
include "esxdos48.inc"
PUBLIC mypager
PUBLIC defaultBank
PUBLIC fputc_cons
PUBLIC _logNum
PUBLIC _toggleSpinner
PUBLIC _myexit

VARS equ 0x5c4b
ERR_NR equ 0x5c3a

jr copydata

;put atexit3 at a known location (0xc002)
atexit3:
  jp atexit4

; copy all the data from this page to elsewhere in the memory map
copydata:
  ld hl, page2page ; hl = source address for ldir
  ld de, farcall ; de = destination address for ldir
  ld bc, page2pageend-page2page ; bc = number of bytes to copy for ldir

  di
  ldir
  ei

  ld a, 4
  ld (currentVirtualPage), a  ; update the current virtual page number to be that of the main function
  ld (_libCPage), a

  ; update atexit jump
  ld a, 0xcd  ; call instruction
  ld (atexit), a ; put instruction into the fputc_cons location
  ld hl, atexit2
  ld (atexit+1), hl

  ;update fputc_cons jump
  ;ld (fputc_cons), a ; put instruction into the fputc_cons location
  ;ld (fputc_cons+1), hl ; put jp_rom3 address here

  ; setup the esxdos pager
  ld de, mypager2 ; location for paging routine
  ld (mypager+1), de  ; update the jump table record

  ;test to see if 128k of divmmc is available. Copy commonly used pages to it if it is
  di
  ld c, DIVMMC
  ld a, 10000001b ; eprom 0 0-0x2000, divmmc ram 0 0x2000-0x4000
  out (c), a
  ld a, (0x2000)
  ld b, a
  ld a, 10000101b ; eprom 0 0-0x2000, divmmc ram 4? 0x2000-0x4000
  out (c), a
  ld a, (0x2000)
  inc a
  ld (0x2000), a
  ld a, 10000001b ; eprom 0 0-0x2000, divmmc ram 0 0x2000-0x4000
  out (c), a
  ld a, (0x2000)
  xor b   ; iff is different then only 32k available
  jr z, has128k

  ; only 32k available
  ld a, b  ;put back the original value
  ld (0x2000), a
  xor a ; ld a, 0 ; put back regular speccy layout
  ;ld (basicBank), a  ; not needed as the values here will already be 0
  ;ld (defaultBank), a
  out (c), a
  ei
  jr startup3

has128k:
  xor a ; ld a, 0 ; put back regular speccy layout
  out (c), a
  ;ld (basicBank), a  ; not needed as the values here will already be 0
  ld a, 10000101b
  ld (defaultBank), a
  ei

  ; pre load the low bank numbers into the virtual pages table
  ld hl, pageLocations+4
  push hl
  ld a, 10000110b
  ld (hl), a
  add a, 2
  inc hl
  inc hl
  ld (hl), a
  add a, 2
  inc hl
  ld (hl), a
  add a, 2
  inc hl
  ld (hl), a
  add a, 2
  inc hl
  ld (hl), a
  pop hl
  ld de, 0x0004  ; start at page 4

  ; Copy the first 5 virtual pages into low banks
  call loadFromDisk2

startup3:
  ld a, (defaultBank)
  call mypager  ; switch it in to $2000-$3fff

  ;setup standard streams
  ld hl, __sgoioblk + 2
  ld (hl), 19 ;stdin
  ld hl, __sgoioblk + 12
  ld (hl), 21 ;stdout
  ld hl, __sgoioblk + 22
  ld (hl), 21 ;stderr

  ld a, 2  ; upper screen
  call call_rom3
  defw 0x1601  ; open channel

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

  ; clear the screen
  push bc
  push de
  push hl
  call call_rom3
  defw 0x0daf
  pop hl
  pop de
  pop bc

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

atexit4:
  ;print a newline character to match the 128k print routine's output
  push hl
  ld hl, 10
  push hl
  call fputc_cons
  call fputc_cons
  pop hl
  pop hl

  dec l
  ld (hlBackup), hl

  ; restore stack pointer
  ld sp, (spBackup)

  di

  ; switch back to the basic bank and disable the extra memory
  ld a, (basicBank)
  ld (defaultBank), a   ; disable the extra memory
  call mypager

  ;switch back to interrupt mode 0
  im 0
  ei

  pop bc  ; get argc
  ld a, c
  pop bc  ; remove argv
  cp 2
  jr nz, skipMoveBack

; restore the a$ basic variable size
  ld bc, 0
  ld hl, (argName)

loopMoveBack:
  ld a, (hl)
  cp 0
  jr z, exitMoveBack
  inc bc
  inc hl
  jr loopMoveBack

exitMoveBack:
  push bc
  push hl
  pop de
  dec hl
  lddr
  pop bc
  ld (hl), c
  inc hl
  ld (hl), b

skipMoveBack:
  ; set the error code to return
  ld a, (hlBackup)
  ld (ERR_NR), a
  ld bc, 0x0058 ; ERROR-3 + 3
  push bc ; reset the stack after we exit
  ret

page2page:
  binary "pager.bin"
  binary "page2page.bin"
page2pageend:
