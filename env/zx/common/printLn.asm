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
PUBLIC isr
PUBLIC fputc_cons
PUBLIC libcRet

PUBLIC pagename

port1 equ 0x7ffd  ; address of ROM/RAM switching port in I/O map
bankm equ 0x5b5c  ; system variable that holds the last value output to 7FFDh
ERR_NR equ 0x5c3a   ; BASIC system variables
SCR_CT equ 0x5c8c
RST_HOOK equ 8

PUBLIC port1
PUBLIC bankm
PUBLIC ERR_NR
PUBLIC SCR_CT

;-------------------------------------
;printNoFile

printNoFile:
  ld a, 0x0a
  ld (pagename+12), a
  ld de, noopen
  call printLn

  ; quit with Invalid I/O device error
  ld hl, invalidFile
  jp atexit

invalidFile:
  ; trigger an 'invalid file name' error
  rst RST_HOOK
  defb 0x0e ; invalid file name

noopen:
  defb "Couldn't open "
pagename:
  defb "qcsv00zx.ovl", 0, 0

;---------------------------------
; more variables

programName:
  defb "querycsv", 0

argv:
  defw programName
argName:
  defw 0x0000
  defw 0x0000

defaultDrive: ; the default drive letter used by esxdos
  defb 0

exhlBackup:
  defw 0

spBackup:
  defw 0

skip_count: ; variable used by fputc_cons_rst. Located here so the code itself can be relocatable
  defb 0

;---------------------------------
; call_rom3
jp_rom3:
  ld (hlBackup), hl
  pop hl
  push de
  ld e, (hl)
  inc hl
  ld d, (hl)
  ld (jumptoit+1), de
  pop de
  jr call_rom3b

call_rom3:
  ld (hlBackup), hl
  pop hl
  push de
  ld e, (hl)
  inc hl
  ld d, (hl)
  ld (jumptoit+1), de
  inc hl
  pop de
  push hl

call_rom3b:
  ld hl, (hlBackup)
  push af
  di
  ld a, (basicBank)
  call mypager

  ld a, (bankm)  ; RAM/ROM switching system variable
  ld (bankmBackup), a
  or 0x17  ; want RAM page 7 and basic rom
  call switchPage
  pop af

  call jumptoit

  push af
  di
  ld a, (defaultBank) ; low page 0
  call mypager  ; switch it in to $0000-$3fff
  ld a, (bankmBackup) ; go back to whichever page was switched in before
  call switchPage
  pop af
  ret

;---------------------------------
; switchPage - switch the high bank to the one specified in the accumulator
; the de and hl registers will be left intact

switchPage:
  ld (bankm), a  ; keep system variables up to date
  push bc
  ld bc, port1  ; port used for horiz ROM switch and RAM paging
  out (c), a  ; RAM page 7 to top and DOS ROM
  pop bc
  ei ; enable interupts here so the ret statement below can quit the entire program with interupts enabled. This makes the code a few bytes smaller as well
  ret

;------------------------------------------------
; isr jump vector - added just in case we need it

isr: ; this must be at exactly 0xbfbf (ie 0xc000 - 0x41 or 65 bytes from the end of printLn.asm)
  jp exit ; just return for now. This jump table entry will get changed later.

;-------------------------------------
; variables

hlBackup:
  defw 0

deBackup:
  defw 0

bcBackup:
  defw 0

;0x0000-0x4000 mb02 banks
basicBank:    ; stores an identifier for the basic rom at 0x0000-0x4000
  defb 0

defaultBank:  ; stores an identifier for the scratchpad working memory at 0x0000-0x4000.
bankmBackup2: ; only used by plus 3 variant to save bankm internally. plus3 dos can't use low banks either
   defb 0

;0xc000-0xffff zx 128k banks
bankmBackup:
  defb 0

destinationHighBank: ; The physical 128k memory bank (0-7) that data should be copied into
  defb 0

;0xc000-0xffff virtual pages
loadVirtualPage: ; which virtual page to load into 0xc000-0xffff
  defb 0

currentVirtualPage: ; which virtual page currently is loaded into the memory at 0xc000-0xffff
  defb 0

libcRet:  ; backup of the return address when calling a libc function
  defw 0

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
  ret   ; printLn = 21 bytes

;-----------------------------
; generic jump table

mypager:
  jp exit ; just return for now. This jump table entry will get changed later.

doresi:
  jp exit ; just return for now. This jump table entry will get changed later.

dodos:
  jp exit ; just return for now. This jump table entry will get changed later.

atexit:
  jp exit ; just return for now. This jump table entry will get changed later.
  ld hl, 0 ; making space for later
  ;ld hl, 0
  ;call call_rom3
  defw 0xe60e  ; atexit location in page 7.

fputc_cons:
  jp exit ; just return for now. This jump table entry will get changed later.
  defw 0xec20  ; fputc_cons location in page 7. making space to use call call_rom3 later

dosload:
  jp exit ; just return for now. This jump table entry will get changed later.

jumptoit:
  jp $0000

pageStart: ; This must be equal to 0xc000
