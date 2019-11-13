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

  SECTION first

  __zx_32col_font equ 0x1d00
  __zx_32col_udgs equ 0x1c00

;
; Entry: a= char to print
;
org 0x10
jp fputc_cons_rst

org 0x38
  push af
  push hl

  ld hl, (_FRAMES) ; frames
  inc hl
  ld (_FRAMES), hl
  ld a, h
  or l
  jp nz, interrupt_end
  ld hl, _FRAMES + 2
  inc (hl)

interrupt_end:
  pop hl ; End of interrrupt
  pop af
  ei
  ret

fputc_cons_rst:
  jr fputc_cons_cont

fputc_cons:
  ld hl, 2
  add hl, sp
  ld a, (hl)

fputc_cons_cont:
  ex af, af'
  ld a, (flags)
  and a
  jp z, putit_out1 ;no parameters pending
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
  or 0x40 ;for first screen?
  ;;or 0xC0 ; for second screen?
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
  defw down ;10 - LF
  defw up ;11 - UP
  defw cls ;12 = FF (and HOME)
  defw cr ;13 - CR (+NL)
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
  di
  ld a, (bankm)  ; system variable that holds current switch state
  or 8
  ld (bankm), a  ; must keep system variable up to date (very important)
  ld bc, port1  ; the horizontal ROM switch/RAM switch I/O address
  out (c), a
  ei
  ret

beep:
  push hl
  push de
  ld hl, $0300 ; parameters for the beep
  ld de, $0030
  call beeper ; call BEEPER
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

  ld hl, $4000+6880;$C000+6880
  ld de, $4000+6881;$C000+6881
  ld bc, 31
  ld a, (__zx_console_attr)
  ld (hl), a
  ldir

  pop ix
  pop hl
  ret

beeper:
  di
  ld a,l
  srl l
  srl l

  cpl
  and 3
  ld c, a
  ld b, 0
  ld ix, beepcode

  add ix,bc

  ld a,(bordcr)
  and 0x38
  rrca
  rrca
  rrca
  or 0x08

beepcode:
  nop
  nop
  nop
  inc b
  inc c

beepcode2:
  dec c
  jr nz, beepcode2
  ld c, 0x3f
  dec b
  jp nz, beepcode2
  xor 0x10
  out (0xfe),a
  ld b, h
  ld c, a
  bit 4, a
  jr nz, beepagain
  ld a,d
  or e
  jr z, beepend
  ld a,c
  ld c,l
  dec de
  jp (ix)

beepagain:
  ld c,l
  inc c
  jp (ix)

beepend:
  ei
  ret

zx_rowtab:
  defw 0x4000
  defw 0x4100
  defw 0x4200
  defw 0x4300
  defw 0x4400
  defw 0x4500
  defw 0x4600
  defw 0x4700
  defw 0x4020
  defw 0x4120
  defw 0x4220
  defw 0x4320
  defw 0x4420
  defw 0x4520
  defw 0x4620
  defw 0x4720
  defw 0x4040
  defw 0x4140
  defw 0x4240
  defw 0x4340
  defw 0x4440
  defw 0x4540
  defw 0x4640
  defw 0x4740
  defw 0x4060
  defw 0x4160
  defw 0x4260
  defw 0x4360
  defw 0x4460
  defw 0x4560
  defw 0x4660
  defw 0x4760
  defw 0x4080
  defw 0x4180
  defw 0x4280
  defw 0x4380
  defw 0x4480
  defw 0x4580
  defw 0x4680
  defw 0x4780
  defw 0x40A0
  defw 0x41A0
  defw 0x42A0
  defw 0x43A0
  defw 0x44A0
  defw 0x45A0
  defw 0x46A0
  defw 0x47A0
  defw 0x40C0
  defw 0x41C0
  defw 0x42C0
  defw 0x43C0
  defw 0x44C0
  defw 0x45C0
  defw 0x46C0
  defw 0x47C0
  defw 0x40E0
  defw 0x41E0
  defw 0x42E0
  defw 0x43E0
  defw 0x44E0
  defw 0x45E0
  defw 0x46E0
  defw 0x47E0
  defw 0x4800
  defw 0x4900
  defw 0x4A00
  defw 0x4B00
  defw 0x4C00
  defw 0x4D00
  defw 0x4E00
  defw 0x4F00
  defw 0x4820
  defw 0x4920
  defw 0x4A20
  defw 0x4B20
  defw 0x4C20
  defw 0x4D20
  defw 0x4E20
  defw 0x4F20
  defw 0x4840
  defw 0x4940
  defw 0x4A40
  defw 0x4B40
  defw 0x4C40
  defw 0x4D40
  defw 0x4E40
  defw 0x4F40
  defw 0x4860
  defw 0x4960
  defw 0x4A60
  defw 0x4B60
  defw 0x4C60
  defw 0x4D60
  defw 0x4E60
  defw 0x4F60
  defw 0x4880
  defw 0x4980
  defw 0x4A80
  defw 0x4B80
  defw 0x4C80
  defw 0x4D80
  defw 0x4E80
  defw 0x4F80
  defw 0x48A0
  defw 0x49A0
  defw 0x4AA0
  defw 0x4BA0
  defw 0x4CA0
  defw 0x4DA0
  defw 0x4EA0
  defw 0x4FA0
  defw 0x48C0
  defw 0x49C0
  defw 0x4AC0
  defw 0x4BC0
  defw 0x4CC0
  defw 0x4DC0
  defw 0x4EC0
  defw 0x4FC0
  defw 0x48E0
  defw 0x49E0
  defw 0x4AE0
  defw 0x4BE0
  defw 0x4CE0
  defw 0x4DE0
  defw 0x4EE0
  defw 0x4FE0
  defw 0x5000
  defw 0x5100
  defw 0x5200
  defw 0x5300
  defw 0x5400
  defw 0x5500
  defw 0x5600
  defw 0x5700
  defw 0x5020
  defw 0x5120
  defw 0x5220
  defw 0x5320
  defw 0x5420
  defw 0x5520
  defw 0x5620
  defw 0x5720
  defw 0x5040
  defw 0x5140
  defw 0x5240
  defw 0x5340
  defw 0x5440
  defw 0x5540
  defw 0x5640
  defw 0x5740
  defw 0x5060
  defw 0x5160
  defw 0x5260
  defw 0x5360
  defw 0x5460
  defw 0x5560
  defw 0x5660
  defw 0x5760
  defw 0x5080
  defw 0x5180
  defw 0x5280
  defw 0x5380
  defw 0x5480
  defw 0x5580
  defw 0x5680
  defw 0x5780
  defw 0x50A0
  defw 0x51A0
  defw 0x52A0
  defw 0x53A0
  defw 0x54A0
  defw 0x55A0
  defw 0x56A0
  defw 0x57A0
  defw 0x50C0
  defw 0x51C0
  defw 0x52C0
  defw 0x53C0
  defw 0x54C0
  defw 0x55C0
  defw 0x56C0
  defw 0x57C0
  defw 0x50E0
  defw 0x51E0
  defw 0x52E0
  defw 0x53E0
  defw 0x54E0
  defw 0x55E0
  defw 0x56E0
  defw 0x57E0

