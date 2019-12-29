HOOK_PACKAGE equ 0xfb

PKG_RESIDOS equ 0
PKG_IDEDOS equ 0x01

include "equs.inc"

;-------------------------------------
; dodos_residos

dodos_residos:
  exx
  ld b, PKG_IDEDOS
  jr testbanks

doresi2:
  exx
  ld b, PKG_RESIDOS

testbanks:
  push af
  ld a, (basicBank)
  di
  call mypager
  ei
  pop af

  push iy
  pop hl
  rst RST_HOOK
  defb HOOK_PACKAGE

  push af
  ld a, (defaultBank) ; page 0
  di
  call mypager
  ei
  pop af

  ld iy, ERR_NR
  ret

end: