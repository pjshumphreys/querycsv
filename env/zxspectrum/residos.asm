org 0xc000
  
  ld de, mypager   ; location for paging routine
  ld iy, RESI_GETPAGER  ; get paging routine
  call doresi
  jr nc, failed   ; call failed if Fc=0

  ....        ; more program
  di        ; disable interrupts
  ld a, (mybank)    ; bank obtained by RESI_ALLOC
  call mypager     ; switch it in to $0000-$3fff
  ....        ; more program
  ld a, (basicbank)   ; bank obtained by RESI_FINDBASIC
  call mypager     ; switch BASIC ROM back in
  ei        ; re-enable interrupts
  ....        ; more program

failed:
  ret

basicbank:
  defb 0x00

mypager:
  defs  32      ; 32 bytes to hold paging routine

doresi:
  exx
  push iy
  pop hl
  ld b, PKG_RESIDOS
  rst RST_HOOK
  defb HOOK_PACKAGE
  ld iy, 23610
  ret

println:   
  ld a, 2  ; upper screen
  call 0x1601  ; open channel
  ld hl, msg
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

msg:
  defb "Hello, World!\n",0

backup:
  defb 0x00
