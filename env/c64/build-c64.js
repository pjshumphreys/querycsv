//cl65 -T -t c64 -Ln libc.lbl -O -Os --static-locals -m mapfile -W -C page-template.cfg libc.c initenv.s
//node build-64.js
//((echo -n "ibase=16; " && grep __RAM2_TOTAL__ libc.lbl | sed -n "s/al \([^ ]*\).*/\1/p") | bc)|xargs -I {} dd if=/dev/zero bs=1 count={} of=padded.bin
//dd if=libcdata.bin of=padded.bin conv=notrunc
//make
//dd if=/dev/zero ibs=1 count=8192 | tr "\000" "\377" > paddedFile.bin
//dd if=querycsv1.bin of=paddedFile.bin conv=notrunc
//cat paddedFile.bin querycsv2.bin > querycsv.bin
//bin2efcrt querycsv.bin querycsv.crt

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
  ["_printf",     2, 0x56DF],
  ["_calloc",     2, 0xA4E5],
  ["_clearerr",   2, 0xA5F4],
  ["_close",      2, 0xA17B],
//  ["_exit",       2, 0xA143],
  ["_fclose",     2, 0xA509],
  ["_feof",       2, 0xA600],
  ["_ferror",     2, 0xA60A],
  ["_fflush",     2, 0xBA44],
  ["_fgetc",      2, 0xA548],
  ["_fopen",      2, 0xA699],
  ["_fprintf",    2, 0xA6B1],
  ["_fputc",      2, 0xA6DC],
  ["_fputs",      2, 0xA76A],
  ["_fread",      2, 0xA7D4],
  ["_free",       2, 0xA8A7],
  ["_fwrite",     2, 0xAA7A],
  ["_getenv",     2, 0xAAF7],
  ["_malloc",     2, 0xAC12],
  ["_memcpy",     2, 0xAD20],
  ["_memmove",    2, 0xAD61],
  ["_memset",     2, 0xADBA],
  ["_open",       2, 0xA340],
  ["_putenv",     2, 0xB1BB],
  ["_read",       2, 0xA3EB],
  ["_realloc",    2, 0xB264],
  ["_setlocale",  2, 0xAB12],
  ["_sprintf",    2, 0xB462],
  ["_strcat",     2, 0xB48D],
  ["_strcmp",     2, 0xB4C5],
  ["_strcpy",     2, 0xB4EC],
  ["_stricmp",    2, 0xB50B],
  ["_strlen",     2, 0xB550],
  ["_strlower",   2, 0xB566],
  ["_strncat",    2, 0xB58E],
  ["_strncmp",    2, 0xB5E3],
  ["_strstr",     2, 0xB622],
  ["_ungetc",     2, 0xB67A],
  ["_vfprintf",   2, 0xB6E0],
  ["_vsnprintf",  2, 0xB709],
  ["_vsprintf",   2, 0xB7D1],
  ["_write",      2, 0xA49D]
];

console.log(".export _stdin\n\
.export _stdout\n\
.export _stderr\n\
.export initlib2");

for(i = 0; i < functionsList.length; i++) {
  console.log(".export "+functionsList[i][0]);
}

console.log("\n_stdin = $0870\n\
_stdout = $0872\n\
_stderr = $0874\n\
initlib2 = $A0D9");

for(i = 0; i < functionsList.length; i++) {
  console.log(functionsList[i][0]+":\n  stx xRegBackup\n  ldx #$" + ("00"+(i.toString(16).toUpperCase())).substr(-2) + "\n  jmp farcall2");
}


console.log("\nhighAddressTable:");
for(i = 0; i < functionsList.length; i++) {
  console.log(".byte $"+("0000"+((functionsList[i][2]-1).toString(16).toUpperCase())).substr(-4).substring(0,2));
}

console.log("\nlowAddressTable:");
for(i = 0; i < functionsList.length; i++) {
  console.log(".byte $"+("00"+((functionsList[i][2]-1).toString(16).toUpperCase())).substr(-2));
}

console.log("\nbankTable:");
for(i = 0; i < functionsList.length; i++) {
  console.log(".byte $"+("0000"+(functionsList[i][1].toString(16).toUpperCase())).substr(-2));
}
