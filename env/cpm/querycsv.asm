writeStr equ 0x9
BDOS equ 0x5

org 0x0100
start:
  ; ms-dos protection
  defb 0xeb, 0x04  ; jmps label
  ex de, hl
  jp chkz80-start+0x0100
  defb 0xb4, 0x09  ; mov ah, 9
  defb 0xba
  defw dosmsg  ; mov dx, offset dosmsg
  defb 0xcd, 0x21  ; int 0x21
  defb 0xcd, 0x20  ; int 0x20

chkz80:
  ; 8080 protection
  ld a, 0x7f  ; put 01111111 into accumulator
  inc a  ; make it overflow ie. 10000000
  jp pe, begin-start+0x0100  ; only 8080 resets for odd parity here
  ld de, z80msg  ; print "Z80 CPU required" message
  jr print

begin:
  ld a, 1
  ld c, 06fh
  call BDOS    ; Get MSX-DOS version
  or a
  jr nz, CPM
  ld a, d
  cp 2
  jr nz, DOS1

DOS2:
  ld de, dos2msg
  jr print

DOS1:
  ld de, dos1msg
  jr print

CPM:
  ld de, cpmmsg

print:
  ld c, writeStr
  jp BDOS

dosmsg:
  defm  "This program is for a CP/M system."
  defb  13, 10, '$'

z80msg:
  defm  "Z80 CPU required."
  defb  13, 10, '$'

cpmmsg:
   defb "CP/M$"

dos1msg:
   defb "MSXDOS 1$"

dos2msg:
   defb "MSXDOS 2$"
