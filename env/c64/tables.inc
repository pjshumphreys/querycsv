.export _stderr
_stderr = $08C1
.export _stdin
_stdin = $08BD
.export _stdout
_stdout = $08BF
.export initlib2
initlib2 = $8034
.export ___float_fac_to_float
___float_fac_to_float = $A0A8
.export ___float_float_to_fac
___float_float_to_fac = $A01B
.export ___float_float_to_fac_arg
___float_float_to_fac_arg = $A05F
.export aRegBackup
aRegBackup = $0820
.export xRegBackup
xRegBackup = $081F
.export _numberEntry
_numberEntry = $0000
.export BASIC_FAC_to_string
BASIC_FAC_to_string:
  stx xRegBackup
  ldx #$00
  jmp FUNC0
.export BASIC_string_to_FAC
BASIC_string_to_FAC:
  stx xRegBackup
  ldx #$01
  jmp FUNC0
.export BASIC_s8_to_FAC
BASIC_s8_to_FAC:
  stx xRegBackup
  ldx #$02
  jmp FUNC0
.export BASIC_u8_to_FAC
BASIC_u8_to_FAC:
  stx xRegBackup
  ldx #$03
  jmp FUNC0
.export BASIC_u16_to_FAC
BASIC_u16_to_FAC:
  stx xRegBackup
  ldx #$04
  jmp FUNC0
.export BASIC_s16_to_FAC
BASIC_s16_to_FAC:
  stx xRegBackup
  ldx #$05
  jmp FUNC0
.export BASIC_FAC_to_u16
BASIC_FAC_to_u16:
  stx xRegBackup
  ldx #$06
  jmp FUNC0
.export __fabs
__fabs:
  stx xRegBackup
  ldx #$07
  jmp FUNC1
.export __fatn
__fatn:
  stx xRegBackup
  ldx #$08
  jmp FUNC1
.export __fcos
__fcos:
  stx xRegBackup
  ldx #$09
  jmp FUNC1
.export __fexp
__fexp:
  stx xRegBackup
  ldx #$0A
  jmp FUNC1
.export __fint
__fint:
  stx xRegBackup
  ldx #$0B
  jmp FUNC1
.export __flog
__flog:
  stx xRegBackup
  ldx #$0C
  jmp FUNC1
.export __frnd
__frnd:
  stx xRegBackup
  ldx #$0D
  jmp FUNC1
.export __fsgn
__fsgn:
  stx xRegBackup
  ldx #$0E
  jmp FUNC1
.export __fsin
__fsin:
  stx xRegBackup
  ldx #$0F
  jmp FUNC1
.export __fsqr
__fsqr:
  stx xRegBackup
  ldx #$10
  jmp FUNC1
.export __ftan
__ftan:
  stx xRegBackup
  ldx #$11
  jmp FUNC1
.export __fnot
__fnot:
  stx xRegBackup
  ldx #$12
  jmp FUNC1
.export __fround
__fround:
  stx xRegBackup
  ldx #$13
  jmp FUNC1
.export __fadd
__fadd:
  stx xRegBackup
  ldx #$14
  jmp FUNC2
.export __fsub
__fsub:
  stx xRegBackup
  ldx #$15
  jmp FUNC2
.export __fmul
__fmul:
  stx xRegBackup
  ldx #$16
  jmp FUNC2
.export __fdiv
__fdiv:
  stx xRegBackup
  ldx #$17
  jmp FUNC2
.export __fpow
__fpow:
  stx xRegBackup
  ldx #$18
  jmp FUNC2
.export __fand
__fand:
  stx xRegBackup
  ldx #$19
  jmp FUNC3
.export __for
__for:
  stx xRegBackup
  ldx #$1A
  jmp FUNC3
.export BASIC_FAC_cmp
BASIC_FAC_cmp:
  stx xRegBackup
  ldx #$1B
  jmp FUNC0
.export BASIC_FAC_testsgn
BASIC_FAC_testsgn:
  stx xRegBackup
  ldx #$1C
  jmp FUNC0
.export BASIC_FAC_Poly1
BASIC_FAC_Poly1:
  stx xRegBackup
  ldx #$1D
  jmp FUNC0
.export BASIC_FAC_Atn
BASIC_FAC_Atn:
  stx xRegBackup
  ldx #$1E
  jmp FUNC0
.export BASIC_ARG_FAC_Div
BASIC_ARG_FAC_Div:
  stx xRegBackup
  ldx #$1F
  jmp FUNC0
.export BASIC_ARG_FAC_Add
BASIC_ARG_FAC_Add:
  stx xRegBackup
  ldx #$20
  jmp FUNC0
.export __ftostr
__ftostr:
  stx xRegBackup
  ldx #$21
  jmp farcall
.export __strtof
__strtof:
  stx xRegBackup
  ldx #$22
  jmp farcall
.export __ctof
__ctof:
  stx xRegBackup
  ldx #$23
  jmp farcall
.export __utof
__utof:
  stx xRegBackup
  ldx #$24
  jmp farcall
.export __stof
__stof:
  stx xRegBackup
  ldx #$25
  jmp farcall
.export __itof
__itof:
  stx xRegBackup
  ldx #$26
  jmp farcall
.export __ftoi
__ftoi:
  stx xRegBackup
  ldx #$27
  jmp farcall
.export __fcmp
__fcmp:
  stx xRegBackup
  ldx #$28
  jmp farcall
.export __ftestsgn
__ftestsgn:
  stx xRegBackup
  ldx #$29
  jmp farcall
.export __fneg
__fneg:
  stx xRegBackup
  ldx #$2A
  jmp farcall
.export __fpoly1
__fpoly1:
  stx xRegBackup
  ldx #$2B
  jmp farcall
.export __fpoly2
__fpoly2:
  stx xRegBackup
  ldx #$2C
  jmp farcall
.export __fatan2
__fatan2:
  stx xRegBackup
  ldx #$2D
  jmp farcall
.export _strtod
_strtod:
  stx xRegBackup
  ldx #$2E
  jmp farcall
.export _calloc
_calloc:
  stx xRegBackup
  ldx #$2F
  jmp farcall2
.export _clearerr
_clearerr:
  stx xRegBackup
  ldx #$30
  jmp farcall2
.export _close
_close:
  stx xRegBackup
  ldx #$31
  jmp farcall2
.export _fclose
_fclose:
  stx xRegBackup
  ldx #$32
  jmp farcall2
.export _feof
_feof:
  stx xRegBackup
  ldx #$33
  jmp farcall2
.export _ferror
_ferror:
  stx xRegBackup
  ldx #$34
  jmp farcall2
.export _fflush
_fflush:
  stx xRegBackup
  ldx #$35
  jmp farcall2
.export _fgetc
_fgetc:
  stx xRegBackup
  ldx #$36
  jmp farcall2
.export _fopen
_fopen:
  stx xRegBackup
  ldx #$37
  jmp farcall2
.export _fprintf
_fprintf:
  stx xRegBackup
  ldx #$38
  jmp farcall2
.export _fputc
_fputc:
  stx xRegBackup
  ldx #$39
  jmp farcall2
.export _fputs
_fputs:
  stx xRegBackup
  ldx #$3A
  jmp farcall2
.export _fread
_fread:
  stx xRegBackup
  ldx #$3B
  jmp farcall2
.export _free
_free:
  stx xRegBackup
  ldx #$3C
  jmp farcall2
.export _fwrite
_fwrite:
  stx xRegBackup
  ldx #$3D
  jmp farcall2
.export _getenv
_getenv:
  stx xRegBackup
  ldx #$3E
  jmp farcall2
.export _ltoa
_ltoa:
  stx xRegBackup
  ldx #$3F
  jmp farcall2
.export _malloc
_malloc:
  stx xRegBackup
  ldx #$40
  jmp farcall2
.export _memmove
_memmove:
  stx xRegBackup
  ldx #$41
  jmp farcall2
.export _memset
_memset:
  stx xRegBackup
  ldx #$42
  jmp farcall2
.export _open
_open:
  stx xRegBackup
  ldx #$43
  jmp farcall2
.export _printf
_printf:
  stx xRegBackup
  ldx #$44
  jmp farcall2
.export _putenv
_putenv:
  stx xRegBackup
  ldx #$45
  jmp farcall2
.export _read
_read:
  stx xRegBackup
  ldx #$46
  jmp farcall2
.export _realloc
_realloc:
  stx xRegBackup
  ldx #$47
  jmp farcall2
.export _setlocale
_setlocale:
  stx xRegBackup
  ldx #$48
  jmp farcall2
.export _sprintf
_sprintf:
  stx xRegBackup
  ldx #$49
  jmp farcall2
.export _strcat
_strcat:
  stx xRegBackup
  ldx #$4A
  jmp farcall2
.export _strcmp
_strcmp:
  stx xRegBackup
  ldx #$4B
  jmp farcall2
.export _strcpy
_strcpy:
  stx xRegBackup
  ldx #$4C
  jmp farcall2
.export _stricmp
_stricmp:
  stx xRegBackup
  ldx #$4D
  jmp farcall2
.export _strlen
_strlen:
  stx xRegBackup
  ldx #$4E
  jmp farcall2
.export _strlower
_strlower:
  stx xRegBackup
  ldx #$4F
  jmp farcall2
.export _strncat
_strncat:
  stx xRegBackup
  ldx #$50
  jmp farcall2
.export _strncmp
_strncmp:
  stx xRegBackup
  ldx #$51
  jmp farcall2
.export _strstr
_strstr:
  stx xRegBackup
  ldx #$52
  jmp farcall2
.export _ultoa
_ultoa:
  stx xRegBackup
  ldx #$53
  jmp farcall2
.export _ungetc
_ungetc:
  stx xRegBackup
  ldx #$54
  jmp farcall2
.export _vfprintf
_vfprintf:
  stx xRegBackup
  ldx #$55
  jmp farcall2
.export _vsnprintf
_vsnprintf:
  stx xRegBackup
  ldx #$56
  jmp farcall2
.export _vsprintf
_vsprintf:
  stx xRegBackup
  ldx #$57
  jmp farcall2
.export _write
_write:
  stx xRegBackup
  ldx #$58
  jmp farcall2
.export _normaliseAndGet
_normaliseAndGet:
  stx xRegBackup
  ldx #$59
  jmp farcall
.export _isInHash2
_isInHash2:
  stx xRegBackup
  ldx #$5A
  jmp farcall
.export _isCombiningChar
_isCombiningChar:
  stx xRegBackup
  ldx #$5B
  jmp farcall
.export _hash
_hash:
  stx xRegBackup
  ldx #$5C
  jmp farcall
.export _in_word_set
_in_word_set:
  stx xRegBackup
  ldx #$5D
  jmp farcall
.export _yylex_destroy
_yylex_destroy:
  stx xRegBackup
  ldx #$5E
  jmp farcall
.export _yylex_init
_yylex_init:
  stx xRegBackup
  ldx #$5F
  jmp farcall
.export _yyparse
_yyparse:
  stx xRegBackup
  ldx #$60
  jmp farcall
.export _yyset_in
_yyset_in:
  stx xRegBackup
  ldx #$61
  jmp farcall
.export _strdup
_strdup:
  stx xRegBackup
  ldx #$62
  jmp farcall
.export _getValue
_getValue:
  stx xRegBackup
  ldx #$63
  jmp farcall
.export _parse_orderingSpec
_parse_orderingSpec:
  stx xRegBackup
  ldx #$64
  jmp farcall
.export _parse_scalarExpColumnRef
_parse_scalarExpColumnRef:
  stx xRegBackup
  ldx #$65
  jmp farcall
.export _parse_columnRefUnsuccessful
_parse_columnRefUnsuccessful:
  stx xRegBackup
  ldx #$66
  jmp farcall
.export _hash_createTable
_hash_createTable:
  stx xRegBackup
  ldx #$67
  jmp farcall
.export _parse_tableFactor
_parse_tableFactor:
  stx xRegBackup
  ldx #$68
  jmp farcall
.export _readParams
_readParams:
  stx xRegBackup
  ldx #$69
  jmp farcall
.export _endOfFile
_endOfFile:
  stx xRegBackup
  ldx #$6A
  jmp farcall
.export _strNumberCompare
_strNumberCompare:
  stx xRegBackup
  ldx #$6B
  jmp farcall
.export _getUnicodeChar
_getUnicodeChar:
  stx xRegBackup
  ldx #$6C
  jmp farcall
.export _outputHeader
_outputHeader:
  stx xRegBackup
  ldx #$6D
  jmp farcall
.export _combiningCharCompare
_combiningCharCompare:
  stx xRegBackup
  ldx #$6E
  jmp farcall
.export _getCalculatedColumns
_getCalculatedColumns:
  stx xRegBackup
  ldx #$6F
  jmp farcall
.export _groupResults
_groupResults:
  stx xRegBackup
  ldx #$70
  jmp farcall
.export _getCsvColumn
_getCsvColumn:
  stx xRegBackup
  ldx #$71
  jmp farcall
.export _parse_whereClause
_parse_whereClause:
  stx xRegBackup
  ldx #$72
  jmp farcall
.export _parse_groupingSpec
_parse_groupingSpec:
  stx xRegBackup
  ldx #$73
  jmp farcall
.export _cleanup_inputColumns
_cleanup_inputColumns:
  stx xRegBackup
  ldx #$74
  jmp farcall
.export _cleanup_atomList
_cleanup_atomList:
  stx xRegBackup
  ldx #$75
  jmp farcall
.export _exp_uminus
_exp_uminus:
  stx xRegBackup
  ldx #$76
  jmp farcall
.export _hash_freeTable
_hash_freeTable:
  stx xRegBackup
  ldx #$77
  jmp farcall
.export _parse_scalarExp
_parse_scalarExp:
  stx xRegBackup
  ldx #$78
  jmp farcall
.export _cleanup_columnReferences
_cleanup_columnReferences:
  stx xRegBackup
  ldx #$79
  jmp farcall
.export _parse_inPredicate
_parse_inPredicate:
  stx xRegBackup
  ldx #$7A
  jmp farcall
.export _exp_divide
_exp_divide:
  stx xRegBackup
  ldx #$7B
  jmp farcall
.export _strCompare
_strCompare:
  stx xRegBackup
  ldx #$7C
  jmp farcall
.export _strReplace
_strReplace:
  stx xRegBackup
  ldx #$7D
  jmp farcall
.export _parse_functionRef
_parse_functionRef:
  stx xRegBackup
  ldx #$7E
  jmp farcall
.export _parse_newOutputColumn
_parse_newOutputColumn:
  stx xRegBackup
  ldx #$7F
  jmp farcall
.export _d_strftime
_d_strftime:
  stx xRegBackup
  ldx #$80
  jmp farcall
.export _getColumnValue
_getColumnValue:
  stx xRegBackup
  ldx #$81
  jmp farcall
.export _cleanup_expression
_cleanup_expression:
  stx xRegBackup
  ldx #$82
  jmp farcall
.export _parse_atomCommaList
_parse_atomCommaList:
  stx xRegBackup
  ldx #$83
  jmp farcall
.export _cleanup_query
_cleanup_query:
  stx xRegBackup
  ldx #$84
  jmp farcall
.export _strRTrim
_strRTrim:
  stx xRegBackup
  ldx #$85
  jmp farcall
.export _walkRejectRecord
_walkRejectRecord:
  stx xRegBackup
  ldx #$86
  jmp farcall
.export _getColumnCount
_getColumnCount:
  stx xRegBackup
  ldx #$87
  jmp farcall
.export _parse_expCommaList
_parse_expCommaList:
  stx xRegBackup
  ldx #$88
  jmp farcall
.export _parse_functionRefStar
_parse_functionRefStar:
  stx xRegBackup
  ldx #$89
  jmp farcall
.export _myfseek
_myfseek:
  stx xRegBackup
  ldx #$8A
  jmp farcall
.export _tree_walkAndCleanup
_tree_walkAndCleanup:
  stx xRegBackup
  ldx #$8B
  jmp farcall
.export _runQuery
_runQuery:
  stx xRegBackup
  ldx #$8C
  jmp farcall
.export _hash_compare
_hash_compare:
  stx xRegBackup
  ldx #$8D
  jmp farcall
.export _hash_lookupString
_hash_lookupString:
  stx xRegBackup
  ldx #$8E
  jmp farcall
.export _reallocMsg
_reallocMsg:
  stx xRegBackup
  ldx #$8F
  jmp farcall
.export _consumeCombiningChars
_consumeCombiningChars:
  stx xRegBackup
  ldx #$90
  jmp farcall
.export _getNextRecordOffset
_getNextRecordOffset:
  stx xRegBackup
  ldx #$91
  jmp farcall
.export _getMatchingRecord
_getMatchingRecord:
  stx xRegBackup
  ldx #$92
  jmp farcall
.export _stringGet
_stringGet:
  stx xRegBackup
  ldx #$93
  jmp farcall
.export _cleanup_inputTables
_cleanup_inputTables:
  stx xRegBackup
  ldx #$94
  jmp farcall
.export _outputResult
_outputResult:
  stx xRegBackup
  ldx #$95
  jmp farcall
.export _recordCompare
_recordCompare:
  stx xRegBackup
  ldx #$96
  jmp farcall
.export _parse_scalarExpLiteral
_parse_scalarExpLiteral:
  stx xRegBackup
  ldx #$97
  jmp farcall
.export _strAppendUTF8
_strAppendUTF8:
  stx xRegBackup
  ldx #$98
  jmp farcall
.export _getCurrentDate
_getCurrentDate:
  stx xRegBackup
  ldx #$99
  jmp farcall
.export _needsEscaping
_needsEscaping:
  stx xRegBackup
  ldx #$9A
  jmp farcall
.export _updateRunningCounts
_updateRunningCounts:
  stx xRegBackup
  ldx #$9B
  jmp farcall
.export _yyerror
_yyerror:
  stx xRegBackup
  ldx #$9C
  jmp farcall
.export _readQuery
_readQuery:
  stx xRegBackup
  ldx #$9D
  jmp farcall
.export _tree_insert
_tree_insert:
  stx xRegBackup
  ldx #$9E
  jmp farcall
.export _d_tztime
_d_tztime:
  stx xRegBackup
  ldx #$9F
  jmp farcall
.export _getFirstRecord
_getFirstRecord:
  stx xRegBackup
  ldx #$A0
  jmp farcall
.export _groupResultsInner
_groupResultsInner:
  stx xRegBackup
  ldx #$A1
  jmp farcall
.export _skipBom
_skipBom:
  stx xRegBackup
  ldx #$A2
  jmp farcall
.export _getUnicodeCharFast
_getUnicodeCharFast:
  stx xRegBackup
  ldx #$A3
  jmp farcall
.export _runCommand
_runCommand:
  stx xRegBackup
  ldx #$A4
  jmp farcall
.export _getLookupTableEntry
_getLookupTableEntry:
  stx xRegBackup
  ldx #$A5
  jmp farcall
.export _isNumberWithGetByteLength
_isNumberWithGetByteLength:
  stx xRegBackup
  ldx #$A6
  jmp farcall
.export _cleanup_resultColumns
_cleanup_resultColumns:
  stx xRegBackup
  ldx #$A7
  jmp farcall
.export _strAppend
_strAppend:
  stx xRegBackup
  ldx #$A8
  jmp farcall
.export _d_sprintf
_d_sprintf:
  stx xRegBackup
  ldx #$A9
  jmp farcall
.export _hash_addString
_hash_addString:
  stx xRegBackup
  ldx #$AA
  jmp farcall
.export _strFree
_strFree:
  stx xRegBackup
  ldx #$AB
  jmp farcall
.export _getGroupedColumns
_getGroupedColumns:
  stx xRegBackup
  ldx #$AC
  jmp farcall
.export _yyerror2
_yyerror2:
  stx xRegBackup
  ldx #$AD
  jmp farcall
.export _cleanup_orderByClause
_cleanup_orderByClause:
  stx xRegBackup
  ldx #$AE
  jmp farcall
.export _cleanup_matchValues
_cleanup_matchValues:
  stx xRegBackup
  ldx #$AF
  jmp farcall

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
.byte $A2
.byte $A2
.byte $A2
.byte $A2
.byte $A2
.byte $A2
.byte $A9
.byte $AC
.byte $AA
.byte $AB
.byte $AC
.byte $AC
.byte $B8
.byte $AB
.byte $AC
.byte $AD
.byte $AD
.byte $AD
.byte $AD
.byte $AE
.byte $B0
.byte $B0
.byte $B2
.byte $B2
.byte $B3
.byte $B4
.byte $B5
.byte $B5
.byte $B6
.byte $B7
.byte $B7
.byte $B1
.byte $B9
.byte $B9
.byte $B9
.byte $BA
.byte $BA
.byte $BA
.byte $BA
.byte $BA
.byte $BB
.byte $BB
.byte $B2
.byte $BC
.byte $BC
.byte $BC
.byte $BD
.byte $BD
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
.byte $4F
.byte $6B
.byte $AD
.byte $B9
.byte $C5
.byte $D8
.byte $E4
.byte $F1
.byte $07
.byte $12
.byte $25
.byte $34
.byte $43
.byte $B6
.byte $ED
.byte $C4
.byte $0E
.byte $5D
.byte $D0
.byte $DC
.byte $F5
.byte $7F
.byte $FD
.byte $15
.byte $40
.byte $97
.byte $DB
.byte $AE
.byte $81
.byte $FC
.byte $04
.byte $97
.byte $E6
.byte $3E
.byte $26
.byte $F6
.byte $73
.byte $1C
.byte $9A
.byte $82
.byte $68
.byte $B9
.byte $F1
.byte $18
.byte $37
.byte $7C
.byte $92
.byte $BA
.byte $0F
.byte $4E
.byte $5B
.byte $0A
.byte $70
.byte $99
.byte $73
.byte $85
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
.byte $02
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
