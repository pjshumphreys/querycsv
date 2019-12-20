include "equs.inc"

;-------------------------------------
; dodos_plus3

dodos_plus3:
;
;IY holds the address of the DOS routine to be run. All other registers are
;passed intact to the DOS routine and are returned from it.
;
;Stack is somewhere in central 32K (conforming to DOS requirements), so save AF
;and BC will not be switched out.
;
  push af
  ld a, (bankm)  ; RAM/ROM switching system variable
  ld (bankmBackup), a
  or 7  ; want RAM page 7
  res 4, a  ; and DOS ROM
  di
  call switchPage
  ei
  pop af

  call jumptoit2  ; go sub routine address in IY

  push af  ; return from JP (IY) will be to here
  ld a, (bankmBackup)
  di
  call switchPage
  ei
  pop af

  ld iy, ERR_NR
  ret

jumptoit2:
  jp (iy)