ERR_SP equ 0x5c3d  ; BASIC system variables
PROG equ 0x5c53
NEWPPC equ 0x5c42
NSPPC equ 0x5c44
VARS equ 0x5c4b
NXTLIN equ  0x5c55

__ESXDOS_SYS_M_GETSETDRV equ 0x89
HOOK_VERSION equ 0xfc

include "equs.inc"

org 0xc000
  jp hello  ; jump to the placeholder payload if all this is successful
  jp initialLoad

memoryType:
  defb 0

  jp (hl)

;-------------------------------------
; hello

hello:
  ld de, helloMsg
  jp printLn2

helloMsg:
  defb "Hello, World!\n", 0

;-------------------------------------
; initialLoad

initialLoad:
  ;check for residos. if it's not present then continue on to the check for esxdos
  ld a,(0x12a0)
  cp 0xcf
  jr nz, esxcheck

  ld a,(0x12a1)
  cp 0xfe
  jr z, resicheck

;-------------------------------------
; esxcheck

esxcheck:
  push ix
  push iy

  ; temporarily route error handling back here
  ld hl, (ERR_SP)
  push hl  ; save the existing ERR_SP

  ld hl, esxfail
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
  jp z, plus3check
  push af

  ; this will now succeed so we can update the basic program
  call reenter

  ; copy all the pieces of code we'll need to continue with into the right places
  ld hl, printLn2
  ld de, 0xC000-printLn2End+printLn2
  ld bc, printLn2End-printLn2
  di
  ldir
  ei

  pop af
  ld (defaultDrive), a ; save default drive for later

  ld hl, dosload_esxdos
  ld de, 0xC000-printLn2End+printLn2-dosload_esxdosEnd+dosload_esxdos
  ld (dosload+1), de
  ld bc, dosload_esxdosEnd-dosload_esxdos
  di
  ldir
  ei

  ld hl, fputc_cons_rst
  ld de, 0xC000-printLn2End+printLn2-dosload_esxdosEnd+dosload_esxdos-fputc_cons_rstEnd+fputc_cons_rst
  ld (fputc_cons+1), de
  ld bc, fputc_cons_rstEnd-fputc_cons_rst
  di
  ldir
  ei

  ; is 128k present?
  ld a, (memoryType)
  cp 0
  jr nz, esxgot128

  ld a, 0x04  ; '4'
  jp dosload

esxgot128:
  ld a, 0x05  ; '5'
  jp dosload

;-------------------------------------
; resicheck

resicheck:
  ld hl, (ERR_SP)
  push hl  ; save the existing ERR_SP

  ld hl, resicont
  push hl  ; stack error-handler return address

  ld hl, 0
  add hl, sp
  ld (ERR_SP), hl  ; set the error-handler SP

  rst RST_HOOK  ; invoke the version info hook code
  defb HOOK_VERSION
  pop hl  ; ResiDOS doesn't return, so if we get
  jr plus3check  ; here, some other hardware is present

resicont:
  pop hl
  ld (ERR_SP), hl  ; restore the old ERR_SP
  ld a, (ERR_NR)
  inc a  ; is the error code now "OK"?
  jr nz, plus3check  ; if not, ResiDOS was not detected
  ex de, hl  ; get HL=ResiDOS version
  ld de, 0x0140  ; DE=minimum version to run with
  and a
  sbc hl, de
  jr c, plus3check

  ld a, 0xff
  ld (ERR_NR), a    ; clear er

  ; this will now succeed so we can update the basic program
  call reenter

  ; copy all the pieces of code we'll need to continue with into the right places
  ld hl, printLn2
  ld de, 0xC000-printLn2End+printLn2
  ld bc, printLn2End-printLn2
  di
  ldir
  ei

  ld de, 0xC000-printLn2End+printLn2-dosload_residosEnd+dosload_residos
  ld (dosload+1), de
  ld hl, dosload_residos
  ld bc, dosload_residosEnd-dosload_residos
  di
  ldir
  ei

  ld de, 0xC000-printLn2End+printLn2-dosload_residosEnd+dosload_residos-dodos_residosEnd+dodos_residos
  ld (dodos+1), de
  ld hl, 5
  add hl, de
  ld (doresi+1), hl
  ld hl, dodos_residos
  ld bc, dodos_residosEnd-dodos_residos
  di
  ldir
  ei

  ld hl, fputc_cons_rst
  ld de, 0xC000-printLn2End+printLn2-dosload_residosEnd+dosload_residos-dodos_residosEnd+dodos_residos-fputc_cons_rstEnd+fputc_cons_rst
  ld (fputc_cons+1), de
  ld bc, fputc_cons_rstEnd-fputc_cons_rst
  di
  ldir
  ei

  ; is 128k present?
  ld a, (memoryType)
  cp 0
  jr nz, resigot128

  ld a, 0x02  ; '2'
  jp dosload

resigot128:
  ld a, 0x03  ; '3'
  jp dosload

;-------------------------------------
; plus3check

plus3check:
  ld a, 0xff
  ld (ERR_NR), a  ; clear error

  ; is plus3dos present?
  ld a, (memoryType)
  cp 2
  jr nz, plus3fail

  ; this will now succeed so we can update the basic program
  call reenter

  ; copy all the pieces of code we'll need to continue with into the right places
  ld hl, printLn2
  ld de, 0xC000-printLn2End+printLn2
  ld bc, printLn2End-printLn2
  di
  ldir
  ei

  ld hl, dosload_residos
  ld de, 0xC000-printLn2End+printLn2-dosload_residosEnd+dosload_residos
  ld (dosload+1), de
  ld bc, dosload_residosEnd-dosload_residos
  di
  ldir
  ei

  ld hl, dodos_plus3
  ld de, 0xC000-printLn2End+printLn2-dosload_residosEnd+dosload_residos-dodos_plus3End+dodos_plus3
  ld (dodos+1), de
  ld bc, dodos_plus3End-dodos_plus3
  di
  ldir
  ei

  ld hl, fputc_cons_rst
  ld de, 0xC000-printLn2End+printLn2-dosload_residosEnd+dosload_residos-dodos_plus3End+dodos_plus3-fputc_cons_rstEnd+fputc_cons_rst
  ld (fputc_cons+1), de
  ld bc, fputc_cons_rstEnd-fputc_cons_rst
  di
  ldir
  ei

  ld a, 0x01  ; overlay page '1'
  jp dosload

plus3fail:
  ; if its not then show a message
  ld bc, failProgEnd-failProg
  ld de, failProg
  jr loadBasic

;-------------------------------------
; reenter - replace the loaded program to free some ram and avoid running the startup code more than once

reenter:
  ld bc, newProgEnd-newProg
  ld de, newProg

loadBasic:
  ;set NEWPPC to $000a (line 10)
  ld hl, $000a
  ld (NEWPPC), hl

  ;set NSPPC to 0 (first statement on line 10)
  push af
  ld a, 0
  ld (NSPPC), a
  pop af

  ;set VARS to prog + (newProgEnd-newProg)
  ld hl, (PROG)
  add hl, bc
  ld (VARS), hl

  ;set NXTLIN to prog + (newProgEnd-newProg) - 2
  dec hl
  dec hl
  ld (NXTLIN), hl

  ;copy the new program
  ld hl, (PROG)
  ex de, hl
  di
  ldir
  ei
  ret

;---------------------------------

newProg:
  ;10 CLEAR VAL "46000":PRINT"Filename?":INPUT LINE a$:RANDOMIZE USR VAL "49152"
  defb $00, $0a, $27, $00, $fd, $b0, $22, $34, $36, $30, $30, $30, $22, $3a
  defb $f5, $22, "Filename?", $22, $3a
  defb $ee, $ca, $61, $24, $3a
  defb $f9, $c0, $b0, $22, $34, $39, $31, $35, $32, $22, $0d, $ff, $0d, $80
newProgEnd:

failProg:
  ;10 PRINT "Either ESXDOS, ResiDOS v1.40+ or PLUS3DOS is required"
  defb $00, $0a, $39, $00, $f5, $22, "Either ESXDOS, ResiDOS v1.40+ or PLUS3DOS is required", $22, $0d, $ff, $0d, $80
failProgEnd:

fputc_cons_rst:
  binary "fputc_cons_rst.bin"
fputc_cons_rstEnd:

dodos_plus3:
  binary "dodos_plus3.bin"
dodos_plus3End:

dodos_residos:
  binary "dodos_residos.bin"
dodos_residosEnd:

dosload_residos:
  binary "dosload.bin"
dosload_residosEnd:

dosload_esxdos:
  binary "dosload_esxdos.bin"
dosload_esxdosEnd:

printLn2:
  binary "printLn.bin"
printLn2End:
