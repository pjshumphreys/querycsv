include "defines.inc"
include "residos48.inc"
PUBLIC mypager
PUBLIC defaultBank
PUBLIC fputc_cons
PUBLIC _logNum

VARS equ 0x5c4b

RESI_GETPAGER equ 0x031c
RESI_FINDBASIC equ 0x0322
RESI_DEALLOC equ 0x0328

jr copydata

;put atexit3 at a known location (0xc002)
atexit3:
  jp atexit4

; copy data from this page to elsewhere in the memory map
copydata:
  ld hl, page2page ; hl = source address for ldir
  ld de, farcall ; de = destination address for ldir
  ld bc, page2pageend-page2page ; bc = number of bytes to copy for ldir

  di
  ldir
  exx
  ld (exhlBackup), hl      ; save BASIC's HL'
  exx
  ei

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
  ; Copy virtual pages into low banks until either we've done them all or we can't allocate any more memory
  ld hl, pageLocations+6
  ld de, 0x0006  ; start at page 6
  di
  call loadFromDisk2 ; load all the pages we can into low ram banks

  ;set up the code to go back to the default bank
  ld a, (pageLocations) 
  ld (defaultBank), a
  call mypager

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
  ld hl, atexit2
  ld (atexit+4), hl

  ;update fputc_cons jump
  ;ld (fputc_cons), a ; put instruction into the fputc_cons location
  ;ld (fputc_cons+1), hl ; put jp_rom3 address here

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
  ld a, 2
  ld (currentVirtualPage), a  ; update the current virtual page number to be that of the main function
  ld (_libCPage), a
  call _realmain

  jp atexit

atexit4:
  ;print a newline character to match the 128k print routine's output
  push hl
  ld bc, 0x0a0a
  push bc
  call fputc_cons
  pop bc
  pop hl

  ld (hlBackup), hl

  ; restore stack pointer
  ld sp, (spBackup)

  di

  ; switch back to the basic bank and disable the extra memory
  ld a, (basicBank)
  ld (defaultBank), a   ; disable the extra memory
  ld b, a  ; keep the value of the basic bank so we can determine when to quit the deallocation loop
  call mypager

  ;switch back to interrupt mode 0
  im 0
  ei

; free all allocated ram
  ld hl, pageLocationsEnd-1
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
  ; restore 'hl values
  exx
  ld hl, (exhlBackup)      ; restore BASIC's HL'
  exx

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
  ;pop hl
  ;ld (deBackup), hl

  ; if hlBackup is non zero, push it onto the stack
  ld hl, (hlBackup)
  ld a, h
  or l
  jp z, nopush
  push hl

nopush:
  ;restore return location
  ;ld hl, (deBackup)
  ;push hl

  ld bc, 0  ; return 0 to basic
  ret

page2page:
  binary "pager.bin"
  binary "page2page.bin"
page2pageend:
