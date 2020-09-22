;
; Spectrum C Library
;
; Print character to the screen in either 32/64 col mode
;
; We will corrupt any register
;
; Scrolling is now achieved by generic_console_scrollup
;
; We print over 24 lines at 32 columns
;
; djm 3/3/2000

include "../common/equs.inc"

  SECTION first

  __zx_32col_font equ 0x3d00
  __zx_32col_udgs equ 0xeb00

;
; Entry: a= char to print
;
org 0xec20

; fputc_cons:
  ld hl, 4
  add hl, sp
  ld a, (hl)
  ex af, af'
  ld a, (flags)
  and a
  jr z, putit_out1 ;no parameters pending
; Set up so dump into params so first param is at highest posn
  ld l, a
  ld h, 0
  ld de, params-1
  add hl, de
  dec a
  ld (flags), a
  ex af, af'
  ld (hl), a
  ex af, af'
  and a
  ret nz
  ld hl, (flagrout)
  jp (hl)

putit_out1:
  ex af, af'
  bit 7, a
  jr z, putit_out2
; deal with UDGs here
  sub 128
  ld c, a ;save character
  call calc_screen_address
  ex de, hl
  ld l, c
  ld h, 0
  add hl, hl
  add hl, hl
  add hl, hl
  ld bc, __zx_32col_udgs
  add hl, bc
  jp print32_entry

putit_out2:
  ld hl, print32
  cp 32
  jr c, handle_control
  jp (hl)

; Control characters
handle_control:
  and 31
  add a, a ;x2
  ld l, a
  ld h, 0
  ld de, code_table
  add hl, de
  ld e, (hl)
  inc hl
  ld d, (hl)
  ld hl, (chrloc) ;most of them will need the position
  push de
  ret

; 32 column print routine..quick 'n' dirty..we take
; x posn and divide by two to get our real position

print32:
  sub 32
  ld c, a ;save character
  call calc_screen_address
  ex de, hl ;de = screen address
  ld l, c
  ld h, 0
  add hl, hl
  add hl, hl
  add hl, hl
  ld bc, __zx_32col_font
  add hl, bc

print32_entry:
  ld b, 8
  ld a, (control_flags)
  ld c, a

loop32:
  bit 0, c
  ld a, (hl)
  jr z, no_inverse32
  cpl

no_inverse32:
  ld (de), a
  inc d ;down screen row
  inc hl
  djnz loop32
  dec d
  ld hl, (chrloc)
  ld a, d
  rrca
  rrca
  rrca
  and 3
  or 88
  ld d, a
  ld a, (__zx_console_attr)
  ld (de), a

increment:
  inc l
  inc l

posncheck:
  bit 6, l
  jr z, char4

cbak1:
  ld l, 0
  inc h

char4:
  ld (chrloc), hl
  ret


; Calculate screen address from xy coords
; Entry: h = row
; l = column
; Exit: hl = screen address
; b = 64 column mask
calc_screen_address:
  ld hl, (chrloc)
  ld a, h
  cp 24
  jr c, noscroll
  ld hl, control_flags
  bit 1, (hl)
  call z, scrollup
  ld hl, 23*256
  ld (chrloc), hl

noscroll:
  srl l ;divide column by 2
  ld b, 0x0f ;mask
  jr c, just_calculate
  ld b, 0xf0

just_calculate:
  ld a, h
  rrca
  rrca
  rrca
  and 248
  or l
  ld l, a
  ld a, h
  and 0x18
  ;;or 0x40 ;for first screen?
  or 0xC0 ; for second screen?
  ld h, a
  ret


; Ooops..ain't written this yet!
; We should scroll the screen up one character here
; Blanking the bottom row..
scrollup:
  push hl
  call generic_console_scrollup
  pop hl
  ret

;Move to new line

cr:
  ld a, h
  cp 23
  jr nz, cr_1
  ld a, (control_flags)
  bit 1, a
  call z, scrollup
  ld h, 22

cr_1:
  inc h
  ld l, 0
  ld (chrloc), hl
  ret

if ASMPC % 2 != 0
  defb 0
endif

SECTION second
org 0xf511

;get the attribute value at 0xd81f
ld (0xd81f), a
ret

;set the attribute value at 0xd81f
ld a, (0xd81f)
ret

; Clear screen and move to home
cls:
  ld hl, 0
  ld (chrloc), hl
  ld hl, 49152;16384
  ld de, 49153;16385
  ld bc, 6144
  ld (hl), l
  ldir
  ld a, (__zx_console_attr)
  ld (hl), a
  ld bc, 767
  ldir
  ld a, (bankmBackup)  ; system variable that holds current switch state
  or 8
  ld (bankmBackup), a  ; must keep system variable up to date (very important)
  di
  ld a, (bankm)  ; system variable that holds current switch state
  or 8
  ld (bankm), a  ; must keep system variable up to date (very important)
  ld bc, port1  ; the horizontal ROM switch/RAM switch I/O address
  out (c), a
  ei
  ret

; This nastily inefficient table is the code table for the routines
; Done this way for future! Expansion

code_table:
  defw noop ; 0 - NUL
  defw noop ; 1 - SOH
  defw noop ; 2
  defw setudg ; 3
  defw setvscroll ; 4
  defw noop ; 5
  defw noop ; 6
  defw beep ; 7 - BEL
  defw left ; 8 - BS
  defw right ; 9 - HT
  defw cr ;13 - CR (+NL)
  defw up ;11 - UP
  defw noop ;12
  defw down ;10 - LF
  defw noop ;14
  defw noop ;15
  defw setink ;16 - ink
  defw setpaper ;17 - paper
  defw setflash ;18 - flash
  defw setbright ;19 - bright
  defw setinverse ;20 - inverse
  defw noop ;21 - over
  defw setposn ;22
  defw noop ;23
  defw noop ;24
  defw noop ;25
  defw noop ;26
  defw noop ;27
  defw noop ;28
  defw noop ;29
  defw noop ;30
  defw noop ;31

; And now the magic routines

; No operation

noop:
  ret

; Move print position left
left:
  ld a, l
  and a
  jr nz, doleft
  ld l, 31
  jr up

doleft:
  ld a, (deltax)
  neg
  add l
  ld l, a
  ld (chrloc), hl
  ret

;Move print position right
right:
  ld a, l
  cp 31
  ret z
  ld a, (deltax)
  add l
  ld l, a
  ld (chrloc), hl
  ret

;Move print position up
up:
  ld a, h
  and a
  ret z
  dec h
  ld (chrloc), hl
  ret

;Move print position down
down:
  ld a, h
  cp 23
  ret z
  inc h
  ld (chrloc), hl
  ret

beep:
  push hl
  push de
  ld hl, $0300 ; parameters for the beep
  ld de, $0030
  call 0x03b5 ; call BEEPER
  pop de
  pop hl
  ret

; Set __zx_console_attributes etc
doinverse:
  ld hl, control_flags
  set 0, (hl)
  ld a, (params)
  rrca
  ret c
  res 0, (hl)
  ret

dovscroll:
  ld hl, control_flags
  res 1, (hl)
  ld a, (params)
  rrca
  ret c
  set 1, (hl)
  ret

dobright:
  ld hl, __zx_console_attr
  set 6, (hl)
  ld a, (params)
  rrca
  ret c
  res 6, (hl)
  ret

doflash:
  ld hl, __zx_console_attr
  set 7, (hl)
  ld a, (params)
  rrca
  ret c
  res 7, (hl)
  ret

dopaper:
  ld hl, __zx_console_attr
  ld a, (hl)
  and @11000111
  ld c, a
  ld a, (params)
  rlca
  rlca
  rlca
  and @00111000
  or c
  ld (hl), a
  ret

doink:
  ld hl, __zx_console_attr
  ld a, (hl)
  and @11111000
  ld c, a
  ld a, (params)
  and 7 ;doesn't matter what chars were used..
  or c
  ld (hl), a
  ret

doudg:
  ld hl, (params)
  ld (__zx_32col_udgs), hl
  ret

setudg:
  ld hl, doudg
  ld a, 2
  jr setparams

setvscroll:
  ld hl, dovscroll
  jr setink1

setinverse:
  ld hl, doinverse
  jr setink1

setbright:
  ld hl, dobright
  jr setink1

setflash:
  ld hl, doflash
  jr setink1

setpaper:
  ld hl, dopaper
  jr setink1

setink:
  ld hl, doink

setink1:
  ld a, 1 ;number to expect

setparams:
  ld (flags), a
  ld (flagrout), hl
  ret

; Set xy position
; Code 22, y, x (as per ZX)

setposn:
  ld a, 2 ;number to expect
  ld hl, doposn
  jr setparams

; Setting the position
; We only care

doposn:
  ld hl, (params)
  ld de, $2020
  and a
  sbc hl, de
  ld a, (deltax)
  cp 1
  jr z, nomult ; if not, do not double
  rl l

nomult:
  ld a, h ;y position
  cp 24
  ret nc
  bit 6, l ;is x > 64
  ret nz
  ld (chrloc), hl
  ret

; SECTION bss_clib
chrloc equ 0x5c84

; Attribute to use

; Flags..used for incoming bit sequences
flags:
  defb 0

; Routine to jump to when we have all the parameters

flagrout:
  defw 0

; Buffer for reading in parameters - 5 should be enuff ATM?

params:
  defs 5

; Bit 0 = inverse
; Bit 1 = scroll disabled
control_flags:
  defb 0

; SECTION data_clib

print_routine:
  defw print32

deltax:
  defb 1 ;how much to move in x

__zx_console_attr:
  defb 56

generic_console_scrollup:
  push hl

  ; Code to be used when the original ROM is missing or not available
  push ix
  ld ix, zx_rowtab
  ld a, 8

outer_loop:
  push af
  push ix
  ld a, 23

inner_loop:
  ld e, (ix+16)
  ld d, (ix+17)
  ex de, hl
  ld e, (ix+0)
  ld d, (ix+1)
  ld bc, 32
  ldir

  ld bc, 16
  add ix, bc
  dec a
  jr nz, inner_loop
  pop ix
  pop af
  inc ix
  inc ix
  dec a
  jr nz, outer_loop
; clear
  ld ix, zx_rowtab + (192 - 8) * 2
  ld a, 8

clear_loop:
  push ix
  ld e, (ix+0)
  ld d, (ix+1)
  ld h, d
  ld l, e
  ld (hl), 0
  inc de
  ld bc, 31
  ldir
  pop ix
  inc ix
  inc ix
  dec a
  jr nz, clear_loop

  ld hl, $C000+6880;$4000+6880
  ld de, $C000+6881;$4000+6881
  ld bc, 31
  ld a, (__zx_console_attr)
  ld (hl), a
  ldir

  pop ix
  pop hl
  ret

if ASMPC % 2 != 0
  defb 0
endif


  SECTION third
  org 0xe438

zx_rowtab:
  defw 0xC000
  defw 0xC100
  defw 0xC200
  defw 0xC300
  defw 0xC400
  defw 0xC500
  defw 0xC600
  defw 0xC700
  defw 0xC020
  defw 0xC120
  defw 0xC220
  defw 0xC320
  defw 0xC420
  defw 0xC520
  defw 0xC620
  defw 0xC720
  defw 0xC040
  defw 0xC140
  defw 0xC240
  defw 0xC340
  defw 0xC440
  defw 0xC540
  defw 0xC640
  defw 0xC740
  defw 0xC060
  defw 0xC160
  defw 0xC260
  defw 0xC360
  defw 0xC460
  defw 0xC560
  defw 0xC660
  defw 0xC760
  defw 0xC080
  defw 0xC180
  defw 0xC280
  defw 0xC380
  defw 0xC480
  defw 0xC580
  defw 0xC680
  defw 0xC780
  defw 0xC0A0
  defw 0xC1A0
  defw 0xC2A0
  defw 0xC3A0
  defw 0xC4A0
  defw 0xC5A0
  defw 0xC6A0
  defw 0xC7A0
  defw 0xC0C0
  defw 0xC1C0
  defw 0xC2C0
  defw 0xC3C0
  defw 0xC4C0
  defw 0xC5C0
  defw 0xC6C0
  defw 0xC7C0
  defw 0xC0E0
  defw 0xC1E0
  defw 0xC2E0
  defw 0xC3E0
  defw 0xC4E0
  defw 0xC5E0
  defw 0xC6E0
  defw 0xC7E0
  defw 0xC800
  defw 0xC900
  defw 0xCA00
  defw 0xCB00
  defw 0xCC00
  defw 0xCD00
  defw 0xCE00
  defw 0xCF00
  defw 0xC820
  defw 0xC920
  defw 0xCA20
  defw 0xCB20
  defw 0xCC20
  defw 0xCD20
  defw 0xCE20
  defw 0xCF20
  defw 0xC840
  defw 0xC940
  defw 0xCA40
  defw 0xCB40
  defw 0xCC40
  defw 0xCD40
  defw 0xCE40
  defw 0xCF40
  defw 0xC860
  defw 0xC960
  defw 0xCA60
  defw 0xCB60
  defw 0xCC60
  defw 0xCD60
  defw 0xCE60
  defw 0xCF60
  defw 0xC880
  defw 0xC980
  defw 0xCA80
  defw 0xCB80
  defw 0xCC80
  defw 0xCD80
  defw 0xCE80
  defw 0xCF80
  defw 0xC8A0
  defw 0xC9A0
  defw 0xCAA0
  defw 0xCBA0
  defw 0xCCA0
  defw 0xCDA0
  defw 0xCEA0
  defw 0xCFA0
  defw 0xC8C0
  defw 0xC9C0
  defw 0xCAC0
  defw 0xCBC0
  defw 0xCCC0
  defw 0xCDC0
  defw 0xCEC0
  defw 0xCFC0
  defw 0xC8E0
  defw 0xC9E0
  defw 0xCAE0
  defw 0xCBE0
  defw 0xCCE0
  defw 0xCDE0
  defw 0xCEE0
  defw 0xCFE0
  defw 0xD000
  defw 0xD100
  defw 0xD200
  defw 0xD300
  defw 0xD400
  defw 0xD500
  defw 0xD600
  defw 0xD700
  defw 0xD020
  defw 0xD120
  defw 0xD220
  defw 0xD320
  defw 0xD420
  defw 0xD520
  defw 0xD620
  defw 0xD720
  defw 0xD040
  defw 0xD140
  defw 0xD240
  defw 0xD340
  defw 0xD440
  defw 0xD540
  defw 0xD640
  defw 0xD740
  defw 0xD060
  defw 0xD160
  defw 0xD260
  defw 0xD360
  defw 0xD460
  defw 0xD560
  defw 0xD660
  defw 0xD760
  defw 0xD080
  defw 0xD180
  defw 0xD280
  defw 0xD380
  defw 0xD480
  defw 0xD580
  defw 0xD680
  defw 0xD780
  defw 0xD0A0
  defw 0xD1A0
  defw 0xD2A0
  defw 0xD3A0
  defw 0xD4A0
  defw 0xD5A0
  defw 0xD6A0
  defw 0xD7A0
  defw 0xD0C0
  defw 0xD1C0
  defw 0xD2C0
  defw 0xD3C0
  defw 0xD4C0
  defw 0xD5C0
  defw 0xD6C0
  defw 0xD7C0
  defw 0xD0E0
  defw 0xD1E0
  defw 0xD2E0
  defw 0xD3E0
  defw 0xD4E0
  defw 0xD5E0
  defw 0xD6E0
  defw 0xD7E0
