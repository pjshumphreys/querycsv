//cl65 -T -t c64 -Ln libc.lbl -O -Os --static-locals -m mapfile -W -C page-template.cfg libc.c initenv.s
//node build-c64.js > tables.inc
//((echo -n "ibase=16; " && grep __RAM2_TOTAL__ libc.lbl | sed -n "s/al \([^ ]*\).*/\1/p") | bc)|xargs -I {} dd if=/dev/zero bs=1 count={} of=padded.bin
//dd if=libcdata.bin of=padded.bin conv=notrunc
//make
//dd if=/dev/zero ibs=1 count=8192 | tr "\000" "\377" > paddedFile.bin
//dd if=querycsv1.bin of=paddedFile.bin conv=notrunc
//cat paddedFile.bin querycsv2.bin > querycsv.bin
//bin2efcrt querycsv.bin querycsv.crt


//find * -name "*.c" -print0 | sed -z "s/\.c$//g" | xargs -0 -I {} sh -c 'rm ../labels.s; egrep -v "\b_'{}'\b" ../labels.inc > ../labels.s; cl65 -o '{}'.bin -T -t c64 -O -Os --static-locals -W -C page-template.cfg ../labels.s '{}'.c;rm '{}'.o;'

//gcc -I /usr/local/lib/cc65/include/ -D __CC65__ -U __unix__ -U __GNUC__ -E sql.c > sql2.c
//sed '/^[#] [0-9]*/d' sql2.c > sql3.c
//indent sql3.c

//printf '\x31\xc0\xc3' | dd of=test_blob bs=1 seek=100 count=3 conv=notrunc

var pages = [
  [
    "skipbom",
    "myfseek"
  ],
  [
    "getCsvColumn",
    "strAppend"
  ],
  [
    "cleanup_query",
    "cleanup_columnReferences",
    "cleanup_resultColumns",
    "cleanup_orderByClause",
    "cleanup_orderByClause",
    "cleanup_expression",
    "cleanup_inputTables"
  ],
  [
    "exp_divide",
    "exp_uminus",
    "getValue"
  ],
  [
    "getCurrentDate",
    "d_strftime",
    "d_tztime"
  ],
  [
    "getUnicodeChar",
    "getUnicodeCharFast"
  ],
  [
    "consumeCombiningChars",
    "isCombiningChar"
  ],
  [
    "outputResult",
    "needsEscaping"
  ],
  [
    "groupResults",
    "groupResultsInner",
    "updateRunningCounts"
  ],
  [
    "hash_addString",
    "hash_lookupString",
    "hash_compare"
  ],
  [
    "getMatchingRecord",
    "endOfFile"
  ]
];

var i  = 0;

var functionsList=[
  /*cc65-floatlib functions */
  ["BASIC_FAC_to_string", 0, 0xBDDD, "FUNC0"],   /* in: FAC value   out: str at $0100 a/y ptr to str */
  ["BASIC_string_to_FAC", 0, 0xB7B5, "FUNC0"],   /* in: $22/$23 ptr to str,a=strlen out: FAC value */

  ["BASIC_s8_to_FAC",     0, 0xBC3C, "FUNC0"],	  /* a: value */
  ["BASIC_u8_to_FAC",     0, 0xB3A2, "FUNC0"],	  /* y: value */

  ["BASIC_u16_to_FAC",    0, 0xBC49, "FUNC0"],   /* a/y:lo/hi value (sta $62 sty $63 sec ldx#$90 jsr...) */
  ["BASIC_s16_to_FAC",    0, 0xB395, "FUNC0"],   /* a/y:lo/hi value */
  ["BASIC_FAC_to_u16",    0, 0xBC9B, "FUNC0"],   /* in:FAC out: y/a:lo/hi value */

/*--------------------------------------------------------------------------------------------- */
/* these functions take one arg (in FAC) and return result (in FAC) aswell */
/*--------------------------------------------------------------------------------------------- */

  ["__fabs",              0, 0xBC58, "FUNC1"],   /* in/out: FAC */
  ["__fatn",              0, 0xE30E, "FUNC1"],   /* in/out: FAC */
  ["__fcos",              0, 0xE264, "FUNC1"],   /* in/out: FAC */
  ["__fexp",              0, 0xBFED, "FUNC1"],   /* in/out: FAC */
/* __ffre:    FUNC1 BASIC_FAC_Fre */
  ["__fint",              0, 0xBCCC, "FUNC1"],   /* in/out: FAC */
  ["__flog",              0, 0xB9EA, "FUNC1"],   /* in/out: FAC */
/* __fpos:    FUNC1 BASIC_FAC_Pos */
  ["__frnd",              0, 0xE097, "FUNC1"],   /* in/out: FAC */
  ["__fsgn",              0, 0xBC39, "FUNC1"],   /* in/out: FAC */
  ["__fsin",              0, 0xE26B, "FUNC1"],   /* in/out: FAC */
  ["__fsqr",              0, 0xBF71, "FUNC1"],   /* in/out: FAC */
  ["__ftan",              0, 0xE2B4, "FUNC1"],   /* in/out: FAC */
  ["__fnot",              0, 0xAED4, "FUNC1"],   /* in/out: FAC */
  ["__fround",            0, 0xBC1B, "FUNC1"],   /* in/out: FAC */

/*--------------------------------------------------------------------------------------------- */
/* these functions take two args (in FAC and ARG) and return result (in FAC) */
/*--------------------------------------------------------------------------------------------- */

  ["__fadd",              0, 0xB86A, "FUNC2"],   /* in: ARG,FAC out:FAC */
  ["__fsub",              0, 0xB853, "FUNC2"],   /* in: ARG,FAC out:FAC */
  ["__fmul",              0, 0xBA2B, "FUNC2"],   /* in: ARG,FAC out:FAC */
  ["__fdiv",              0, 0xBB12, "FUNC2"],   /* in: ARG,FAC out:FAC */
  ["__fpow",              0, 0xBF7B, "FUNC2"],   /* in: ARG,FAC out:FAC */

  ["__fand",              0, 0xAFE9, "FUNC3"],   /* in: ARG,FAC out:FAC */
  ["__for",               0, 0xAFE6, "FUNC3"],   /* in: ARG,FAC out:FAC */

  ["BASIC_FAC_cmp",       0, 0xBC5B, "FUNC0"],   /* in: FAC(x1) a/y ptr lo/hi to x2 out: a=0 (x1=x2) a=1 (x1>x2) a=255 (x1<x2) */
  ["BASIC_FAC_testsgn",   0, 0xBC2B, "FUNC0"],   /* in: FAC(x1) out: a=0 (x1=0) a=1 (x1>0) a=255 (x1<0) */
  ["BASIC_FAC_Poly1",     0, 0xE059, "FUNC0"],   /* in: FAC x  a/y ptr to poly (1byte grade,5bytes per coefficient) */

  /*normally these functions are wrapped by more code, but we need to be able to call them in a raw form as as well */
  ["BASIC_FAC_Atn",       0, 0xE30E, "FUNC0"],   /* in/out: FAC */
  ["BASIC_ARG_FAC_Div",   0, 0xBB12, "FUNC0"],   /* in: ARG,FAC out:FAC */
  ["BASIC_ARG_FAC_Add",   0, 0xB86A, "FUNC0"],    /* in: ARG,FAC out:FAC */

  /* remaining floatlib functions that do actually require easyflash paging to get to */
  ["__ftostr",            2, 0xA154, "farcall"],
  ["__strtof",            2, 0xA170, "farcall"],
  ["__ctof",              2, 0xA18A, "farcall"],
  ["__utof",              2, 0xA196, "farcall"],
  ["__stof",              2, 0xA1A2, "farcall"],
  ["__itof",              2, 0xA1B5, "farcall"],
  ["__ftoi",              2, 0xA1C1, "farcall"],
  ["__fcmp",              2, 0xA1CE, "farcall"],
  ["__ftestsgn",          2, 0xA1E4, "farcall"],
  ["__fneg",              2, 0xA1EF, "farcall"],
  ["__fpoly1",            2, 0xA202, "farcall"],
  ["__fpoly2",            2, 0xA211, "farcall"],
  ["__fatan2",            2, 0xA220, "farcall"],

  /* standard C library functions */
  ["_calloc",             2, 0xA800, "farcall2"],
  ["_clearerr",           2, 0xA90F, "farcall2"],
  ["_close",              2, 0xA496, "farcall2"],
  //["_exit",               2, 0xA45E, "farcall2"],
  ["_fclose",             2, 0xA824, "farcall2"],
  ["_feof",               2, 0xA91B, "farcall2"],
  ["_ferror",             2, 0xA925, "farcall2"],
  ["_fflush",             2, 0xBD92, "farcall2"],
  ["_fgetc",              2, 0xA863, "farcall2"],
  ["_fopen",              2, 0xA9B4, "farcall2"],
  ["_fprintf",            2, 0xA9CC, "farcall2"],
  ["_fputc",              2, 0xA9F7, "farcall2"],
  ["_fputs",              2, 0xAA85, "farcall2"],
  ["_fread",              2, 0xAAEF, "farcall2"],
  ["_free",               2, 0xABC2, "farcall2"],
  ["_fwrite",             2, 0xAD95, "farcall2"],
  ["_getenv",             2, 0xAE12, "farcall2"],
  ["_ltoa",               2, 0xAE9A, "farcall2"],
  ["_malloc",             2, 0xAF2D, "farcall2"],
  //["_memcpy",             2, 0xB03B, "farcall2"],
  ["_memmove",            2, 0xB07C, "farcall2"],
  ["_memset",             2, 0xB0D5, "farcall2"],
  ["_open",               2, 0xA65B, "farcall2"],
  ["_printf",             2, 0xB4D6, "farcall2"],
  ["_putenv",             2, 0xB509, "farcall2"],
  ["_read",               2, 0xA706, "farcall2"],
  ["_realloc",            2, 0xB5B2, "farcall2"],
  ["_setlocale",          2, 0xAE2D, "farcall2"],
  ["_sprintf",            2, 0xB7B0, "farcall2"],
  ["_strcat",             2, 0xB7DB, "farcall2"],
  ["_strcmp",             2, 0xB813, "farcall2"],
  ["_strcpy",             2, 0xB83A, "farcall2"],
  ["_stricmp",            2, 0xB859, "farcall2"],
  ["_strlen",             2, 0xB89E, "farcall2"],
  ["_strlower",           2, 0xB8B4, "farcall2"],
  ["_strncat",            2, 0xB8DC, "farcall2"],
  ["_strncmp",            2, 0xB931, "farcall2"],
  ["_strstr",             2, 0xB970, "farcall2"],
  ["_ultoa",              2, 0xAEF1, "farcall2"],
  ["_ungetc",             2, 0xB9C8, "farcall2"],
  ["_vfprintf",           2, 0xBA2E, "farcall2"],
  ["_vsnprintf",          2, 0xBA57, "farcall2"],
  ["_vsprintf",           2, 0xBB1F, "farcall2"],
  ["_write",              2, 0xA7B8, "farcall2"],

  /* functions that make up querycsv specifically */
  ["_cleanup_atomList",             0, 0x0001, "farcall"],
  ["_cleanup_columnReferences",     0, 0x0001, "farcall"],
  ["_cleanup_expression",           0, 0x0001, "farcall"],
  ["_cleanup_inputColumns",         0, 0x0001, "farcall"],
  ["_cleanup_inputTables",          0, 0x0001, "farcall"],
  ["_cleanup_matchValues",          0, 0x0001, "farcall"],
  ["_cleanup_orderByClause",        0, 0x0001, "farcall"],
  ["_cleanup_query",                0, 0x0001, "farcall"],
  ["_cleanup_resultColumns",        0, 0x0001, "farcall"],
  ["_combiningCharCompare",         0, 0x0001, "farcall"],
  ["_consumeCombiningChars",        0, 0x0001, "farcall"],
  ["_d_sprintf",                    0, 0x0001, "farcall"],
  ["_d_strftime",                   0, 0x0001, "farcall"],
  ["_d_tztime",                     0, 0x0001, "farcall"],
  ["_endOfFile",                    0, 0x0001, "farcall"],
  ["_exp_divide",                   0, 0x0001, "farcall"],
  ["_exp_uminus",                   0, 0x0001, "farcall"],
  ["_getCalculatedColumns",         0, 0x0001, "farcall"],
  ["_getColumnCount",               0, 0x0001, "farcall"],
  ["_getColumnValue",               0, 0x0001, "farcall"],
  ["_getCsvColumn",                 0, 0x0001, "farcall"],
  ["_getCurrentDate",               0, 0x0001, "farcall"],
  ["_getFirstRecord",               0, 0x0001, "farcall"],
  ["_getGroupedColumns",            0, 0x0001, "farcall"],
  ["_getLookupTableEntry",          0, 0x0001, "farcall"],
  ["_getMatchingRecord",            0, 0x0001, "farcall"],
  ["_getNextRecordOffset",          0, 0x0001, "farcall"],
  ["_getUnicodeChar",               0, 0x0001, "farcall"],
  ["_getUnicodeCharFast",           0, 0x0001, "farcall"],
  ["_getValue",                     0, 0x0001, "farcall"],
  ["_groupResults",                 0, 0x0001, "farcall"],
  ["_groupResultsInner",            0, 0x0001, "farcall"],
  ["_hash_addString",               0, 0x0001, "farcall"],
  ["_hash_compare",                 0, 0x0001, "farcall"],
  ["_hash_createTable",             0, 0x0001, "farcall"],
  ["_hash_freeTable",               0, 0x0001, "farcall"],
  ["_hash_lookupString",            0, 0x0001, "farcall"],
  ["_in_word_set",                  0, 0x0001, "farcall"],
  ["_isCombiningChar",              0, 0x0001, "farcall"],
  ["_isInHash2",                    0, 0x0001, "farcall"],
  ["_isNumberWithGetByteLength",    0, 0x0001, "farcall"],
  ["_myfseek",                      0, 0x0001, "farcall"],
  ["_needsEscaping",                0, 0x0001, "farcall"],
  ["_normaliseAndGet",              0, 0x0001, "farcall"],
  ["_outputHeader",                 0, 0x0001, "farcall"],
  ["_outputResult",                 0, 0x0001, "farcall"],
  ["_parse_atomCommaList",          0, 0x0001, "farcall"],
  ["_parse_columnRefUnsuccessful",  0, 0x0001, "farcall"],
  ["_parse_expCommaList",           0, 0x0001, "farcall"],
  ["_parse_functionRef",            0, 0x0001, "farcall"],
  ["_parse_functionRefStar",        0, 0x0001, "farcall"],
  ["_parse_groupingSpec",           0, 0x0001, "farcall"],
  ["_parse_inPredicate",            0, 0x0001, "farcall"],
  ["_parse_newOutputColumn",        0, 0x0001, "farcall"],
  ["_parse_orderingSpec",           0, 0x0001, "farcall"],
  ["_parse_scalarExp",              0, 0x0001, "farcall"],
  ["_parse_scalarExpColumnRef",     0, 0x0001, "farcall"],
  ["_parse_scalarExpLiteral",       0, 0x0001, "farcall"],
  ["_parse_tableFactor",            0, 0x0001, "farcall"],
  ["_parse_whereClause",            0, 0x0001, "farcall"],
  ["_readParams",                   0, 0x0001, "farcall"],
  ["_readQuery",                    0, 0x0001, "farcall"],
  ["_reallocMsg",                   0, 0x0001, "farcall"],
  ["_recordCompare",                0, 0x0001, "farcall"],
  ["_runCommand",                   0, 0x0001, "farcall"],
  ["_runQuery",                     0, 0x0001, "farcall"],
  ["_skipBom",                      0, 0x0001, "farcall"],
  ["_strAppend",                    0, 0x0001, "farcall"],
  ["_strAppendUTF8",                0, 0x0001, "farcall"],
  ["_strCompare",                   0, 0x0001, "farcall"],
  ["_strdup",                       0, 0x0001, "farcall"],
  ["_stringGet",                    0, 0x0001, "farcall"],
  ["_strNumberCompare",             0, 0x0001, "farcall"],
  ["_strReplace",                   0, 0x0001, "farcall"],
  ["_strRTrim",                     0, 0x0001, "farcall"],
  ["_strtod",                       0, 0x0001, "farcall"],
  ["_tree_insert",                  0, 0x0001, "farcall"],
  ["_tree_walkAndCleanup",          0, 0x0001, "farcall"],
  ["_updateRunningCounts",          0, 0x0001, "farcall"],
  ["_walkRejectRecord",             0, 0x0001, "farcall"],
  ["_yyerror2",                     0, 0x0001, "farcall"],
  ["_yyerror",                      0, 0x0001, "farcall"],
  ["_yylex",                        0, 0x0001, "farcall"],
  ["_yylex_destroy",                0, 0x0001, "farcall"],
  ["_yylex_init",                   0, 0x0001, "farcall"],
  ["_yyparse",                      0, 0x0001, "farcall"],
  ["_yyset_in",                     0, 0x0001, "farcall"]
];

console.log(".export _stdin\n\
.export _stdout\n\
.export _stderr");

for(i = 0; i < functionsList.length; i++) {
  console.log(".export "+functionsList[i][0]);
}

console.log("\n_stderr = $0913\n\
_stdin = $090F\n\
_stdout = $0911\n\
initlib2 = $A3F4\n\
___float_fac_to_float = $A0AC\n\
___float_float_to_fac = $A01B\n\
___float_float_to_fac_arg = $A061\n\
aRegBackup = $0820\n\
xRegBackup = $081F");

for(i = 0; i < functionsList.length; i++) {
  console.log(functionsList[i][0]+":\n  stx xRegBackup\n  ldx #$" + ("00"+(i.toString(16).toUpperCase())).substr(-2) + "\n  jmp "+functionsList[i][3]);
}

console.log("\nhighAddressTable:");
for(i = 0; i < functionsList.length; i++) {
  console.log(".byte $"+("000"+((functionsList[i][2]-1).toString(16).toUpperCase())).substr(-4).substring(0,2));
}

console.log("\nlowAddressTable:");
for(i = 0; i < functionsList.length; i++) {
  console.log(".byte $"+("0"+((functionsList[i][2]-1).toString(16).toUpperCase())).substr(-2));
}

console.log("\nbankTable:");
for(i = 0; i < functionsList.length; i++) {
  console.log(".byte $"+("0"+(functionsList[i][1].toString(16).toUpperCase())).substr(-2));
}
