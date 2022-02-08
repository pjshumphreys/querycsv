const childProcess = require('child_process');
const execSync = childProcess.execSync;
const exec = childProcess.exec;
const fs = require('graceful-fs');
const readline = require('readline');
const walk = require('walk');
const shellEscape = require('shell-escape');
let i;
const files = [];
const hasProp = (obj, prop) => Object.hasOwnProperty.call(obj, prop);

let hash2ChunkCount;

let remainder = 16384;

const matchOperatorsRe = /[|\\{}()[\]^$+*?.]/g;

const hashMap = {};
let passPostfix = '';

/* functionsList = A mapping of function names to the memory page that
contains them, the address within that page and which trampoline function
in the $C000 range to call to set them up correctly */
const functionsList = [
  ['BASIC_FAC_to_string', 1, 0xBDDD, 'FUNC0'], /* in: FAC value   out: str at $0100 a/y ptr to str */
  ['BASIC_string_to_FAC', 1, 0xB7B5, 'FUNC0'], /* in: $22/$23 ptr to str,a=strlen out: FAC value */

  ['BASIC_s8_to_FAC', 1, 0xBC3C, 'FUNC0'], /* a: value */
  ['BASIC_u8_to_FAC', 1, 0xB3A2, 'FUNC0'], /* y: value */

  ['BASIC_u16_to_FAC', 1, 0xBC49, 'FUNC0'], /* a/y:lo/hi value (sta $62 sty $63 sec ldx#$90 jsr...) */
  ['BASIC_s16_to_FAC', 1, 0xB395, 'FUNC0'], /* a/y:lo/hi value */
  ['BASIC_FAC_to_u16', 1, 0xBC9B, 'FUNC0'], /* in:FAC out: y/a:lo/hi value */

  /* ------------------------------------------------------------------------- */
  /* these functions take one arg (in FAC) and return result (in FAC) as well */
  /* ------------------------------------------------------------------------- */

  ['__fabs', 1, 0xBC58, 'FUNC1'], /* in/out: FAC */
  ['__fatn', 1, 0xE30E, 'FUNC1'], /* in/out: FAC */
  ['__fcos', 1, 0xE264, 'FUNC1'], /* in/out: FAC */
  ['__fexp', 1, 0xBFED, 'FUNC1'], /* in/out: FAC */
  /* __ffre:    FUNC1 BASIC_FAC_Fre */
  ['__fint', 1, 0xBCCC, 'FUNC1'], /* in/out: FAC */
  ['__flog', 1, 0xB9EA, 'FUNC1'], /* in/out: FAC */
  /* __fpos:    FUNC1 BASIC_FAC_Pos */
  ['__frnd', 1, 0xE097, 'FUNC1'], /* in/out: FAC */
  ['__fsgn', 1, 0xBC39, 'FUNC1'], /* in/out: FAC */
  ['__fsin', 1, 0xE26B, 'FUNC1'], /* in/out: FAC */
  ['__fsqr', 1, 0xBF71, 'FUNC1'], /* in/out: FAC */
  ['__ftan', 1, 0xE2B4, 'FUNC1'], /* in/out: FAC */
  ['__fnot', 1, 0xAED4, 'FUNC1'], /* in/out: FAC */
  ['__fround', 1, 0xBC1B, 'FUNC1'], /* in/out: FAC */

  /* -------------------------------------------------------------------------- */
  /* these functions take two args (in FAC and ARG) and return result (in FAC) */
  /* -------------------------------------------------------------------------- */

  ['__fadd', 1, 0xB86A, 'FUNC2'], /* in: ARG,FAC out:FAC */
  ['__fsub', 1, 0xB853, 'FUNC2'], /* in: ARG,FAC out:FAC */
  ['__fmul', 1, 0xBA2B, 'FUNC2'], /* in: ARG,FAC out:FAC */
  ['__fdiv', 1, 0xBB12, 'FUNC2'], /* in: ARG,FAC out:FAC */
  ['__fpow', 1, 0xBF7B, 'FUNC2'], /* in: ARG,FAC out:FAC */

  ['__fand', 1, 0xAFE9, 'FUNC3'], /* in: ARG,FAC out:FAC */
  ['__for', 1, 0xAFE6, 'FUNC3'], /* in: ARG,FAC out:FAC */

  ['BASIC_FAC_cmp', 1, 0xBC5B, 'FUNC0'], /* in: FAC(x1) a/y ptr lo/hi to x2 out: a=0 (x1=x2) a=1 (x1>x2) a=255 (x1<x2) */
  ['BASIC_FAC_testsgn', 1, 0xBC2B, 'FUNC0'], /* in: FAC(x1) out: a=0 (x1=0) a=1 (x1>0) a=255 (x1<0) */
  ['BASIC_FAC_Poly1', 1, 0xE059, 'FUNC0'], /* in: FAC x  a/y ptr to poly (1byte grade,5bytes per coefficient) */

  /* normally these functions are wrapped by more code, but we need to be able
  to call them in a raw form */
  ['BASIC_FAC_Atn', 1, 0xE30E, 'FUNC0'], /* in/out: FAC */
  ['BASIC_ARG_FAC_Div', 1, 0xBB12, 'FUNC0'], /* in: ARG,FAC out:FAC */
  ['BASIC_ARG_FAC_Add', 1, 0xB86A, 'FUNC0'] /* in: ARG,FAC out:FAC */
];

/* these are unused */
/* BASIC_FAC_Poly2   = $e043     ; in: FAC x  a/y ptr to poly (1byte grade,5bytes per coefficient) */

/* BASIC_LoadARG   = $babc     ; a/y:lo/hi ptr to 5-byte float */
/* BASIC_LoadFAC   = $bba2     ; a/y:lo/hi ptr to 5-byte float */

const floatlibFunctionsList = [
  ['__ftostr', 1, 0x0001, 'farcall'],
  ['__strtof', 1, 0x0001, 'farcall'],
  ['__ctof', 1, 0x0001, 'farcall'],
  ['__utof', 1, 0x0001, 'farcall'],
  ['__stof', 1, 0x0001, 'farcall'],
  ['__itof', 1, 0x0001, 'farcall'],
  ['__ftoi', 1, 0x0001, 'farcall'],
  ['__fcmp', 1, 0x0001, 'farcall'],
  ['__ftestsgn', 1, 0x0001, 'farcall'],
  ['__fneg', 1, 0x0001, 'farcall'],
  ['__fpoly1', 1, 0x0001, 'farcall'],
  ['__fpoly2', 1, 0x0001, 'farcall'],
  ['__fatan2', 1, 0x0001, 'farcall']
];

const clibFunctionsList = [
  /* standard C library functions */
  ['_strtod', 1, 0x0001, 'farcall'],
  ['_atol', 2, 0x0001, 'farcall2'],
  ['_ltoa', 2, 0x0001, 'farcall2'],
  ['_ultoa', 2, 0x0001, 'farcall2'],
  ['_isdigit', 2, 0x0001, 'farcall2'],
  ['_clearerr', 2, 0x0001, 'farcall2'],
  ['_fclose', 2, 0x0001, 'farcall2'],
  ['_feof', 2, 0x0001, 'farcall2'],
  ['_ferror', 2, 0x0001, 'farcall2'],
  ['_fflush', 2, 0x0001, 'farcall2'],
  ['_fgetc', 2, 0x0001, 'farcall2'],
  ['_fgets', 2, 0x0001, 'farcall2'],
  ['_fopen', 2, 0x0001, 'farcall2'],
  ['_fputc', 2, 0x0001, 'farcall2'],
  ['_fread', 2, 0x0001, 'farcall2'],
  ['_fwrite', 2, 0x0001, 'farcall2'],
  ['_chdir', 2, 0x0001, 'farcall2'],
  ['_free', 2, 0x0001, 'farcall2'],
  ['_malloc', 2, 0x0001, 'farcall2'],
  ['_memcpy', 2, 0x0001, 'farcall2'],
  ['_memmove', 2, 0x0001, 'farcall2'],
  ['_memset', 2, 0x0001, 'farcall2'],
  ['_realloc', 2, 0x0001, 'farcall2'],
  ['_sprintf', 2, 0x0001, 'farcall2'],
  ['_strcat', 2, 0x0001, 'farcall2'],
  ['_strchr', 2, 0x0001, 'farcall2'],
  ['_strcmp', 2, 0x0001, 'farcall2'],
  ['_strcpy', 2, 0x0001, 'farcall2'],
  ['_strlen', 2, 0x0001, 'farcall2'],
  ['_strncat', 2, 0x0001, 'farcall2'],
  ['_strstr', 2, 0x0001, 'farcall2'],
  ['_ungetc', 2, 0x0001, 'farcall2'],
  ['_vsnprintf', 2, 0x0001, 'farcall2'],
  ['_vsprintf', 2, 0x0001, 'farcall2'],

  // other special functions we'll need
  ['_isCombiningChar', 0, 0x0001, 'farcall2'],
  ['_in_word_set_a', 0, 0x0001, 'farcall2'],
  ['_in_word_set_b', 0, 0x0001, 'farcall2'],
  ['_in_word_set_c', 0, 0x0001, 'farcall2']
];

const defines = {
  _stderr: 0,
  _stdin: 0,
  _stdout: 0,
  zerobss2: 0,
  initlib2: 0,
  donelib2: 0,
  ___float_fac_to_float: 0,
  ___float_float_to_fac: 0,
  ___float_float_to_fac_arg: 0,
  aRegBackup: 0,
  spRegBackup: 0,
  xRegBackup: 0,
  updateSpinner: 0
};

const rodataLabels = [];

if (fs.existsSync('querycsv.c')) {
  /* The first action to initiate */
  start();
}

function start () {
  console.log('start');

  /* create the necessary folders */
  execSync(
    'mkdir -p build;' +
      'mkdir -p build/bin;' +
      'mkdir -p build/s;' +
      'mkdir -p build/g;' +
      'mkdir -p build/ro;' +
      'mkdir -p build/obj;' +
      'mkdir -p build/obj2'
  );

  createFloatLibInclude();
}

/* create the cc65-floatlib include */
function createFloatLibInclude () {
  console.log('createFloatLibInclude');

  const tables = fs.createWriteStream('build/floatlib.inc');

  for (i = 0; i < functionsList.length; i++) {
    hashMap[functionsList[i][0]] = i;

    tables.write(
      functionsList[i][0] + ':\n' +
        '  stx xRegBackup\n' +
        '  ldx #$' + ('00' + (i.toString(16).toUpperCase())).substr(-2) + '\n' +
        '  jmp ' + functionsList[i][3] + '\n'
    );
  }

  tables.write('\nhighAddressTable:\n');
  for (i = 0; i < functionsList.length; i++) {
    tables.write(
      '.byte $' +
        ('0000' + (functionsList[i][2] - 1).toString(16).toUpperCase())
          .substr(-4)
          .substring(0, 2) +
        '\n'
    );
  }

  tables.write('\nlowAddressTable:\n');
  for (i = 0; i < functionsList.length; i++) {
    tables.write(
      '.byte $' +
        ('00' + (functionsList[i][2] - 1).toString(16).toUpperCase())
          .substr(-2) +
        '\n'
    );
  }

  for (i = 0; i < floatlibFunctionsList.length; i++) {
    tables.write('.export ' + floatlibFunctionsList[i][0] + '\n');
  }

  tables.end(compileFloatLib);
}

/* compile cc65-floatlib. */
function compileFloatLib () {
  console.log('compileFloatLib');

  /* compile the floatlib portion written in C (strtod) */
  execSync('cc65 -T -t c64 -I ./floatlib floatlib.c -o build/floatlib.s');

  /* link into a binary */
  execSync('cl65 -T -t c64 ' +
      '-I ./floatlib ' +
      '-Ln build/floatlib.lbl ' +
      '--config ' + (passPostfix === '' ? '' : 'build/') + 'floatlib' + passPostfix + '.cfg ' +
      'floatlib/float.s build/floatlib.s'
  );

  /* create a zeroed binary the same size as the total ram used */
  execSync(
    '(' +
        'echo -n "ibase=16;scale=16;" && ' +
        '((grep __RAM2_LAST__ build/floatlib.lbl|' +
        "sed -n \"s/al \\([^ ]*\\).*/\\1/p\")|tr -d '\\n')" +
        ' && echo -n "-" && ' +
        'grep __RAM2_START__ build/floatlib.lbl|' +
        'sed -n "s/al \\([^ ]*\\).*/\\1/p"' +
      ')|bc|' +
      'xargs -I {} dd if=/dev/zero bs=1 count={} of=build/floatlibdata.bin'
  );

  /* Create a combined binary of the data and bss
  segments so that they can be copied as one blob */
  execSync(
    'dd if=build/floatlibdata2.bin of=build/floatlibdata.bin conv=notrunc;' +
      'rm build/floatlibdata2.bin'
  );

  if (passPostfix === '') {
    /* add the float lib functions to the hashmap */
    floatlibFunctionsList.forEach(function (v) {
      hashMap[v[0]] = this.length;
      this.push(v);
    }, functionsList);

    clibFunctionsList.forEach(function (v) {
      hashMap[v[0]] = this.length;
      this.push(v);
    }, functionsList);

    hash2ChunkCount = 15;

    for (i = 0; i < hash2ChunkCount; i++) {
      /*
      put an entry in the functions list for each file.
      as the code in each of them doesn't need to call any other page
      we can probably use farret2
      */
      const name = '_isInHash2_' + i;
      hashMap[name] = functionsList.length;
      functionsList.push([
        name,
        0,
        0,
        'farcall2'
      ]);
    }
  }

  /* read the label file and use its contents to
  update each function address in the hashmap */
  const lineReader = readline.createInterface({
    input: fs.createReadStream('build/floatlib.lbl')
  });

  lineReader.on('line', updateFunctionAddress.bind(1));

  /* when finished, start the next step (compiling libc) */
  lineReader.on('close', compileLibC);
}

/* compile cc65 standard library */
function compileLibC () {
  console.log('compileLibC');

  /* compile a fake program that uses c library */
  execSync(
    'cl65 -T -t c64 -O -Os ' +
      '-Ln build/libc.lbl ' +
      '--static-locals ' +
      '--config ' + (passPostfix === '' ? '' : 'build/') + 'libc' + passPostfix + '.cfg ' +
      'libc.c'
  );

  /* create an zeroed binary the same size as the total ram used */
  execSync(
    '(' +
        'echo -n "ibase=16;scale=16;" && ' +
        '((grep __RAM2_LAST__ build/libc.lbl|' +
        "sed -n \"s/al \\([^ ]*\\).*/\\1/p\")|tr -d '\n')" +
        ' && echo -n "-" && ' +
        'grep __RAM2_START__ build/libc.lbl|' +
        'sed -n "s/al \\([^ ]*\\).*/\\1/p"' +
      ')|bc|' +
      'xargs -I {} dd if=/dev/zero bs=1 count={} of=build/libcdata.bin'
  );

  /* Create a combined binary of the ram segments
  so that they can be copied as one blob */
  execSync(
    'dd if=build/libcdata2.bin of=build/libcdata.bin conv=notrunc;' +
      'rm build/libcdata2.bin'
  );

  /* read the label file and use its contents to
  update each function address in the hashmap */
  const lineReader = readline.createInterface({
    input: fs.createReadStream('build/libc.lbl')
  });

  lineReader.on('line', updateFunctionAddress.bind(2));

  /* when finished, start the next step (compiling the functions that
  make up my program) */
  lineReader.on('close', passPostfix === '' ? compileLexer : compileData);
}

// compile and split up lexer
function compileLexer () {
  console.log('compileLexer');

  execSync(
    'sed "' +
        's/struct yy_trans_info/flex_int8_t yy_accept2(unsigned int offset);' +
        'flex_uint16_t yy_nxt2(unsigned int offset);' +
        'flex_int16_t yy_chk2(unsigned int offset);' +
        'YY_CHAR yy_ec2(unsigned int offset);' +
        'struct yy_trans_info/g;' +
        's/%s/%S/g;' +
        's/yyconst char msg\\[\\]/yyconst char *msg/g;' +
        's/flex_int32_t yy_rule_can_match_eol/flex_int8_t yy_rule_can_match_eol/g;' +
        's/flex_int16_t yy_accept/flex_int8_t yy_accept/g;' +
        's/yy_ec\\[YY_SC_TO_UI(\\*yy_cp)\\]/yy_ec2(YY_SC_TO_UI(*yy_cp))/g;' +
        's/yy_nxt\\[yy_base\\[yy_current_state\\] + yy_c\\]/yy_nxt2\\(yy_base\\[yy_current_state\\] + yy_c\\)/g;' +
        's/yy_chk\\[yy_base\\[yy_current_state\\] + yy_c\\]/yy_chk2\\(yy_base\\[yy_current_state\\] + yy_c\\)/g;' +
        's/yy_accept\\[yy_current_state\\]/yy_accept2\\(yy_current_state\\)/g;' +
      '" lexer.c > build/lexer2.h'
  );

  /* compile functions into assembly language. use our own
  patched cc65 executable that does "jmp farret" instead of "rts" */
  execSync('cc65 -e farret -T -t c64 lexer2.c -O -Os --static-locals --writable-strings -o build/lexer2.s');

  splitUpFunctions('lexer2', compileParser);
}

function compileParser () {
  console.log('compileParser');

  execSync(
    'sed -e"' +
      's/YY_INITIAL_VALUE (static YYSTYPE yyval_default;)//g;' +
      's/yycheck\\[\\(.[^]]*\\)\\]/yycheck2(\\1)/g;' +
      's/yydefact\\[\\(.[^]]*\\)\\]/yydefact2(\\1)/g;' +
      's/yyr1\\[\\(.[^]]*\\)\\]/yyr1a(\\1)/g;' +
      's/#define YY_LAC_ESTABLISH/yytype_int16 yycheck2(int offset);\\n#define YY_LAC_OESTABLISH/g;' +
      's/%s/%S/g;' +
      's/%d/%D/g;' +
      's/%lu/%LU/g;' +
      's/\'s\'/\'S\'/g;' +
      's/YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);/static YYSTYPE yylval;/g;' +
      's/static const yytype_int16 yypact\\[\\]/yytype_int16 yypact2(int offset);yytype_int8 yydefact2(int offset);yytype_int8 yyr1a(int offset);static const yytype_int16 yypact[]/g;' +
      '" sql.c > build/sql2.h');

  execSync(
    'sed -i -r "' +
      's/yypact\\[([^]]+)\\]/yypact2(\\1)/gi;' +
      '" build/sql2.h');

  execSync('cc65 -e farret -T -t c64 -O -Os sql2.c --static-locals --writable-strings -o build/sql2.s');

  // split parser up into 1 function per .s file
  // (including all rodata. add all data vars to a single .s file)
  splitUpFunctions('sql2', compileC64, true);
}

function compileC64 () {
  console.log('compileC64');

  execSync('cc65 -e farret -T -t c64 c64.c --writable-strings -o build/c64.s');

  splitUpFunctions('c64', compileQueryCSV, true);
}

function compileQueryCSV () {
  console.log('compileQueryCSV');

  execSync('cc65 -e farret -T -t c64 querycsv.c --writable-strings -o build/querycsv.s');

  splitUpFunctions('querycsv', compileData, true);
}

/* compile the data segment to a memory page.
(this will be copied to ram on startup) */
function compileData () {
  console.log('compileData');

  /* compile data and bss segments. generate an assembly
  language include of all the labels */

  execSync(
    'cl65 ' +
      '-Ln build/data.lbl ' +
      '-C ' + (passPostfix === '' ? '' : 'build/') + 'data' + passPostfix + '.cfg ' +
      'build/data.s -vm'
  );

  execSync(
    '(' +
        'echo -n "ibase=16;scale=16;" && ' +
        '((grep __RAM2_LAST__ build/data.lbl|' +
        "sed -n \"s/al \\([^ ]*\\).*/\\1/p\")|tr -d '\n')" +
        ' && echo -n "-" && ' +
        'grep __RAM2_START__ build/data.lbl|' +
        'sed -n "s/al \\([^ ]*\\).*/\\1/p"' +
      ')|bc|' +
      'xargs -I {} dd if=/dev/zero bs=1 count={} of=build/data.bin'
  );

  /* TODO: confirm this: Create a combined binary of the ram segments
  so that they can be copied as one blob? */
  execSync('dd if=build/data2.bin of=build/data.bin conv=notrunc;rm build/data2.bin');

  const lineReader2 = readline.createInterface({
    input: fs.createReadStream('build/data.lbl')
  });

  const labels = fs.createWriteStream('build/labels.s');

  /* read the resultant memory locations and make an assembly include
  containing just their addresses */
  lineReader2.on('line', line => {
    const name = line.replace(/^al\s+[0-9A-F]+ \./, '');

    if (name.match(/l[0-9a-f]{4}/)) {
      const address = parseInt(line.match(/[0-9A-F]+/), 16);

      labels.write(
        name + ' = $' +
          ('0000' + address.toString(16).toUpperCase()).substr(-4) +
          '\n'
      );
    } else if (name.match(/\b_[0-9a-zA-Z]/) || name === 'jmpvec') { // && name !== '_main') {
    // starts with an underscore (name defined by the c source code)
      const address = parseInt(line.match(/[0-9A-F]+/), 16);

      labels.write('.export ' + name + '\n');

      labels.write(
        name + ' = $' +
          ('0000' + address.toString(16).toUpperCase()).substr(-4) +
          '\n'
      );
    }
  });

  lineReader2.on('close', () => {
    labels.end(compileHash2);
  });
}

function compileHash2 () {
  let i, name;

  console.log('compileHash2');

  for (i = 0; fs.existsSync('./hash2in' + i + '.c'); i++) {
    execSync(
      'cl65 -T -t c64 ' +
        '-o build/obj2/hash2in' + i + '.bin ' +
        '-Ln build/obj2/hash2in' + i + '.lbl ' +
        '-C rodata-page.cfg ' +
        'hash2in' + i + '.c build/labels.s;' +
        'rm *.o'
    );

    /*
      put an entry in the functions list for each file.
      as the code in each of them doesn't need to call any other page
      we can probably use farret2
    */
    name = '_isInHash2_' + i;
    functionsList[hashMap[name]][2] = parseInt(execSync(
      'sh -c "(echo -n \\"ibase=16;scale=16;\\" && (grep _isInHash2_' +
      i +
      ' build/obj2/hash2in' +
      i +
      '.lbl|sed -n \\"s/al \\([^ ]*\\).*/\\1/p\\"))|bc"'
    ).toString(), 10);
  }

  if (passPostfix === '') {
    /* allow the hash2 function to be packed also */
    execSync('cc65 -e farret -T -t c64 hash2out.c -o build/hash2out.s');
    splitUpFunctions('hash2out', compileHash3And4, true);
  } else {
    compileHash3And4();
  }
}

function compileHash3And4 () {
  console.log('compileHash3And4');

  execSync(
    'cl65 -T -t c64 ' +
      '-o build/obj2/hash3.bin ' +
      '-Ln build/obj2/hash3.lbl ' +
      '-C rodata-page.cfg ' +
      'hash3.c build/labels.s;' +
      'rm *.o'
  );

  functionsList[hashMap._isCombiningChar][2] = parseInt(execSync(
    'sh -c "(echo -n \\"ibase=16;scale=16;\\" && (grep _isCombiningChar' +
      ' build/obj2/hash3.lbl|sed -n \\"s/al \\([^ ]*\\).*/\\1/p\\"))|bc"'
  ).toString(), 10);

  execSync(
    'sed "' +
      '1s/^/#include \\"cc65iso.h\\"\\nstruct hash4Entry { const char *name; int script; int index; int isNotLower; }; extern struct hash4Entry hash4export;\\n/;' +
      's/static struct hash4Entry/' +
        'static const struct hash4Entry/gi;' +
      's/static unsigned short/' +
        'static const unsigned short/gi;' +
      's/if (\\*str == \\*s && !strncmp (str + 1, s + 1, len - 1) && s\\[len\\]/' +
        'while(len \\&\\& *str \\&\\& (*str == *s)) {\\n++str;\\n++s;\\n--len;\\n}\\nif(len == 0 \\&\\& *s/gi;' +
      's/return \\&wordlist\\[key\\];/' +
        '{\\n' +
          'hash4export.script = wordlist[key].script;\\n' +
          'hash4export.index = wordlist[key].index;\\n' +
          'hash4export.isNotLower = wordlist[key].isNotLower;\\n' +
          'return (struct hash4EntryA *)\\&hash4export;\\n' +
        '}/gi;' +
      '" hash4a.h > hash4a.c');

  execSync(
    'cl65 -T -t c64 ' +
      '-o build/obj2/hash4a.bin ' +
      '-Ln build/obj2/hash4a.lbl ' +
      '-C rodata-page.cfg ' +
      'hash4a.c build/labels.s;' +
      'rm *.o');

  functionsList[hashMap._in_word_set_a][2] = parseInt(execSync(
    'sh -c "(echo -n \\"ibase=16;scale=16;\\" && (grep _in_word_set_a' +
      ' build/obj2/hash4a.lbl|sed -n \\"s/al \\([^ ]*\\).*/\\1/p\\"))|bc"'
  ).toString(), 10);

  execSync(
    'sed -e"' +
      '1s/^/#include \\"cc65iso.h\\"\\nstruct hash4Entry { const char *name; int script; int index; int isNotLower; }; extern struct hash4Entry hash4export;\\n/;' +
      's/static struct hash4Entry/' +
        'static const struct hash4Entry/gi;' +
      's/static unsigned short/' +
        'static const unsigned short/gi;' +
      's/if (\\*str == \\*s && !strncmp (str + 1, s + 1, len - 1) && s\\[len\\]/' +
        'while(len \\&\\& *str \\&\\& (*str == *s)) {\\n++str;\\n++s;\\n--len;\\n}\\nif(len == 0 \\&\\& *s/gi;' +
      's/return \\&wordlist\\[key\\];/' +
        '{\\n' +
          'hash4export.script = wordlist[key].script;\\n' +
          'hash4export.index = wordlist[key].index;\\n' +
          'hash4export.isNotLower = wordlist[key].isNotLower;\\n' +
          'return (struct hash4EntryB *)\\&hash4export;\\n' +
        '}/gi;' +
      '" hash4b.h > hash4b.c');

  execSync(
    'cl65 -T -t c64 ' +
      '-o build/obj2/hash4b.bin ' +
      '-Ln build/obj2/hash4b.lbl ' +
      '-C rodata-page.cfg ' +
      'hash4b.c build/labels.s;' +
      'rm *.o'
  );

  functionsList[hashMap._in_word_set_b][2] = parseInt(execSync(
    'sh -c "(echo -n \\"ibase=16;scale=16;\\" && (grep _in_word_set_b' +
      ' build/obj2/hash4b.lbl|sed -n \\"s/al \\([^ ]*\\).*/\\1/p\\"))|bc"'
  ).toString(), 10);

  execSync(
    'sed -e"' +
      '1s/^/#include \\"cc65iso.h\\"\\nstruct hash4Entry { const char *name; int script; int index; int isNotLower; }; extern struct hash4Entry hash4export;\\n/;' +
      's/static struct hash4Entry/' +
        'static const struct hash4Entry/gi;' +
      's/static unsigned short/' +
        'static const unsigned short/gi;' +
      's/if (\\*str == \\*s && !strncmp (str + 1, s + 1, len - 1) && s\\[len\\]/' +
        'while(len \\&\\& *str \\&\\& (*str == *s)) {\\n++str;\\n++s;\\n--len;\\n}\\nif(len == 0 \\&\\& *s/gi;' +
      's/return \\&wordlist\\[key\\];/' +
        '{\\n' +
          'hash4export.script = wordlist[key].script;\\n' +
          'hash4export.index = wordlist[key].index;\\n' +
          'hash4export.isNotLower = wordlist[key].isNotLower;\\n' +
          'return (struct hash4EntryC *)\\&hash4export;\\n' +
        '}/gi;' +
      '" hash4c.h > hash4c.c');

  execSync(
    'cl65 -T -t c64 ' +
      '-o build/obj2/hash4c.bin ' +
      '-Ln build/obj2/hash4c.lbl ' +
      '-C rodata-page.cfg ' +
      'hash4c.c build/labels.s;' +
      'rm *.o'
  );

  functionsList[hashMap._in_word_set_c][2] = parseInt(execSync(
    'sh -c "(echo -n \\"ibase=16;scale=16;\\" && (grep _in_word_set_c' +
      ' build/obj2/hash4c.lbl|sed -n \\"s/al \\([^ ]*\\).*/\\1/p\\"))|bc"'
  ).toString(), 10);

  createTrampolinesInclude();
}

function createTrampolinesInclude () {
  console.log('createTrampolinesInclude');

  const tables = fs.createWriteStream('build/tables.inc');

  for (i in defines) {
    tables.write(
      '.export ' + i + '\n' +
        i + ' = $' +
        ('0000' + ((defines[i]).toString(16).toUpperCase())).substr(-4) +
        '\n'
    );
  }

  for (i = 0; i < functionsList.length; i++) {
    const secondTable = i > 255;

    if (secondTable && functionsList[i][3] === 'farcall') {
      functionsList[i][3] = 'farcall3';
    }

    tables.write(
      '.export ' + functionsList[i][0] + '\n' +

        functionsList[i][0] + ':\n' +
        '  stx xRegBackup\n' +
        '  ldx #$' + ('00' + (i.toString(16).toUpperCase())).substr(-2) + '\n' +
        '  jmp ' + functionsList[i][3] + '\n'
    );
  }

  tables.write('\nhighAddressTable:\n');
  for (i = 0; i < Math.min(256, functionsList.length); i++) {
    tables.write(
      '.byte $' +
        ('0000' + ((functionsList[i][2] - 1).toString(16).toUpperCase()))
          .substr(-4)
          .substring(0, 2) +
        ' ;' + functionsList[i][0] +
        '\n'
    );
  }

  tables.write('\nlowAddressTable:\n');
  for (i = 0; i < Math.min(256, functionsList.length); i++) {
    tables.write(
      '.byte $' +
        ('00' + ((functionsList[i][2] - 1).toString(16).toUpperCase()))
          .substr(-2) +
        ' ;' + functionsList[i][0] +
        '\n'
    );
  }

  tables.write('\nbankTable:\n');
  for (i = 0; i < Math.min(256, functionsList.length); i++) {
    tables.write(
      '.byte $' +
        ('00' + (functionsList[i][1].toString(16).toUpperCase()))
          .substr(-2) +
        ' ;' + functionsList[i][0] +
        '\n'
    );
  }

  tables.write('\nhighAddressTable2:\n');
  for (i = 256; i < functionsList.length; i++) {
    tables.write(
      '.byte $' +
        ('0000' + ((functionsList[i][2] - 1).toString(16).toUpperCase()))
          .substr(-4)
          .substring(0, 2) +
        ' ;' + functionsList[i][0] +
        '\n'
    );
  }

  tables.write('\nlowAddressTable2:\n');
  for (i = 256; i < functionsList.length; i++) {
    tables.write(
      '.byte $' +
        ('00' + ((functionsList[i][2] - 1).toString(16).toUpperCase()))
          .substr(-2) +
        ' ;' + functionsList[i][0] +
        '\n'
    );
  }

  tables.write('\nbankTable2:\n');
  for (i = 256; i < functionsList.length; i++) {
    tables.write(
      '.byte $' +
        ('00' + (functionsList[i][1].toString(16).toUpperCase()))
          .substr(-2) +
        ' ;' + functionsList[i][0] +
        '\n'
    );
  }

  tables.end(compileMain);
}

function compileMain () {
  console.log('compileMain');

  execSync('cl65 -T -t c64 -Ln build/main.lbl --config ' + (passPostfix === '' ? '' : 'build/') + 'main' + (passPostfix === '' ? '' : 'a') + '.cfg crt0.s');

  execSync(
    '(' +
        'echo -n "ibase=16;scale=16;" && ' +
        '((grep __RAM2_LAST__ build/main.lbl|' +
        "sed -n \"s/al \\([^ ]*\\).*/\\1/p\")|tr -d '\\n')" +
        ' && echo -n "-" && ' +
        'grep __RAM2_START__ build/main.lbl|' +
        'sed -n "s/al \\([^ ]*\\).*/\\1/p"' +
      ')|bc|' +
      'xargs -I {} dd if=/dev/zero bs=1 count={} of=build/maindata.bin'
  );

  /* read the label file and update each function address */

  const lineReader = readline.createInterface({
    input: fs.createReadStream('build/main.lbl')
  });

  execSync('cat build/labels.s > build/labels2.s');

  const labels = fs.createWriteStream('build/labels2.s', { flags: 'a' });

  for (i in defines) {
    labels.write(
      i + ' = $' +
        ('0000' + ((defines[i]).toString(16).toUpperCase())).substr(-4) +
        '\n'
    );
  }

  labels.write('.export _main\n');
  labels.write('.import pushl0\n');

  lineReader.on('line', line => {
    const name = line.replace(/^al\s+[0-9A-F]+ \./, '');
    const address = parseInt(line.match(/[0-9A-F]+/), 16);

    if (name === 'farret') {
      labels.write(
        'farret = $' +
          ('0000' + (address.toString(16).toUpperCase())).substr(-4) +
          '\n'
      );
    }

    if (name === '_exit') {
      labels.write(
        '_exit = $' +
          ('0000' + (address.toString(16).toUpperCase())).substr(-4) +
          '\n'
      );
    }

    if (hasProp(hashMap, name)) {
      labels.write(
        name + ' = $' +
          ('0000' + (address.toString(16).toUpperCase())).substr(-4) +
          '\n'
      );
    }
  });

  lineReader.on('close', () => {
    if (passPostfix === '') {
      labels.end(calculateSizes);
    } else if (passPostfix === 'a') {
      labels.end(compileYYParse);
    } else {
      labels.end(glueTogetherBinary);
    }
  });
}

/* compile yyparse twice, first to get its size, then to locate it in the proper place */
function calculateSizes () {
  console.log('calculateSizes');

  updateName(['_yyparse.s', fs.readFileSync('build/s/_yyparse.s', { encoding: 'utf8' })]);

  execSync(
    'cat ./updateSpinner.s ./build/s/_yyparse.s > ./build/g/_yyparse.s;' +
      'cl65 -T -t c64 ' +
      '-o ./build/obj2/yyparse.bin ' +
      '-Ln ./build/obj2/yyparse.lbl ' +
      '-C ./yyparse.cfg ./build/g/_yyparse.s'
  );

  passPostfix = 'a';

  const yyparseSize = Math.max(0x2000, fs.statSync('build/obj2/yyparse.bin').size);

  const yyparseStart = 0xC000 - yyparseSize;

  const dataSize = fs.statSync('build/data.bin').size;

  const dataStart = yyparseStart - dataSize;

  const libcSize = fs.statSync('build/libcdata.bin').size;

  const libcStart = dataStart - libcSize;

  const floatlibSize = fs.statSync('build/floatlibdata.bin').size;

  const floatlibStart = libcStart - floatlibSize;

  const mainSize = fs.statSync('build/maindata.bin').size;

  const mainStart = floatlibStart - mainSize;

  const heapSize = mainStart - 0x0800;

  remainder -= (yyparseSize + dataSize + libcSize + floatlibSize + mainSize);

  execSync(
    "sed 's/ROMH:   file = %O, start = $8000, size = $4000/" +
      'ROMH:   file = %O, start = $' +
      (yyparseStart.toString(16).toUpperCase()) +
      ', size = $' +
      (yyparseSize.toString(16).toUpperCase()) +
      '/g;' +
      's/RAM:    file = "", start = $0800, size = $9800/' +
      'RAM:    file = "", start = $0800, size = $' +
      (heapSize.toString(16).toUpperCase()) + '/g;' +
      "' yyparse.cfg > build/yyparsea.cfg"
  );

  execSync(
    "sed 's/data2.bin\\\", start = $0800, size = $9800/" +
      'data2.bin\\", start = $' +
      (dataStart.toString(16).toUpperCase()) +
      ', size = $' +
      (dataSize.toString(16).toUpperCase()) +
      '/g;' +
      's/RAM:    file = "", start = $0800, size = $9800/' +
      'RAM:    file = "", start = $0800, size = $' +
      (heapSize.toString(16).toUpperCase()) + '/g;' +
      "' data.cfg > build/dataa.cfg"
  );

  execSync(
    "sed 's/floatlibdata2.bin\\\", start = $0800, size = $9800/" +
      'floatlibdata2.bin\\", start = $' +
      (floatlibStart.toString(16).toUpperCase()) +
      ', size = $' +
      (floatlibSize.toString(16).toUpperCase()) +
      '/g;' +
      's/RAM:    file = "", start = $0800, size = $9800/' +
      'RAM:    file = "", start = $0800, size = $' +
      (heapSize.toString(16).toUpperCase()) + '/g;' +
      "' floatlib.cfg > build/floatliba.cfg"
  );

  execSync(
    "sed 's/libcdata2.bin\\\", start = $0800, size = $9800/" +
      'libcdata2.bin\\", start = $' +
      (libcStart.toString(16).toUpperCase()) +
      ', size = $' +
      (libcSize.toString(16).toUpperCase()) +
      '/g;' +
      's/RAM:      file = "", start = $0800, size = $9800/' +
      'RAM:      file = "", start = $0800, size = $' +
      (heapSize.toString(16).toUpperCase()) + '/g;' +
      "' libc.cfg > build/libca.cfg"
  );

  execSync(
    "sed 's/RAM2:     file = \\\"\\\", start = $0800, size = $9800/" +
      'RAM2:     file = \\"\\", start = $' +
      (mainStart.toString(16).toUpperCase()) +
      ', size = $' +
      (mainSize.toString(16).toUpperCase()) +
      '/g;' +
      's/RAM:      file = "", start = $0800, size = $9800/' +
      'RAM:      file = "", start = $0800, size = $' +
      (heapSize.toString(16).toUpperCase()) + '/g;' +
      "' main.cfg > build/maina.cfg"
  );

  execSync(
    "sed '" +
      's/RAM:     file = "", start = $0800, size = $9800/' +
      'RAM:     file = "", start = $0800, size = $' +
      (heapSize.toString(16).toUpperCase()) + '/g;' +
      "' page.cfg > build/pagea.cfg"
  );

  compileFloatLib();
}

function compileYYParse () {
  console.log('compileYYParse');

  /* yyparse goes directly into an oversized page of its own */
  execSync(
    'cl65 -T -t c64 ' +
      '-o ./build/obj2/yyparse.bin ' +
      '-Ln ./build/obj2/yyparse.lbl ' +
      '-C ./build/yyparsea.cfg ./build/g/_yyparse.s'
  );

  execSync('mv build/s/_yyparse.s ./build/_yyparse.s');

  /* read the label file and use its contents to
  update each function address in the hashmap */
  const lineReader = readline.createInterface({
    input: fs.createReadStream('./build/obj2/yyparse.lbl')
  });

  lineReader.on('line', updateFunctionAddress.bind(3));

  /* when finished, start the next step (compiling the functions that
  make up my program) */
  lineReader.on('close', addROData);
}

/* pack most function code into a set of 8k memory pages */
function addROData () {
  console.log('addROData');

  const list = [];
  const walker = walk.walk('./build/s', {});

  walker.on('file', (root, fileStats, next) => {
    list.push([fileStats.name, fs.readFileSync('build/s/' + fileStats.name, { encoding: 'utf8' })]);

    next();
  });

  walker.on('errors', (root, nodeStatsArray, next) => {
    next();
  });

  walker.on('end', () => {
    list.forEach(updateName);

    packPages();
  });
}

function packPages () {
  console.log('packPages');

  /* patch compareCodepoints into the functions that need it (so the table is
  always in the same page) */
  execSync("sed -i 's/jmp     farret/rts/g;s/__compareCodepoints/_compareCodepoints/g;' build/s/_compareCodepoints.s");

  execSync("cat build/s/_compareCodepoints.s >> build/s/_getBytesCP1252.s;sed -i 's/_compareCodepoints/_compareCP1252/g;s/querycsv/querycsv1/g;' build/s/_getBytesCP1252.s");
  execSync("cat build/s/_compareCodepoints.s >> build/s/_getBytesCommon.s;sed -i 's/_compareCodepoints/_compareCommon/g;s/querycsv/querycsv2/g;' build/s/_getBytesCommon.s");
  execSync("cat build/s/_compareCodepoints.s >> build/s/_getBytesPetscii.s;sed -i 's/_compareCodepoints/_comparePetscii/g;s/querycsv/querycsv3/g;' build/s/_getBytesPetscii.s");
  execSync("cat build/s/_compareCodepoints.s >> build/s/_getBytesAtariST.s;sed -i 's/_compareCodepoints/_compareAtariST/g;s/querycsv/querycsv4/g;' build/s/_getBytesAtariST.s");
  execSync("cat build/s/_compareCodepoints.s >> build/s/_getBytesZXCommon.s;sed -i 's/_compareCodepoints/_compareZX/g;s/querycsv/querycsv5/g;' build/s/_getBytesZXCommon.s");
  execSync("cat build/s/_compareCodepoints.s >> build/s/_getBytesCP1047.s;sed -i 's/_compareCodepoints/_compareCP1047/g;s/querycsv/querycsv6/g;' build/s/_getBytesCP1047.s");
  execSync('rm build/s/_compareCodepoints.s');

  execSync(
    'pushd build/s;' +
      'find * -name "*.s" ! -name build/_yyparse.s -print0|' +
      'sed -z "s/\\.s$//g"|' +
      "xargs -0 -I {} sh -c '" +
        "echo \".include \\\"../../header.inc\\\"\" > ../g/'{}'.s;" +
        "echo \".include \\\"../labels2.s\\\"\" >> ../g/'{}'.s;" +
        "cat '{}'.s >> ../g/'{}'.s;" +
        '' +
        'cl65 -T -t c64 ' +
        "-o ../obj/'{}'.bin " +
        '-C ../../function.cfg ' +
        "-Ln ../g/'{}'.lbl ../g/'{}'.s;" +
        "rm ../g/'{}'.o;" +
      "';" +
    'popd'
  );

  /* use a bin packing algorithm to group the functions and
  produce a binary of each 8k page */
  const list = exec(
    "sh -c '" +
        'pushd build/obj > /dev/null;' +
          'find * -type f ! -name build/_yyparse.bin -print0|' +
          'xargs -0 stat --printf="%s %n\\n"|' +
          'sort -rh;' +
        'popd > /dev/null' +
      "'"
  );

  const lineReader = readline.createInterface({
    input: list.stdout
  });

  const maxSize = 8192;// 8277;

  //files = [];
  const totalSizes = [];

  lineReader.on('line', line => {
    const size = parseInt(line.match(/^[0-9]+/)[0], 10);
    const name = line.replace(/^[0-9]+ /, '');

    if (size > maxSize) {
      throw new Error('file too big');
    }

    for (i = 0; ;i++) {
      if (i === files.length) {
        /* the function won't fit in any of the existing pages. Add a new page for it instead */
        files.push([]);
        totalSizes.push(0);
      }

      if (totalSizes[i] + size < maxSize) {
        files[i].push(name);
        totalSizes[i] += size;
        break;
      }
    }
  });

  lineReader.on('close', compilePages);
}

function compilePages () {
  console.log('compilePages');

  let i = 0;

  /* compile each page and then update the addresses and
  page numbers of each function in the table */
  for (i = 0; i < files.length; i++) {
    const names = files[i]
      .map(x => JSON.stringify(x.replace(/\.bin$/g, '.s')))
      .join(' ');

    execSync(
      'pushd build/s;' +
        'echo ".include \\"../header.inc\\"" > ../page' + (i + 1) + '.s;' +
        'echo ".include \\"labels2.s\\"" >> ../page' + (i + 1) + '.s;' +
        'cat ' + names + ' >> ../page' + (i + 1) + '.s;' +
          '' +
          '' +
          'cl65 -T -t c64 ' +
            '-o ../obj2/page' + (i + 1) + '.bin ' +
            '-Ln ../obj2/page' + (i + 1) + '.lbl ' +
            '-C ../pagea.cfg ../page' + (i + 1) + '.s;' +
          '' +
        'popd'
    );
  }

  updatePageFunctionAddresses(4);
}

function updatePageFunctionAddresses (pageNumber) {
  console.log('updatePageFunctionAddresses: ', pageNumber - 3);

  const input = fs.createReadStream('./build/obj2/page' + (pageNumber - 3) + '.lbl');

  input.once('readable', () => {
    const lineReader = readline.createInterface({ input });

    lineReader.on('line', updateFunctionAddress.bind(pageNumber));

    /* when finished, start the next step (compiling libc) */
    lineReader.on('close', () => {
      updatePageFunctionAddresses(pageNumber + 1);
    });
  });

  input.once('error', err => {
    if (err.code === 'ENOENT') {
      passPostfix = 'b';

      functionsList[hashMap._isCombiningChar][1] = pageNumber++;
      functionsList[hashMap._in_word_set_a][1] = pageNumber++;
      functionsList[hashMap._in_word_set_b][1] = pageNumber++;
      functionsList[hashMap._in_word_set_c][1] = pageNumber++;

      for (i = 0; i < hash2ChunkCount; i++) {
        const name = '_isInHash2_' + i;
        functionsList[hashMap[name]][1] = pageNumber + i;
      }

      createTrampolinesInclude();
    } else {
      console.log('Caught', err);
    }
  });
}

function glueTogetherBinary () {
  console.log('glueTogetherBinary');

  execSync('dd if=/dev/zero bs=1 count=' + remainder + ' | tr "\\000" "\\377" > build/padding.bin');
  execSync('dd if=/dev/zero bs=1 count=8192 | tr "\\000" "\\377" > build/8k.bin');

  execSync('cat build/8k.bin build/querycsv.bin > build/full.bin');
  execSync('cat build/8k.bin build/floatlib.bin >> build/full.bin');
  execSync('cat build/8k.bin build/libc.bin >> build/full.bin');
  execSync('cat build/padding.bin build/maindata.bin build/floatlibdata.bin ' +
  'build/libcdata.bin build/data.bin build/obj2/yyparse.bin >> build/full.bin');

  for (i = 0; i < files.length; i++) {
    execSync('cat build/8k.bin ./build/obj2/page' + (i + 1) + '.bin >> build/full.bin');
  }

  execSync('cat build/8k.bin ./build/obj2/hash3.bin >> build/full.bin');
  execSync('cat build/8k.bin ./build/obj2/hash4a.bin >> build/full.bin');
  execSync('cat build/8k.bin ./build/obj2/hash4b.bin >> build/full.bin');
  execSync('cat build/8k.bin ./build/obj2/hash4c.bin >> build/full.bin');

  for (i = 0; i < hash2ChunkCount; i++) {
    execSync('cat build/8k.bin ./build/obj2/hash2in' + i + '.bin >> build/full.bin');
  }

  i = 64 - files.length - hash2ChunkCount - 8;

  while (i--) {
    execSync('cat build/8k.bin build/8k.bin >> build/full.bin');
  }

  execSync('cartconv -t easy -i build/full.bin -o querycsv.crt -n "querycsv"');

  // all done (hooray!)
}

/* *** HELPER FUNCTIONS AFTER THIS POINT *** */

/*
Split the specified assembly file into one file for each function's code
(including the necessary rodata) and append to a global data segment.
Pause if node.js can't keep up
*/
function splitUpFunctions (filename, callback, append) {
  console.log('splitUpFunctions');

  let j;
  let notAddedJmpVec = true;

  const lineReader = readline.createInterface({
    input: fs.createReadStream('build/' + filename + '.s')
  });

  const data = fs.createWriteStream('build/data.s', {
    flags: append ? 'a' : 'w'
  });

  const rodataOutputStreams = [];

  const code = fs.createWriteStream('build/' + filename + '_code.s');

  const functionOutputStreams = [];
  let activeStream = code;
  let rodataType = 0;

  lineReader.on('line', line => {
    let name;

    if (/^\.segment\s+"[_0-9A-Z]+"/.test(line)) {
      name = line.replace(/\.segment\s+"/, '').match(/[_0-9A-Z]+/)[0];

      if (name === 'CODE') {
        rodataType = 0;

        if (functionOutputStreams.length) {
          activeStream = functionOutputStreams[functionOutputStreams.length - 1];
        } else {
          activeStream = code;
        }
      } else if (name === 'RODATA') {
        rodataType = 1;
        activeStream = data;

        // don't output this line
      } else {
        rodataType = 2;

        activeStream = data;

        if (name !== 'BSS') {
          writePause(activeStream, line + '\n');
        }

        /* explicitly add jmpvec, so it works with rom paging */
        if (name === 'DATA' && !append && notAddedJmpVec) {
          notAddedJmpVec = false;
          writePause(activeStream, '.export jmpvec\njmpvec: jmp $ffff\n');
        }
      }
    } else if (/\.export/.test(line) || /\.endproc/.test(line)) {
      // do nothing
    } else if (/^\.proc\s+/.test(line)) {
      name = (line.replace(/^\.proc\s+/, '')).match(/[^:]+/)[0];
      functionOutputStreams.push(fs.createWriteStream('build/s/' + name + '.s'));

      activeStream = functionOutputStreams[functionOutputStreams.length - 1];

      writePause(
        activeStream,
        '.segment "CODE"\n' +
          '.export _' + name + '\n' +
          line.replace(name, '_' + name) + '\n'
      );

      /* add an entry for each into the mapping table */
      if (name !== '_compareCodepoints') { /* compareCodepoints is a bsearch callback that
        needs to be in the same page as the function that called bsearch. it doesn't need
        to be in the jump table */
        hashMap[name] = functionsList.length;
        functionsList.push([name, 0, 0x0001, 'farcall']);
      }
    } else if (rodataType && /^[a-z0-9A-Z]+:/.test(line)) {
      if (activeStream) {
        name = line.match(/^[a-z0-9A-Z]+/)[0];

        if (rodataType === 1 || rodataType === 3) {
          rodataType = 3;

          rodataOutputStreams.push(fs.createWriteStream('build/ro/' + name + '.s'));

          /* add to the list of rodata regexes used to add the appropriate rodata to each function */
          rodataLabels.push([name, false, new RegExp('(\\b' + name.replace(matchOperatorsRe, '\\$&') + '\\b)', 'm')]);

          activeStream = rodataOutputStreams[rodataOutputStreams.length - 1];
        } else {
          writePause(activeStream, '.export ' + name.toLowerCase() + filename + '\n');
        }

        writePause(activeStream, line.replace(name, name.toLowerCase() + filename) + '\n');
      }
    } else if (rodataType && /^[_a-z0-9A-Z]+\s*:/.test(line)) {
      if (activeStream) {
        name = line.match(/^[_a-z0-9A-Z]+/)[0];

        if (rodataType === 1 || rodataType === 3) {
          rodataType = 3;

          rodataOutputStreams.push(fs.createWriteStream('build/ro/' + name + '.s'));
          rodataLabels.push([
            name,
            false,
            new RegExp('(\\b' + name.replace(matchOperatorsRe, '\\$&') + '\\b)', 'm')
          ]);

          activeStream = rodataOutputStreams[rodataOutputStreams.length - 1];
          writePause(activeStream, '.ifndef ' + name + '\n');
        } else {
          writePause(activeStream, '.export ' + name + '\n');
        }
        writePause(activeStream, line + '\n');
      }
    } else if (rodataType && /^[a-z0-9A-Z]+\s+:=/.test(line)) {
      if (activeStream) {
        name = line.match(/^[a-z0-9A-Z]+/)[0];

        if (rodataType === 1 || rodataType === 3) {
          rodataType = 3;

          rodataOutputStreams.push(fs.createWriteStream('build/ro/' + name + '.s'));
          rodataLabels.push([name, false, new RegExp('(\\b' + name.replace(matchOperatorsRe, '\\$&') + '\\b)', 'm')]);
          activeStream = rodataOutputStreams[rodataOutputStreams.length - 1];
        } else {
          writePause(activeStream, '.export ' + name.toLowerCase() + filename + '\n');
        }

        line = line.replace(name, name.toLowerCase() + filename);
        var name2 = line.match(/L[0-9A-F]+/);
        if (name2) {
          writePause(
            activeStream,
            line.replace(name2[0], name2[0].toLowerCase() + filename) +
              '\n'
          );
        } else {
          writePause(activeStream, line + '\n');
        }
      }
    } else {
      writePause(
        activeStream,
        line
          .replace(/L[0-9A-F]{4}/g, match => match.toLowerCase() + filename) +
            '\n'
      );
    }
  });

  lineReader.on('close', () => {
    j = functionOutputStreams.length + rodataOutputStreams.length + 1;

    data.end(writeFunctionPostfixes);
  });

  function writeFunctionPostfixes () {
    code.end(allStreamsClosed);

    for (i = 0; i < rodataOutputStreams.length; i++) {
      /* close current stream */
      writePause(rodataOutputStreams[i], '.endif\n');
      rodataOutputStreams[i].end(allStreamsClosed);
    }

    for (i = 0; i < functionOutputStreams.length; i++) {
      writePause(functionOutputStreams[i], '.endproc\n');

      /* close current stream */
      functionOutputStreams[i].end(allStreamsClosed);
    }
  }

  /* when all streams have been closed, go to the next step (compiling the
  data segment) */
  function allStreamsClosed () {
    if (--j === 0) {
      callback();
    }
  }

  function writePause (stream, text) {
    if (!stream.write(text)) {
      lineReader.pause();

      stream.on('drain', () => {
        lineReader.resume();
      });
    }
  }
}

/*
  update the addresses for functions in the functionslist array with
  their resultant values after compilation
*/
function updateFunctionAddress (line) {
  const name = line.replace(/^al\s+[0-9A-F]+ \./, '');

  const address = parseInt(line.match(/[0-9A-F]+/), 16);
  const pageNumber = this + 0;

  const name2 = name.replace(/^_/, '');

  // the libc page is page 2
  if (pageNumber === 2 && name === 'initlib') {
    defines.initlib2 = address;
  } else if (pageNumber === 2 && name === 'zerobss') {
    defines.zerobss2 = address;
  } else if (pageNumber === 2 && name === 'donelib') {
    defines.donelib2 = address;
  } else if (pageNumber === 3 && name === '__updateSpinner') {
    defines.updateSpinner = address;
  } else if (hasProp(defines, name)) {
    defines[name] = address;
  }

  if (
    name !== '_main' && // don't use the wrong main
      pageNumber < 3 &&
      hasProp(hashMap, name)
  ) {
    functionsList[hashMap[name]][2] = address;
    functionsList[hashMap[name]][1] = pageNumber;
  } else if (
    hasProp(hashMap, name2) &&
      functionsList[hashMap[name2]][2] === 1 /* address 1 means it we don't yet
      have the real value */
  ) {
    functionsList[hashMap[name2]][2] = address;
    functionsList[hashMap[name2]][1] = pageNumber;
  }
}

/* updateName adds to rodata values needed by each function to the end of
its assembly source file */
function updateName (elem) {
  const text = elem[1];

  let hasMatches = false;

  rodataLabels.forEach(element => {
    element[1] = element[2].test(text);

    if (element[1]) {
      hasMatches = true;
    }
  });

  execSync(
    'echo ".segment \\"INIT\\"" >> build/s/' + elem[0] + ' &&' +
    'echo ".segment \\"STARTUP\\"" >> build/s/' + elem[0] + ' &&' +
    'echo ".segment \\"ONCE\\"" >> build/s/' + elem[0] + (!hasMatches ? '' : (' &&' +
    'echo ".segment \\"RODATA\\"" >> build/s/' + elem[0] + ' &&' +
    'cat ' +
    shellEscape(rodataLabels.filter(label => label[1]).map(label => 'build/ro/' + label[0] + '.s')) +
    '>> build/s/' + elem[0]))
  );
}

// add all functions found to the jump table. generate the jump table as a binary file, including the trampoline code to compute the offsets the code will use

// compute the output size of each c file (code+rodata) and the total size of static vars. use a modified cc65 that adds an underscore to the start of each called function and does "jmp farret" instead of rts

// each c file will complain about missing functions. create an assembler include that satisfies these with addresses within the jump table and compile the page again

// group the c files together to attempt to fill 8k ram pages. keep a running total of the amount of memory bytes used for static variables and update the config file accordingly. update the addesses in the jump table as we go along

// create a new jump table with the correct offsets

// create a binary file that contains the inital values for all static variables and an assembler function that copies the data to the right location

// glue all the binaries together

// figure out the total ram used and alter the config file so stack and heap start in a memory area above static variables

// update the start address for ram

/* compile the main page again, using our updated table */

/* glue all the memory bank dumps together */
