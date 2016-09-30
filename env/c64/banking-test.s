
; EasyFlashSDK sample code
; see README for a description details

* = $0000

EASYFLASH_BANK    = $DE00
EASYFLASH_CONTROL = $DE02
EASYFLASH_LED     = $80
EASYFLASH_16K     = $07
EASYFLASH_KILL    = $04

;.export	initenv

; =============================================================================
; 00:0:0000 (LOROM, bank 0)
bankstart_00_0:
  ; This resides on LOROM, it becomes visible at $8000, but is not used
  !pseudopc $8000 {
    ; fill the whole bank with value $ff
    !align $ffff, $a000, $ff
  }

; =============================================================================
; 00:1:0000 (HIROM, bank 0)
bankstart_00_1:
  ; This code runs in Ultimax mode after reset, so this memory becomes
  ; visible at $E000..$FFFF first and must contain a reset vector
  !pseudopc $e000 {

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
    ldx #(startUpEnd - startUpCode)
l1:
    lda startUpCode, x
    sta $0100, x
    dex
    bpl l1
    jmp $0100

startUpCode:
    !pseudopc $0100 {
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
      lda main-$4000,x  ;in the switch from ultimax to 16k cartridge mode $4000 needs to be subtracted
      sta $c000,x
      dex
      bne lp1
      inc lp1+2
      inc lp1+5
      lda lp1+5
      eor #$c2
      bne lp1 
      jmp entry

kill:
      lda #EASYFLASH_KILL
      sta EASYFLASH_CONTROL
      jmp ($fffc) ; reset
    }

startUpEnd:

main:
    !pseudopc $C000 {
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
      !pet "       querycsv by paul humphreys", 13, 0

      ; jump table with our own [if], [rem] and [sys] statements.
      ; [if] and [rem] necessary to make the new jump table work properly
A00C:
      !word $A830
      !word $A741
      !word $AD1D
      !word $A8F7
      !word $ABA4
      !word $ABBE
      !word $B080
      !word $AC05
      !word $A9A4
      !word $A89F
      !word $A870
      !word A928-1
      !word $A81C
      !word $A882
      !word $A8D1
      !word A93B-1
      !word $A82E
      !word $A94A
      !word $B82C
      !word $E167
      !word $E155
      !word $E164
      !word $B3B2
      !word $B823
      !word $AA7F
      !word $AA9F
      !word $A856
      !word $A69B
      !word $A65D
      !word $AA85
      !word E12A-1
      !word $E1BD
      !word $E1C6
      !word $AB7A
      !word $A641

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

AA9A:
      jsr $AB21     ; Output String
      lda $01       ; 6510 On-chip 8-bit Input/Output Register
      ora #$07
      and #$FE
      sta $01       ; 6510 On-chip 8-bit Input/Output Register
      lda #1
      sta EASYFLASH_BANK
      lda #EASYFLASH_16K
      sta EASYFLASH_CONTROL
      jmp foobar

jumpback:
      lda $01       ; 6510 On-chip 8-bit Input/Output Register
      ora #$07
      sta $01       ; 6510 On-chip 8-bit Input/Output Register
      lda #EASYFLASH_KILL
      sta EASYFLASH_CONTROL
      rts

initenv:
      lda #0
      tax
      rts
    }

    ; fill it up to $FFFA to put the vectors there
    !align $ffff, $fffa, $ff

    !word reti        ; NMI
    !word coldstart   ; RESET

    ; we don't need the IRQ vector and can put RTI here to save space :)
reti:
    rti
    !byte 0xff
  }

; =============================================================================
; 01:0:0000 (LOROM, bank 1)
bankstart_01_0:
  ; This resides on LOROM, but never becomes visible
  !pseudopc $8000 {
    ; fill the whole bank with value $ff
    !align $ffff, $a000, $ff
  }

; =============================================================================
; 01:1:0000 (HIROM, bank 1)
bankstart_01_1:
  ; This resides on HIROM, it becomes visible at $a000
  !pseudopc $a000 {

foobar:
    lda $41
    sta $0400
    jmp jumpback

    ; fill the whole bank with value $ff
    !align $ffff, $c000, $ff
  }
