
; EasyFlashSDK sample code
; see README for a description details

EASYFLASH_BANK    = $DE00
EASYFLASH_CONTROL = $DE02
EASYFLASH_LED     = $80
EASYFLASH_16K     = $07
EASYFLASH_KILL    = $04

;.export _exit
;.export __STARTUP__ : absolute = 1      ; Mark as startup

.import _main

.importzp       ptr1, ptr2, tmp1
.import donelib
.import zerobss
.import BSOUT
.import __RAM_START__, __RAM_SIZE__     ; Linker generated
.import __LOADER_LOAD__
.import __STARTUP_LOAD__
.import __RAM2_LAST__, __RAM2_START__, __ROML0_START__

.export entry,premain
.export farcall2, farret, farret2

__LOADER_LOAD2__  = __LOADER_LOAD__+$4000

.segment "DATA"
.incbin "libcdata.bin"

.segment "LIBC"
.incbin "libc.bin"

; This code runs in Ultimax mode after reset, so this memory becomes
; visible at $E000..$FFFF first and must contain a reset vector
.segment  "ULTIMAX"

coldstart:
    ; === the reset vector points here ===
    sei
    ldx #$ff
    txs
    cld

    ; enable VIC (e.g. RAM refresh)
    lda #8
    sta $d016

    ; write to RAM to make sure it starts up correctly (=> RAM datasheets)
startWait:
    sta $0100, x
    dex
    bne startWait

    ; copy the final start-up code to RAM (bottom of CPU stack)
    ldx #(startUpEnd - startUpBegin)
l1:
    lda __LOADER_LOAD2__, x
    sta $0100, x
    dex
    bpl l1
    jmp $0100

.segment  "LOADER"

startUpBegin:

  ; === this code is copied to the stack area, does some inits ===
  ; === scans the keyboard and kills the cartridge or          ===
  ; === starts the main application                            ===
  lda #EASYFLASH_16K + EASYFLASH_LED
  sta EASYFLASH_CONTROL

  ; Check if one of the magic kill keys is pressed
  ; This should be done in the same way on any EasyFlash cartridge!

  ; Prepare the CIA to scan the keyboard
  lda #$7f
  sta $dc00   ; pull down row 7 (DPA)

  ldx #$ff
  stx $dc02   ; DDRA $ff = output (X is still $ff from copy loop)
  inx
  stx $dc03   ; DDRB $00 = input

  ; Read the keys pressed on this row
  lda $dc01   ; read coloumns (DPB)

  ; Restore CIA registers to the state after (hard) reset
  stx $dc02   ; DDRA input again
  stx $dc00   ; Now row pulled down

  ; Check if one of the magic kill keys was pressed
  and #$e0    ; only leave "Run/Stop", "Q" and "C="
  cmp #$e0
  bne kill    ; branch if one of these keys is pressed

  ldx #0
  stx $D016     ; VIC: Control Register 2
  jsr $ff84     ; Initialise I/O

  ; === the main application entry point ===
  ; copy the main code to $C000 (or whereever) - we don't run it here
  ; since the banking would make it invisible
  ; it may be a good idea to let exomizer do this in real life
  ldx #0
lp1:
  lda __STARTUP_LOAD__,x
  sta $c000,x
  dex
  bne lp1
  inc lp1+2
  inc lp1+5
  lda lp1+5
  eor #$cc
  bne lp1 
  jmp entry

kill:
  lda #EASYFLASH_KILL
  sta EASYFLASH_CONTROL
  jmp ($fffc) ; reset

startUpEnd:

.segment "STARTUP"
farret:
  jmp farret3
.include "tables.inc"

;entry point is here!
entry:
  ;disable easyflash cartridge as we want to hook into basic
  lda #EASYFLASH_KILL
  sta EASYFLASH_CONTROL
  ;jsr $ff87     ; Initialise System Constants
  lda #$00
  tay

lp2:
  sta $0002,Y
  sta $0200,Y
  sta $0300,Y
  iny
  bne lp2

  ldx #$00
  ldy #$A0
  jsr $fd8c
  jsr $ff8a     ; Restore Kernal Vectors
  jsr $ff81     ; Initialize screen editor
  cli

  ;do 'normal' system setup
  jsr $E453     ; Initialize Vectors
  jsr $E3BF     ; Initialize BASIC RAM
  jsr $E422     ; Output Power-Up Message

  ;wedge the exec next BASIC command entry point
  lda #<A7E1
  sta $0308
  lda #>A7E1
  sta $0309

  ;output our startup message
  lda #<message
  ldy #>message
  jsr $AB1E     ; Output String

  ;start BASIC
  ldx #$FB
  txs
  bne E386     ; BASIC Warm start [RUNSTOP-RESTORE]
  jmp $E3A2

E386:
  jmp $E386

message:
  .byte "       querycsv by paul humphreys", 13, 0

  ; jump table with our own [if], [rem] and [sys] statements.
  ; [if] and [rem] necessary to make the new jump table work properly
A00C:
  .word $A830
  .word $A741
  .word $AD1D
  .word $A8F7
  .word $ABA4
  .word $ABBE
  .word $B080
  .word $AC05
  .word $A9A4
  .word $A89F
  .word $A870
  .word A928-1
  .word $A81C
  .word $A882
  .word $A8D1
  .word A93B-1
  .word $A82E
  .word $A94A
  .word $B82C
  .word $E167
  .word $E155
  .word $E164
  .word $B3B2
  .word $B823
  .word $AA7F
  .word $AA9F
  .word $A856
  .word $A69B
  .word $A65D
  .word $AA85
  .word E12A-1
  .word $E1BD
  .word $E1C6
  .word $AB7A
  .word $A641

  ; Vector: BASIC Character dispatch Routine
A7E1:    
  jsr $0073     ; CHRGET: Get next Byte of BASIC Text
  jsr A7ED      ; Perform BASIC Keyword
  jmp $A7AE     ; BASIC Warm start

A7ED:
  beq A82B      ; Perform [restore]

  ;Jump from $A95C: ; ignored as the on statement can only do gosub or goto anyway

  sbc #$80
  bcc A804
  cmp #$23
  bcs A80E
  asl
  tay
  lda A00C+1,Y  ; BASIC Command Vectors - 1
  pha
  lda A00C,Y    ; BASIC Command Vectors - 1
  pha
  jmp $0073     ; CHRGET: Get next Byte of BASIC Text

A804:
  jmp $A9A5     ; Perform [let]

A80E:
  jmp $A80E

A82B:
  jmp $A82B

  ;$A928/43304:   Perform [if]
A928:
  jsr $AD9E     ; Evaluate Expression in Text
  jsr $0079     ; CHRGOT: Get same Byte again
  cmp #$89
  beq A937
  lda #$A7
  jsr $AEFF     ; Test ','

  ;Jump from $A930:
A937:
  lda $61       ; FAC Exponent
  bne A940      ; Perform [rem]

  ;$A93B/43323:   Perform [rem]
A93B:
  jsr $A909     ; Search for Next statement / Line
  beq A8FB      ; Perform [data]

  ;Jump from $A939:
A940:
  jsr $0079     ; CHRGOT: Get same Byte again
  bcs A948
  jmp $A8A0     ; Perform [goto]

  ;Jump from $A943:
A948:
  jmp A7ED      ; Perform BASIC Keyword

A8FB:
  jmp $A8FB     ; Perform [data]

  ;$E12A/57642:   Perform [sys]
E12A: 
  jsr $AD9E     ; Evaluate Expression in Text
  bit $0D       ; Data type Flag
  bmi AA9A      ; Print String From Memory
  jmp $E12D     ; continue where sys left off

;copy string into buffer above $C000 as everything in the BASIC memory
;area will be erased
AB21:
  jsr $B6A6
  tax
  ldy #$00
  inx

AB28:
  dex
  beq AAE7
  lda ($22),Y

  ;jsr $AB47  ;print a char
  sta strbuf, Y

  iny
  cmp #$0D
  bne AB28
  jsr $AAE5
  jmp AB28

AAE7:
  lda #0
  sta strbuf, Y
  rts

AA9A:
  jsr AB21
  lda $01       ; 6510 On-chip 8-bit Input/Output Register
  ora #$07
  sta $01       ; 6510 On-chip 8-bit Input/Output Register
  lda #1
  sta EASYFLASH_BANK
  lda #EASYFLASH_16K
  sta EASYFLASH_CONTROL

.include "zeropage.inc"
.include "c64.inc"
premain:
  lda #14
  jsr BSOUT

  jsr zerobss

  ; copydata won't work for us as it needs to do DATA *and* RODATA, so we roll our own memory copying code instead
  lda #0
  sta EASYFLASH_BANK

  ldx #<__ROML0_START__
  stx ptr1
  ldx #>__ROML0_START__
  stx ptr1+1
  ldx #<__RAM2_START__
  stx ptr2
  ldx #>__RAM2_START__
  stx ptr2+1

  datSize = __RAM2_LAST__ - __RAM2_START__

movedown:
  ldy #0
  ldx #>datSize
  beq MD2

MD1:
  lda (ptr1),Y ; move a page at a time
  sta (ptr2),Y
  iny
  bne MD1
  inc ptr1+1
  inc ptr2+1
  dex
  bne MD1

MD2:
  ldx #<datSize
  beq MD4

MD3:
  lda (ptr1),Y ; move the remaining bytes
  sta (ptr2),Y
  iny
  dex
  bne MD3

MD4:
  lda $01       ; 6510 On-chip 8-bit Input/Output Register
  ora #$07
  and #$FE
  sta $01       ; 6510 On-chip 8-bit Input/Output Register

  ; and here
  ; Set argument stack ptr
  lda #<($A000)
  sta sp
  lda #>($A000)
  sta sp + 1

  lda #2
  sta EASYFLASH_BANK
  jsr initlib2
  lda #1
  sta currentBank
  sta EASYFLASH_BANK
  ;jsr initlib
  jsr _main

_exit:
  jsr donelib

jumpback:
  lda $01       ; 6510 On-chip 8-bit Input/Output Register
  ora #$07
  and #$FE
  sta $01       ; 6510 On-chip 8-bit Input/Output Register
  lda #EASYFLASH_KILL
  sta EASYFLASH_CONTROL
  rts

farcall2:   ;backup the original return address then swap it for our paging out return code. This should hopefully work as all of the c standard library is in 1 page and doesn't call non stdlib c functions, so more than 1 backup return address shouldn't be needed
  sta aRegBackup
  pla 
  sta stackBackup+1
  pla
  sta stackBackup
  lda #>(farret2-1)
  pha
  lda #<(farret2-1)
  pha
  lda aRegBackup

farcall:
  sta aRegBackup
  lda highAddressTable, x
  pha
  lda lowAddressTable, x
  pha
  lda bankTable, x
  php
  pha
  lda currentBank
  tsx
  inx
  inx
  inx
  inx
  sta bankStack, x
  pla
  plp
  sta currentBank
  sta EASYFLASH_BANK
  lda aRegBackup
  ldx xRegBackup
  rts ; non local jmp to the real function

farret2:
  sta aRegBackup
  stx xRegBackup
  lda stackBackup
  pha
  lda stackBackup+1
  pha
  tsx
  lda bankStack, x
  sta currentBank
  sta EASYFLASH_BANK
  lda aRegBackup
  ldx xRegBackup
  rts

farret3:
  sta aRegBackup
  stx xRegBackup
  tsx
  lda bankStack, x
  sta currentBank
  sta EASYFLASH_BANK
  lda aRegBackup
  ldx xRegBackup
  rts

FUNC0:
  sta aRegBackup
  lda #>(__basicoff-1)
  pha
  lda #<(__basicoff-1)
  pha
  lda highAddressTable, x
  pha
  lda lowAddressTable, x
  pha
  lda aRegBackup
  ldx xRegBackup
  jmp __basicon

FUNC1:
  sta aRegBackup
  lda #>(__basicoff2-1)
  pha
  lda #<(__basicoff2-1)
  pha
  lda highAddressTable, x
  pha
  lda lowAddressTable, x
  pha
  lda #2
  sta EASYFLASH_BANK
  lda aRegBackup
  lda xRegBackup
  jsr ___float_float_to_fac    ; also pops pointer to float
  jmp __basicon

FUNC2:
  sta aRegBackup
  lda #>(__basicoff2-1)
  pha
  lda #<(__basicoff2-1)
  pha
  lda highAddressTable, x
  pha
  lda lowAddressTable, x
  pha
  lda #2
  sta EASYFLASH_BANK
  lda aRegBackup
  lda xRegBackup
  jsr ___float_float_to_fac_arg
  lda $61
  jmp __basicon

FUNC3:
  sta aRegBackup
  lda #>(__basicoff2-1)
  pha
  lda #<(__basicoff2-1)
  pha
  lda highAddressTable, x
  pha
  lda lowAddressTable, x
  pha
  lda #2
  sta EASYFLASH_BANK
  lda aRegBackup
  ldx xRegBackup
  jsr ___float_float_to_fac_arg

;---------------------------------------------------------------------------------------------

__basicon:
  sei
  ldx #EASYFLASH_KILL
  stx EASYFLASH_CONTROL
  ldx #$37
  stx $01
  rts ; non local jmp to the real function

__basicoff:
  ldx #EASYFLASH_16K
  stx EASYFLASH_CONTROL
  ldx #$36
  stx $01
  cli
  rts

__basicoff2:
  ldx #EASYFLASH_16K
  stx EASYFLASH_CONTROL
  ldx #$36
  stx $01
  cli
  jsr ___float_fac_to_float
  sta aRegBackup
  lda currentBank
  sta EASYFLASH_BANK
  lda aRegBackup
  rts

strbuf: ;stores the argument string for main
  .res 256

.segment "BSS"

currentBank:
  .byte $00
stackBackup:
  .word $0000
bankStack:
  .res 256

;reset vectors. these appear at $FFFA onwards when the machine is turned on
.segment "VECTORS"

.word reti        ; NMI
.word coldstart   ; RESET

reti:
  rti ; we don't need the IRQ vector and can put RTI here to save space :)
  .byte $FF
