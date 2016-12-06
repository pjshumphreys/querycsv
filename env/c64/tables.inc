.export _stdin
.export _stdout
.export _stderr
.export BASIC_FAC_to_string
.export BASIC_string_to_FAC
.export BASIC_s8_to_FAC
.export BASIC_u8_to_FAC
.export BASIC_u16_to_FAC
.export BASIC_s16_to_FAC
.export BASIC_FAC_to_u16
.export __fabs
.export __fatn
.export __fcos
.export __fexp
.export __fint
.export __flog
.export __frnd
.export __fsgn
.export __fsin
.export __fsqr
.export __ftan
.export __fnot
.export __fround
.export __fadd
.export __fsub
.export __fmul
.export __fdiv
.export __fpow
.export __fand
.export __for
.export BASIC_FAC_cmp
.export BASIC_FAC_testsgn
.export BASIC_FAC_Poly1
.export BASIC_FAC_Atn
.export BASIC_ARG_FAC_Div
.export BASIC_ARG_FAC_Add
.export __ftostr
.export __strtof
.export __ctof
.export __utof
.export __stof
.export __itof
.export __ftoi
.export __fcmp
.export __ftestsgn
.export __fneg
.export __fpoly1
.export __fpoly2
.export __fatan2
.export _calloc
.export _clearerr
.export _close
.export _fclose
.export _feof
.export _ferror
.export _fflush
.export _fgetc
.export _fopen
.export _fprintf
.export _fputc
.export _fputs
.export _fread
.export _free
.export _fwrite
.export _getenv
.export _ltoa
.export _malloc
.export _memmove
.export _memset
.export _open
.export _printf
.export _putenv
.export _read
.export _realloc
.export _setlocale
.export _sprintf
.export _strcat
.export _strcmp
.export _strcpy
.export _stricmp
.export _strlen
.export _strlower
.export _strncat
.export _strncmp
.export _strstr
.export _ultoa
.export _ungetc
.export _vfprintf
.export _vsnprintf
.export _vsprintf
.export _write

_stderr = $0913
_stdin = $090F
_stdout = $0911
initlib2 = $A3F4
___float_fac_to_float = $A0AC
___float_float_to_fac = $A01B
___float_float_to_fac_arg = $A061
aRegBackup = $0820
xRegBackup = $081F
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
__ftostr:
  stx xRegBackup
  ldx #$21
  jmp farcall
__strtof:
  stx xRegBackup
  ldx #$22
  jmp farcall
__ctof:
  stx xRegBackup
  ldx #$23
  jmp farcall
__utof:
  stx xRegBackup
  ldx #$24
  jmp farcall
__stof:
  stx xRegBackup
  ldx #$25
  jmp farcall
__itof:
  stx xRegBackup
  ldx #$26
  jmp farcall
__ftoi:
  stx xRegBackup
  ldx #$27
  jmp farcall
__fcmp:
  stx xRegBackup
  ldx #$28
  jmp farcall
__ftestsgn:
  stx xRegBackup
  ldx #$29
  jmp farcall
__fneg:
  stx xRegBackup
  ldx #$2A
  jmp farcall
__fpoly1:
  stx xRegBackup
  ldx #$2B
  jmp farcall
__fpoly2:
  stx xRegBackup
  ldx #$2C
  jmp farcall
__fatan2:
  stx xRegBackup
  ldx #$2D
  jmp farcall
_calloc:
  stx xRegBackup
  ldx #$2E
  jmp farcall2
_clearerr:
  stx xRegBackup
  ldx #$2F
  jmp farcall2
_close:
  stx xRegBackup
  ldx #$30
  jmp farcall2
_fclose:
  stx xRegBackup
  ldx #$31
  jmp farcall2
_feof:
  stx xRegBackup
  ldx #$32
  jmp farcall2
_ferror:
  stx xRegBackup
  ldx #$33
  jmp farcall2
_fflush:
  stx xRegBackup
  ldx #$34
  jmp farcall2
_fgetc:
  stx xRegBackup
  ldx #$35
  jmp farcall2
_fopen:
  stx xRegBackup
  ldx #$36
  jmp farcall2
_fprintf:
  stx xRegBackup
  ldx #$37
  jmp farcall2
_fputc:
  stx xRegBackup
  ldx #$38
  jmp farcall2
_fputs:
  stx xRegBackup
  ldx #$39
  jmp farcall2
_fread:
  stx xRegBackup
  ldx #$3A
  jmp farcall2
_free:
  stx xRegBackup
  ldx #$3B
  jmp farcall2
_fwrite:
  stx xRegBackup
  ldx #$3C
  jmp farcall2
_getenv:
  stx xRegBackup
  ldx #$3D
  jmp farcall2
_ltoa:
  stx xRegBackup
  ldx #$3E
  jmp farcall2
_malloc:
  stx xRegBackup
  ldx #$3F
  jmp farcall2
_memmove:
  stx xRegBackup
  ldx #$40
  jmp farcall2
_memset:
  stx xRegBackup
  ldx #$41
  jmp farcall2
_open:
  stx xRegBackup
  ldx #$42
  jmp farcall2
_printf:
  stx xRegBackup
  ldx #$43
  jmp farcall2
_putenv:
  stx xRegBackup
  ldx #$44
  jmp farcall2
_read:
  stx xRegBackup
  ldx #$45
  jmp farcall2
_realloc:
  stx xRegBackup
  ldx #$46
  jmp farcall2
_setlocale:
  stx xRegBackup
  ldx #$47
  jmp farcall2
_sprintf:
  stx xRegBackup
  ldx #$48
  jmp farcall2
_strcat:
  stx xRegBackup
  ldx #$49
  jmp farcall2
_strcmp:
  stx xRegBackup
  ldx #$4A
  jmp farcall2
_strcpy:
  stx xRegBackup
  ldx #$4B
  jmp farcall2
_stricmp:
  stx xRegBackup
  ldx #$4C
  jmp farcall2
_strlen:
  stx xRegBackup
  ldx #$4D
  jmp farcall2
_strlower:
  stx xRegBackup
  ldx #$4E
  jmp farcall2
_strncat:
  stx xRegBackup
  ldx #$4F
  jmp farcall2
_strncmp:
  stx xRegBackup
  ldx #$50
  jmp farcall2
_strstr:
  stx xRegBackup
  ldx #$51
  jmp farcall2
_ultoa:
  stx xRegBackup
  ldx #$52
  jmp farcall2
_ungetc:
  stx xRegBackup
  ldx #$53
  jmp farcall2
_vfprintf:
  stx xRegBackup
  ldx #$54
  jmp farcall2
_vsnprintf:
  stx xRegBackup
  ldx #$55
  jmp farcall2
_vsprintf:
  stx xRegBackup
  ldx #$56
  jmp farcall2
_write:
  stx xRegBackup
  ldx #$57
  jmp farcall2

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
.byte $A1
.byte $A1
.byte $A1
.byte $A1
.byte $A1
.byte $A1
.byte $A1
.byte $A1
.byte $A1
.byte $A1
.byte $A2
.byte $A2
.byte $A2
.byte $A7
.byte $A9
.byte $A4
.byte $A8
.byte $A9
.byte $A9
.byte $BD
.byte $A8
.byte $A9
.byte $A9
.byte $A9
.byte $AA
.byte $AA
.byte $AB
.byte $AD
.byte $AE
.byte $AE
.byte $AF
.byte $B0
.byte $B0
.byte $A6
.byte $B4
.byte $B5
.byte $A7
.byte $B5
.byte $AE
.byte $B7
.byte $B7
.byte $B8
.byte $B8
.byte $B8
.byte $B8
.byte $B8
.byte $B8
.byte $B9
.byte $B9
.byte $AE
.byte $B9
.byte $BA
.byte $BA
.byte $BB
.byte $A7

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
.byte $53
.byte $6F
.byte $89
.byte $95
.byte $A1
.byte $B4
.byte $C0
.byte $CD
.byte $E3
.byte $EE
.byte $01
.byte $10
.byte $1F
.byte $FF
.byte $0E
.byte $95
.byte $23
.byte $1A
.byte $24
.byte $91
.byte $62
.byte $B3
.byte $CB
.byte $F6
.byte $84
.byte $EE
.byte $C1
.byte $94
.byte $11
.byte $99
.byte $2C
.byte $7B
.byte $D4
.byte $5A
.byte $D5
.byte $08
.byte $05
.byte $B1
.byte $2C
.byte $AF
.byte $DA
.byte $12
.byte $39
.byte $58
.byte $9D
.byte $B3
.byte $DB
.byte $30
.byte $6F
.byte $F0
.byte $C7
.byte $2D
.byte $56
.byte $1E
.byte $B7

bankTable:
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $00
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
.byte $02
