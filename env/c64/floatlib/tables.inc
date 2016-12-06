BASIC_FAC_to_string:
  stx xRegBackup
  ldx #$00
  jmp FUNC0
BASIC_string_to_FAC:
  stx xRegBackup
  ldx #$01
  jmp FUNC0
BASIC_s8_to_FAC:
  stx xRegBackup
  ldx #$02
  jmp FUNC0
BASIC_u8_to_FAC:
  stx xRegBackup
  ldx #$03
  jmp FUNC0
BASIC_u16_to_FAC:
  stx xRegBackup
  ldx #$04
  jmp FUNC0
BASIC_s16_to_FAC:
  stx xRegBackup
  ldx #$05
  jmp FUNC0
BASIC_FAC_to_u16:
  stx xRegBackup
  ldx #$06
  jmp FUNC0
__fabs:
  stx xRegBackup
  ldx #$07
  jmp FUNC1
__fatn:
  stx xRegBackup
  ldx #$08
  jmp FUNC1
__fcos:
  stx xRegBackup
  ldx #$09
  jmp FUNC1
__fexp:
  stx xRegBackup
  ldx #$0A
  jmp FUNC1
__fint:
  stx xRegBackup
  ldx #$0B
  jmp FUNC1
__flog:
  stx xRegBackup
  ldx #$0C
  jmp FUNC1
__frnd:
  stx xRegBackup
  ldx #$0D
  jmp FUNC1
__fsgn:
  stx xRegBackup
  ldx #$0E
  jmp FUNC1
__fsin:
  stx xRegBackup
  ldx #$0F
  jmp FUNC1
__fsqr:
  stx xRegBackup
  ldx #$10
  jmp FUNC1
__ftan:
  stx xRegBackup
  ldx #$11
  jmp FUNC1
__fnot:
  stx xRegBackup
  ldx #$12
  jmp FUNC1
__fround:
  stx xRegBackup
  ldx #$13
  jmp FUNC1
__fadd:
  stx xRegBackup
  ldx #$14
  jmp FUNC2
__fsub:
  stx xRegBackup
  ldx #$15
  jmp FUNC2
__fmul:
  stx xRegBackup
  ldx #$16
  jmp FUNC2
__fdiv:
  stx xRegBackup
  ldx #$17
  jmp FUNC2
__fpow:
  stx xRegBackup
  ldx #$18
  jmp FUNC2
__fand:
  stx xRegBackup
  ldx #$19
  jmp FUNC3
__for:
  stx xRegBackup
  ldx #$1A
  jmp FUNC3
BASIC_FAC_cmp:
  stx xRegBackup
  ldx #$1B
  jmp FUNC0
BASIC_FAC_testsgn:
  stx xRegBackup
  ldx #$1C
  jmp FUNC0
BASIC_FAC_Poly1:
  stx xRegBackup
  ldx #$1D
  jmp FUNC0
BASIC_FAC_Atn:
  stx xRegBackup
  ldx #$1E
  jmp FUNC0
BASIC_ARG_FAC_Div:
  stx xRegBackup
  ldx #$1F
  jmp FUNC0
BASIC_ARG_FAC_Add:
  stx xRegBackup
  ldx #$20
  jmp FUNC0

highAddressTable:
.byte $BD
.byte $B7
.byte $BC
.byte $B3
.byte $BC
.byte $B3
.byte $BC
.byte $BC
.byte $E3
.byte $E2
.byte $BF
.byte $BC
.byte $B9
.byte $E0
.byte $BC
.byte $E2
.byte $BF
.byte $E2
.byte $AE
.byte $BC
.byte $B8
.byte $B8
.byte $BA
.byte $BB
.byte $BF
.byte $AF
.byte $AF
.byte $BC
.byte $BC
.byte $E0
.byte $E3
.byte $BB
.byte $B8

lowAddressTable:
.byte $DC
.byte $B4
.byte $3B
.byte $A1
.byte $48
.byte $94
.byte $9A
.byte $57
.byte $0D
.byte $63
.byte $EC
.byte $CB
.byte $E9
.byte $96
.byte $38
.byte $6A
.byte $70
.byte $B3
.byte $D3
.byte $1A
.byte $69
.byte $52
.byte $2A
.byte $11
.byte $7A
.byte $E8
.byte $E5
.byte $5A
.byte $2A
.byte $58
.byte $0D
.byte $11
.byte $69
