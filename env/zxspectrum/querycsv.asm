port1 equ 0x7ffd  ; address of ROM/RAM switching port in I/O map
bankm equ 0x5b5c  ; system variable that holds the last value output to 7FFDh
ERR_NR equ 0x5c3a   ; BASIC system variables
ERR_SP equ 0x5c3d
PROG equ 0x5c53
jumptoit equ 0xbffc
dodos equ 0xbffd
SEED equ 0xbffe
offset equ 45

HOOK_VERSION equ 0xfc
HOOK_PACKAGE equ 0xfb
RST_HOOK equ 8

PKG_IDEDOS equ 0x01

DOS_OPEN equ 0x0106
DOS_READ equ 0x0112
DOS_CLOSE equ 0x0109
DOS_ABANDON equ 0x010c

__ESXDOS_SYS_M_GETSETDRV equ 0x89
F_OPEN equ 0x9a
F_CLOSE equ 0x9b
F_READ equ 0x9d

FA_READ equ 0x01

  defb 0x0d
  defb 0xff

  ld a, 0xfd  ; jp (iy)
  ld (jumptoit), a

  ld a, 0xc3  ; jp nn
  ld (dodos), a

  ld a, 0xe9  ; jp (hl)
  ld (0xc000), a

  ld a,(0x12a0)
  cp 0xcf
  jr nz, esxcheck

  ld a,(0x12a1)
  cp 0xfe
  jr z, resicheck






esxcheck:
  push ix
  push iy

  ; temporarily route error handling back here
  ld hl, (ERR_SP)
  push hl  ; save the existing ERR_SP

  ld hl, (PROG)
  ld de, esxfail+offset
  add hl, de
  push hl

  ld hl, 0
  add hl, sp
  ld (ERR_SP), hl

  xor a  ; ld a, 0
  rst RST_HOOK
  defb __ESXDOS_SYS_M_GETSETDRV
  pop hl  ; if the code doesn't work, comment out this line
  jr esxcont

esxfail:
  xor a  ; ld a, 0

esxcont:
  pop hl
  ld (ERR_SP), hl

  pop iy
  pop ix

  cp 0
  jr z, noresidos2
  push af  ; save default drive for later

  ; check if 128k is present
  ld hl, (PROG)
  ld de, checkmem+offset
  add hl, de
  call 0xc000

  cp 0
  jr nz, esxgot128
  ld a, 0x34  ; '4'
  jr esxload

noresidos2:
  jr noresidos3

resicheck:
  jr resicheck2

esxgot128:
  ld a, 0x35  ; '5'
  ; jr esxload

esxload:
  ld hl, (PROG)
  ld de, pagename+offset+7
  add hl, de
  ld (hl), a
  ld de, 7
  sbc hl, de
  ld (0xc001), hl
  ld ix, (0xc001)

  pop af
  ld b, FA_READ  ; b = open mode
  rst RST_HOOK
  defb F_OPEN
  jr c, esxend
  push af  ; save file handle for later

  ; ld a, filehandle  ; a  = the file handler
  ld ix, 0xc000  ; ix = address where to store what is read
  ld bc, 0x4000  ; bc = bytes to read
  rst RST_HOOK
  defb F_READ
  jr c, esxexit

  pop af
  ;ld a, handle           ; a  = file handler
  rst RST_HOOK
  defb F_CLOSE

  jp 0xc000

noresidos3:
  jr noresidos

esxexit:
  pop af
  ;ld a, handle           ; a  = file handler
  rst RST_HOOK
  defb F_CLOSE
esxend:
  jr printnofile2






resicheck2:
  ld hl, (ERR_SP)
  push hl  ; save the existing ERR_SP

  ld hl, (PROG)
  ld de, detect_error+offset
  add hl, de
  push hl  ; stack error-handler return address

  ld hl, 0
  add hl, sp
  ld (ERR_SP), hl  ; set the error-handler SP

  rst RST_HOOK  ; invoke the version info hook code
  defb HOOK_VERSION
  pop hl  ; ResiDOS doesn't return, so if we get
  jr noresidos  ; here, some other hardware is present



detect_error:
  pop hl
  ld (ERR_SP), hl  ; restore the old ERR_SP
  ld a, (ERR_NR)
  inc a  ; is the error code now "OK"?
  jr nz, noresidos  ; if not, ResiDOS was not detected
  ex de, hl  ; get HL=ResiDOS version
  ld de, 0x0140  ; DE=minimum version to run with
  and a
  sbc hl, de
  jr c, noresidos

  ld a, 0xff
  ld (ERR_NR), a    ; clear er

  ;update dodos value
  ld hl, (PROG)
  ld de, dodos_residos+offset
  add hl, de
  ld (SEED), hl

  ; check if 128k is present
  ld hl, (PROG)
  ld de, checkmem+offset
  add hl, de
  call 0xc000

  cp 0
  jr nz, resigot128
  ld a, 0x32  ; '2'
  jr dosload

resigot128:
  ld a, 0x33  ; '3'
  jr dosload

printnofile2:
  jr printnofile

noresidos:
  ld a, 0xff
  ld (ERR_NR), a  ; clear error

  ; check if 128k is present
  ld hl, (PROG)
  ld de, checkmem+offset
  add hl, de
  call 0xc000

  cp 2
  jr nz, nadagot

  ;update dodos value
  ld hl, (PROG)
  ld de, plus3dodos+offset
  add hl, de
  ld (SEED), hl
  ld a, 0x31  ; overlay page '1'

dosload:
  exx
  push  hl      ; save BASIC's HL'
  exx

  ld hl, (PROG)
  ld de, pagename+offset+7
  add hl, de
  ld (hl), a
  ld de, 5
  add hl, de
  ld (hl), 255

  ld de, 12
  sbc hl, de

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
  ld hl, 0xc000
  ld bc, 0x0000  ; file id 0, 0 - ram page 0
  ld de, 16384  ; amount of data to load
  ld iy, DOS_READ  ; +3DOS call ID
  call dodos
  push af
  jr nc, finish  ; exit if error
  pop af

  ; Close the file
  ld b, 0  ; file 0
  ld iy, DOS_CLOSE  ; +3DOS call ID
  call dodos
  push af
  jr c, finish  ; exit if error

  ; Otherwise, abandon file 0
  ld b, 0  ; file 0
  ld iy, DOS_ABANDON  ; +3DOS call ID
  call dodos

finish:
  pop af  ; restore error code

  exx
  pop hl      ; restore BASIC's HL'
  exx

  jp c, 0xc000

printnofile:
  push hl
  ld hl, (PROG)
  ld de, pagename+offset+12
  add hl, de
  ld (hl), 0x0a
  pop hl
  ld de, noopen+offset
  push de
  jr println

nadagot:
  ld de, nada+offset
  push de

println:
  ld a, 2  ; upper screen
  call 0x1601  ; open channel
  pop de
  ld hl, (PROG)
  add hl, de
loop:
  ld a, (hl)
  or a
  jr z, exit
  cp 0x0d
  jr z, dee
  cp 0x0a
  jr z, ay
print:
  rst 16
  inc hl
  jr loop

exit:
  ret

dee:
  ld a, 0x0a
  jr print

ay:
  ld a, 0x0d
  jr print





plus3dodos:
;
;IY holds the address of the DOS routine to be run. All other registers are
;passed intact to the DOS routine and are returned from it.
;
;Stack is somewhere in central 32K (conforming to DOS requirements), so save AF
;and BC will not be switched out.
;
  push af
  push bc  ; temp save registers while switching
  ld a, (bankm)  ; RAM/ROM switching system variable
  or 7  ; want RAM page 7
  res 4, a  ; and DOS ROM
  ld bc, port1  ; port used for horiz ROM switch and RAM paging
  di
  ld (bankm), a  ; keep system variables up to date
  out (c), a  ; RAM page 7 to top and DOS ROM
  ei
  ld a, 0xe9  ; jp (iy)
  ld (dodos), a
  pop bc
  pop af

  call jumptoit  ; go sub routine address in IY

  push af  ; return from JP (IY) will be to here
  push bc
  ld a, (bankm)
  and 0F8h  ; reset bits for page 0
  set 4, a  ; switch to ROM 3 (48 BASIC)
  ld bc, port1
  di
  ld (bankm), a
  out (c), a  ; switch back to RAM page 0 and 48 BASIC
  ei
  ld a, 0xc3  ; jp nn
  ld (dodos), a
  pop bc
  pop af
  ld iy, 23610
  ret



dodos_residos:
  exx
  push iy
  pop hl
  ld b, PKG_IDEDOS
  rst RST_HOOK
  defb HOOK_PACKAGE
  ld iy, 23610
  ret

checkmem:
  ld bc, port1  ; the horizontal ROM switch/RAM switch I/O address

  di
  ld a, (bankm)  ; system variable that holds current switch state
  or 23
  ld (bankm), a  ; must keep system variable up to date (very important)
  out (c), a
  ei

  ld a, (0xc000)
  cp 0xe9
  jr z, noram

  ; copy the character set up into page 7 so we can print using page 7 and use page 5 for data
  ld hl, 0x3d00  ; Pointer to the source
  ld de, 0xe800  ; Pointer to the destination
  ld bc, 0x2ff  ; Number of bytes to move
  ldir

  ld bc, port1  ; the horizontal ROM switch/RAM switch I/O address

  di
  ld a, (bankm)  ; system variable that holds current switch state
  res 4, a  ; move right to left in horizontal ROM switch (3 to 2)
  ld (bankm), a  ; must keep system variable up to date (very important)
  out (c), a

  ld a, (0x0008)
  cp 0x50  ; 'P' of 'PLUS3DOS'

  ld a, (bankm)  ; get current switch state
  set 4, a  ; move left to right (ROM 2 to ROM 3)
  jr z, plus3
  and 0xf8  ; also want RAM page 0
  ld (bankm), a  ; update the system variable (very important)
  out (c), a
  ei
  ld a, 1  ; 128k, but no plus3 dos
  ret

noram:
  ld a, 0  ; 48k only
  ret

plus3:
  and 0xf8  ; also want RAM page 0
  ld (bankm), a  ; update the system variable (very important)
  out (c), a
  ei
  ld a, 2  ; 128k with plus3dos
  ret

noopen:
  defb "Couldn't open "
pagename:
  defb "qrycsv00.ovl",0,0

nada:
  defb "Either ESXDOS, ResiDOS v1.40+ or PLUS3DOS is required\n",0
