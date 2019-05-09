writeStr equ 0x9
BDOS equ 0x5

org 0x0100
  ld c, writeStr
  ld de, msg
  call BDOS
  rst 0
msg:
   defb "Hello World!$"
