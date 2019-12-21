PUBLIC mypager
PUBLIC dodos
PUBLIC printNoFile
PUBLIC call_rom3
PUBLIC switchPage

PUBLIC bankmBackup
PUBLIC basicBank
PUBLIC defaultBank
PUBLIC defaultDrive
PUBLIC destinationHighBank
PUBLIC skip_count

PUBLIC pagename

port1 equ 0x7ffd  ; address of ROM/RAM switching port in I/O map
bankm equ 0x5b5c  ; system variable that holds the last value output to 7FFDh
ERR_NR equ 0x5c3a   ; BASIC system variables
SCR_CT equ 0x5c8c
invalidFile equ 13

PUBLIC port1
PUBLIC bankm
PUBLIC ERR_NR
PUBLIC SCR_CT

org 0xbf5f

;-------------------------------------
;printNoFile

printNoFile:
  ld a, 0x0a
  ld (pagename+12), a
  ld de, noopen
  call printLn

  ; quit with Invalid I/O device error
  ld hl, invalidFile ; +13
  jp atexit

noopen:
  defb "Couldn't open "
pagename:
  defb "qrycsv00.ovl", 0, 0

;---------------------------------
; call_rom3

call_rom3:
  pop hl
  push de
  ld d, (hl)
  inc hl
  ld e, (hl)
  ld (jumptoit+1), de
  inc hl
  pop de
  push hl

  push af
  di
  ld a, (basicBank)
  call mypager

  ld a, (bankm)  ; RAM/ROM switching system variable
  ld (bankmBackup), a
  or 7  ; want RAM page 7
  call switchPage
  ei
  pop af

  call jumptoit

  push af
  di
  ld a, (bankmBackup) ; go back to whichever page was switched in before
  call switchPage

  ld a, (defaultBank) ; page 0
  call mypager  ; switch it in to $0000-$3fff
  ei
  pop af
  ret

jumptoit:
  jp $0000

;---------------------------------
; switchPage

switchPage:
  ld (bankm), a  ; keep system variables up to date
  push bc
  ld bc, port1  ; port used for horiz ROM switch and RAM paging
  out (c), a  ; RAM page 7 to top and DOS ROM
  pop bc
  ret

;-------------------------------------
;printLn

printLn:
  ld a, 2  ; upper screen
  call call_rom3
  defw 0x1601  ; open channel
loop:
  ld a, (de)
  or a
  jr z, exit
  ld b, a
  ;ld c, a
  push bc
  call fputc_cons
  pop bc
  inc de
  jr loop
exit:
  ret

;-------------------------------------
; variables

skip_count:
  defb 0

defaultDrive:
  defb 0

bankmBackup:
  defb 0

currentPage:
  defb 0

destinationHighBank:
  defb 0

defaultBank:
  defb 0

basicBank:
  defb 0

;-----------------------------
; generic jump table

mypager:
  jp exit ; just return for now. This jump table entry will get changed later.

doresi:
  jp exit ; just return for now. This jump table entry will get changed later.

atexit:
  jp exit ; just return for now. This jump table entry will get changed later.

farcall:
  jp exit ; just return for now. This jump table entry will get changed later.

fputc_cons:
  jp exit ; just return for now. This jump table entry will get changed later.

dodos:
  jp exit ; just return for now. This jump table entry will get changed later.

dosload:
  jp exit ; just return for now. This jump table entry will get changed later.

pageStart: ; This must be equal to 0xc000