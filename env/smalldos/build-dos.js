/*
wasm -0 pager.asm
wcl -ms -0 -os -fpc -s -fm=qrycsv16 -fe=qrycsv16 pager.obj libc.c

wcl -mt -0 -os -s -fm dosload.c
hexdump -v -e '8/1 "0x%02X, "' -e '"\n"' ./dosload.com | sed -e 's/^/db /g;s/, 0x  //g;s/, /,/g;s/,$//g' >dosload.inc
*/

const childProcess = require('child_process');
const execSync = childProcess.execSync;
const fs = require('graceful-fs');
const path = require('path');
const readline = require('readline');
const walk = require('walk');
// const shellEscape = require('shell-escape');
const matchAll = require('match-all');

const hasProp = (obj, prop) =>
  Object.prototype.hasOwnProperty.bind(obj)(prop);

const matchOperatorsRe = /[|\\{}()[\]^$+*?.]/g;

const hashMap = {};

let rodataSize = 0;
let codeOffset = 0;

// name, pageNo, trampolineAddr, pageAddr, callMethod

const functionsList = [
  ['realmain_', 1, 0x0001, 0x0001, 'farcall'],
  ['setupDos__', 0, 0x0001, 0x0001, 'farcall'],
  ['exit__', 0, 0x0001, 0x0001, 'farcall'],
  ['strcmp__', 0, 0x0001, 0x0001, 'farcall'],
  ['stricmp__', 0, 0x0001, 0x0001, 'farcall'],
  ['strlen__', 0, 0x0001, 0x0001, 'farcall'],
  ['strstr__', 0, 0x0001, 0x0001, 'farcall'],
  ['strcat__', 0, 0x0001, 0x0001, 'farcall'],
  ['strncat__', 0, 0x0001, 0x0001, 'farcall'],
  ['strnicmp__', 0, 0x0001, 0x0001, 'farcall'],
  ['strcpy__', 0, 0x0001, 0x0001, 'farcall'],
  ['strncpy__', 0, 0x0001, 0x0001, 'farcall'],
  ['memcpy__', 0, 0x0001, 0x0001, 'farcall'],
  ['memmove__', 0, 0x0001, 0x0001, 'farcall'],
  ['memset__', 0, 0x0001, 0x0001, 'farcall'],
  ['vfprintf__', 0, 0x0001, 0x0001, 'farcall'],
  ['vsprintf__', 0, 0x0001, 0x0001, 'farcall'],
  ['fopen__', 0, 0x0001, 0x0001, 'farcall'],
  ['fflush__', 0, 0x0001, 0x0001, 'farcall'],
  ['fclose__', 0, 0x0001, 0x0001, 'farcall'],
  ['fread__', 0, 0x0001, 0x0001, 'farcall'],
  ['fwrite_dos__', 0, 0x0001, 0x0001, 'farcall'],
  ['macYield__', 0, 0x0001, 0x0001, 'farcall'],
  //  ['logNum', 1, 0x0001, 0x0001, 'farcall'],
  ['fprintf_dos__', 0, 0x0001, 0x0001, 'farcall'],
  ['fputs_dos__', 0, 0x0001, 0x0001, 'farcall'],
  ['getenv__', 0, 0x0001, 0x0001, 'farcall'],
  ['putenv__', 0, 0x0001, 0x0001, 'farcall'],
  ['malloc__', 0, 0x0001, 0x0001, 'farcall'],
  ['free__', 0, 0x0001, 0x0001, 'farcall'],
  ['realloc__', 0, 0x0001, 0x0001, 'farcall'],
  ['atexit__', 0, 0x0001, 0x0001, 'farcall'],
  ['atexit_dos__', 0, 0x0001, 0x0001, 'farcall'],
  ['fseek__', 0, 0x0001, 0x0001, 'farcall'],
  ['fgetc__', 0, 0x0001, 0x0001, 'farcall'],
  ['ungetc__', 0, 0x0001, 0x0001, 'farcall'],
  ['sprintf__', 0, 0x0001, 0x0001, 'farcall'],
  ['abs__', 0, 0x0001, 0x0001, 'farcall'],
  ['strrchr__', 0, 0x0001, 0x0001, 'farcall'],
  ['atol__', 0, 0x0001, 0x0001, 'farcall'],
  ['strtod__', 0, 0x0001, 0x0001, 'farcall'],
  ['bsearch__', 0, 0x0001, 0x0001, 'farcall'],
  ['qsort__', 0, 0x0001, 0x0001, 'farcall'],

  ['int86__', 0, 0x0001, 0x0001, 'farcall'],
  ['tzset__', 0, 0x0001, 0x0001, 'farcall'],
  ['time__', 0, 0x0001, 0x0001, 'farcall'],
  ['gmtime__', 0, 0x0001, 0x0001, 'farcall'],
  ['localtime__', 0, 0x0001, 0x0001, 'farcall'],
  ['getcwd__', 0, 0x0001, 0x0001, 'farcall'],
  ['_getdrive__', 0, 0x0001, 0x0001, 'farcall'],
  ['chdir__', 0, 0x0001, 0x0001, 'farcall'],
  ['_chdrive__', 0, 0x0001, 0x0001, 'farcall'],
  ['__I4D_', 0, 0x0001, 0x0001, 'farcall'],
  ['__EDA_', 0, 0x0001, 0x0001, 'farcall'],
  ['__FDA_', 0, 0x0001, 0x0001, 'farcall'],
  ['__FDC_', 0, 0x0001, 0x0001, 'farcall'],
  ['__FDD_', 0, 0x0001, 0x0001, 'farcall'],
  ['__FDM_', 0, 0x0001, 0x0001, 'farcall'],
  ['__FDN_', 0, 0x0001, 0x0001, 'farcall'],
  ['__EDC_', 0, 0x0001, 0x0001, 'farcall'],
  ['__FDS_', 0, 0x0001, 0x0001, 'farcall'],
  ['__I4FD_', 0, 0x0001, 0x0001, 'farcall'],
  ['__get_errno_ptr__', 0, 0x0001, 0x0001, 'farcall']
];

const env = process.env;

const os = require('os');

env.WATCOM = path.resolve(os.homedir(), '.wine/drive_c/WATCOM');
env.PATH = path.resolve(env.WATCOM, 'binl') + ':' + env.PATH;
env.INCLUDE = path.resolve(env.WATCOM, 'h');

/* don't include these functions in the output files as they are never invoked (dead code elimination) */
const ignoreFunctions = [
  'yyunput_',
  'logNum_',
  'input_',
  'yypush_buffer_state_',
  'yyget_extra_',
  'yyget_lineno_',
  'yyget_column_',
  'yyget_in_',
  'yyget_out_',
  'yyget_leng_',
  'yyget_text_',
  'yyset_lineno_',
  'yyset_column_',
  'yyset_out_',
  'yyget_debug_',
  'yyset_debug_',
  'yyget_lval_',
  'yyset_lval_',
  'yylex_init_'
].reduce((acc, item) => {
  acc[item] = !0;
  return acc;
}, {});

const startOffset = 0x1c5;// 0x0200;

let currentAddr = startOffset;

/* contains a map of the addresses of all global variables */
const defines = {};

const rodataLabels = [];

let pageSize;

//

const cflags = '-dMICROSOFT=1 -dDOS_DAT=1 -mc -fpc -0 -zdf -ob -oh -ou -ot -or -od -ox';

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
    'mkdir -p build/h;' +
    'mkdir -p build/ro;' +
    'mkdir -p build/obj;' +
    'mkdir -p build/obj2;' +
    'mkdir -p build/fcb;' +
    'mkdir -p output;' +
    'rm -f build/rodata2.asm;' +
    'rm -f build/data.inc;' +
    'touch build/data.inc'
  );

  // update the jump table locations, starting at call_rom3 -4 and working downward in memory
  functionsList.forEach((item, index) => {
    hashMap[item[0]] = index;
    item[2] = currentAddr;
    currentAddr += 4;
  });

  fs.writeFileSync('build/rodata.asm', `.387
include roexports.inc
CONST		SEGMENT	WORD PUBLIC USE16 'DATA'
  include roorg.inc
rostart:
`, { encoding: 'utf-8' });

  fs.writeFileSync('build/roexports.inc', '', { encoding: 'utf-8' });

  compileLexer();
}

// compile and split up lexer
function compileLexer () {
  console.log('compileLexer');

  execSync(
    'sed "' +
        's/stdin/mystdin/g;' +
        's/stdout/mystdout/g;' +
        's/stderr/mystderr/g;' +
        's/struct yy_trans_info/flex_int8_t yy_accept2(unsigned int offset);' +
        'flex_uint16_t yy_nxt2(unsigned int offset);' +
        'flex_int16_t yy_chk2(unsigned int offset);' +
        'YY_CHAR yy_ec2(unsigned int offset);' +
        'struct yy_trans_info/g;' +
        's/yyconst char msg\\[\\]/yyconst char *msg/g;' +
        's/flex_int32_t yy_rule_can_match_eol/flex_int8_t yy_rule_can_match_eol/g;' +
        's/flex_int16_t yy_accept/flex_int8_t yy_accept/g;' +
        's/yy_ec\\[YY_SC_TO_UI(\\*yy_cp)\\]/yy_ec2(YY_SC_TO_UI(*yy_cp))/g;' +
        's/yy_nxt\\[yy_base\\[yy_current_state\\] + yy_c\\]/yy_nxt2\\(yy_base\\[yy_current_state\\] + yy_c\\)/g;' +
        's/yy_chk\\[yy_base\\[yy_current_state\\] + yy_c\\]/yy_chk2\\(yy_base\\[yy_current_state\\] + yy_c\\)/g;' +
        's/yy_accept\\[yy_current_state\\]/yy_accept2\\(yy_current_state\\)/g;' +
      '" lexer.c > build/lexer2.h'
  );

  execSync(
    'wcc ' + cflags + ' -oe=0 -zc lexer2.c;wdis -s -a -l=build/lexer.asm lexer2.o'
  );

  splitUpFunctions('lexer', compileParser);
}

function compileParser () {
  console.log('compileParser');

  execSync(
    'sed -e"' +
      's/stdin/mystdin/g;' +
      's/stdout/mystdout/g;' +
      's/stderr/mystderr/g;' +
      's/YY_INITIAL_VALUE (static YYSTYPE yyval_default;)//g;' +
      's/yycheck\\[\\(.[^]]*\\)\\]/yycheck2(\\1)/g;' +
      's/yydefact\\[\\(.[^]]*\\)\\]/yydefact2(\\1)/g;' +
      's/yytable\\[\\(.[^]]*\\)\\]/yytable2(\\1)/g;' +
      's/yyr1\\[\\(.[^]]*\\)\\]/yyr1a(\\1)/g;' +
      's/#define YY_LAC_ESTABLISH/yytype_int16 yycheck2(int offset);\\n#define YY_LAC_OESTABLISH/g;' +
      's/\'s\'/\'S\'/g;' +
      's/YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);/static YYSTYPE yylval;/g;' +
      's/static const yytype_int16 yypact\\[\\]/yytype_int16 yypact2(int offset);yytype_int8 yydefact2(int offset);yytype_int8 yyr1a(int offset);yytype_int16 yytable2(int offset);static const yytype_int16 yypact[]/g;' +
      '" sql.c > build/sql2.h');

  execSync(
    'sed -i -r "' +
      's/yypact\\[([^]]+)\\]/yypact2(\\1)/gi;' +
      '" build/sql2.h');

  execSync(
    'wcc ' + cflags + ' -oe=0 -zc sql2.c;wdis -s -a -l=build/sql.asm sql2.o'
  );

  splitUpFunctions('sql', compileDos, true);
}

function compileDos () {
  console.log('compileDos');

  execSync(
    'wcc ' + cflags + ' -zc dos.c;wdis -s -a -l=build/dos.asm dos.o'
  );

  splitUpFunctions('dos', compileQueryCSV, true);
}

function compileQueryCSV () {
  console.log('compileQueryCSV');

  execSync(
    'sed -e"' +
      's/main/realmain/g;' +
      's/stderr/mystderr/g;' +
      's/stdout/mystdout/g;' +
      's/stdin/mystdin/g;' +
      '" querycsv.c > querycsv2.c');

  execSync(
    'wcc ' + cflags + ' -zc querycsv2.c;wdis -s -a -l=build/querycsv.asm querycsv2.o'
  );

  splitUpFunctions('querycsv', compileHash2, true);
}

function compileHash2 () {
  console.log('compileHash2');

  execSync(
    'sed -e"' +
      's/stderr/mystderr/g;' +
      's/stdout/mystdout/g;' +
      's/stdin/mystdin/g;' +
      's/stdin/mystdin/g;' +
      '" hash2dat.c > hash2dat2.c');

  execSync(
    'wcc ' + cflags + ' -zc hash2dat2.c;wdis -s -a -l=build/hash2dat.asm hash2dat2.o'
  );

  splitUpFunctions('hash2dat', compileHash3, true);
}

function compileHash3 () {
  console.log('compileHash3');

  execSync(
    'wcc ' + cflags + ' -zc hash3.c;wdis -s -a -l=build/hash3.asm hash3.o'
  );

  splitUpFunctions('hash3', compileHash4a, true);
}

function compileHash4a () {
  console.log('compileHash4a');

  execSync(
    'sed "' +
      '1s/^/struct hash4Entry { const char *name; int script; int index; int isNotLower; }; extern struct hash4Entry hash4export;\\n/;' +
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
    '" hash4a.c > build/hash4a.c'
  );

  execSync(
    'wcc ' + cflags + ' build/hash4a.c;wdis -s -a -l=build/hash4a.asm hash4a.o'
  );

  execSync('sed -i.bak "s/word ptr ss[:]L[$]/word ptr cs:L$/g;s/DGROUP[:]//g" build/hash4a.asm');

  splitUpFunctions('hash4a', compileHash4b, true);
}

function compileHash4b () {
  console.log('compileHash4b');

  execSync(
    'sed "' +
      '1s/^/struct hash4Entry { const char *name; int script; int index; int isNotLower; }; extern struct hash4Entry hash4export;\\n/;' +
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
    '" hash4b.c > build/hash4b.c'
  );

  execSync(
    'wcc ' + cflags + ' build/hash4b.c;wdis -s -a -l=build/hash4b.asm hash4b.o'
  );
  execSync('sed -i.bak "s/word ptr ss[:]L[$]/word ptr cs:L$/g;s/DGROUP[:]//g" build/hash4b.asm');

  splitUpFunctions('hash4b', compileHash4c, true);
}

function compileHash4c () {
  console.log('compileHash4c');

  execSync(
    'sed "' +
      '1s/^/struct hash4Entry { const char *name; int script; int index; int isNotLower; }; extern struct hash4Entry hash4export;\\n/;' +
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
    '" hash4c.c > build/hash4c.c'
  );

  execSync(
    'wcc ' + cflags + ' build/hash4c.c;wdis -s -a -l=build/hash4c.asm hash4c.o'
  );

  execSync('sed -i.bak "s/word ptr ss[:]L[$]/word ptr cs:L$/g;s/DGROUP[:]//g" build/hash4c.asm');

  splitUpFunctions('hash4c', buildData, true);
}

function buildData () {
  let name;
  console.log('buildData');

  /* patch compareCodepoints into the functions that need it (so the table is
  always in the same page) */
  execSync(
    'cat build/s/compareCodepoints.asm >> build/s/getBytes.asm;' +
    'rm build/s/compareCodepoints.asm');

  name = 'sortCodepoints_';
  execSync(
    'cat build/s/sortCodepoints.asm >> build/s/getBytes.asm;'
  );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 1, currentAddr, 0x0001, 'farcall']);
  currentAddr += 4;

  name = 'sortCodesParse_';
  execSync(
    'cat build/s/sortCodepoints.asm >> build/s/parse_mbcs.asm;' +
    "sed -i 's/sortCodepoints_/" + name + "/g;' build/s/parse_mbcs.asm"
  );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 1, currentAddr, 0x0001, 'farcall']);
  currentAddr += 4;

  execSync('rm build/s/sortCodepoints.asm');

  name = 'sortBytesParse_';
  execSync(
    'cat build/s/sortBytes.asm >> build/s/parse_mbcs.asm;' +
    "sed -i 's/sortBytes_/" + name + "/g;' build/s/parse_mbcs.asm"
  );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 1, currentAddr, 0x0001, 'farcall']);
  currentAddr += 4;

  name = 'sortBytes_';
  execSync(
    'cat build/s/sortBytes.asm >> build/s/getCodepointsMbcs.asm;'
  );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 1, currentAddr, 0x0001, 'farcall']);
  currentAddr += 4;

  execSync('rm build/s/sortBytes.asm');

  compileLibC();

  const lineReader = readline.createInterface({
    input: fs.createReadStream('build/qrycsv16.map')
  });

  lineReader.on('line', line => {
    if ((/^_NULL/).test(line) && codeOffset === 0) {
      codeOffset = parseInt((line.match(/_NULL {18}BEGDATA {8}DGROUP {9}([^:]+)[:]0000 {7}0000(.+)/))[1], 16) * 16;
    } else {
      const match = line.match(/[:]([0-9a-f]+)[ *+] {5}(.*)/);

      if (match !== null) {
        const name = match[2];
        //        console.log(name);
        // name, pageNo, trampolineAddr, pageAddr, callMethod

        if (hasProp(hashMap, name)) {
          functionsList[hashMap[name]][3] = parseInt(match[1], 16);
        } else if (hasProp(hashMap, name + '_')) {
          functionsList[hashMap[name + '_']][3] = parseInt(match[1], 16);
        } else if (hasProp(defines, match[2])) {
          defines[match[2]] = parseInt(match[1], 16);
        }
      }
    }
  });

  lineReader.on('close', () => {
    addROData();
  });
}

function compileLibC () {
  console.log('compileLibC');

  fs.writeFileSync('build/exports.inc', functionsList.reduce((acc, item) => acc + `  PUBLIC ${item[0]}
`, '') + (() => {
    let key;
    let acc = '';

    for (key in defines) {
      if (hasProp(defines, key)) {
        acc += `  PUBLIC ${key}` + '\n';
      }
    }

    return acc;
  })(), 'utf8');

  fs.writeFileSync('build/functions.inc', functionsList.reduce((acc, item) => {
    if (hasProp(ignoreFunctions, item[0])) {
      return acc;
    }
    return acc + `${item[0]}:
    call ${item[4]}
    db 0x${('00' + ((item[1] | 0)).toString(16)).slice(-2)}
`;
  }, ''), 'utf8');

  // name, pageNo, trampolineAddr, pageAddr, callMethod

  fs.writeFileSync('build/lookupTable.inc', functionsList.reduce((acc, item) => {
    if (hasProp(ignoreFunctions, item[0])) {
      return acc;
    }
    return acc + `dw 0x${('0000' + ((item[3] | 0)).toString(16)).slice(-4).toUpperCase()}  ; ${item[0]}
`;
  }, ''), 'utf8');

  /* compile the data immediately above the function jump table */
  execSync('cp libc.c en_gb.h pager.asm build/;cd build;wasm -0 -fo=pager.obj pager.asm; wcl -mc -fpc -0 -zdf -ob -oh -ou -ot -or -od -ox -d3 -fm=qrycsv16 -fe=qrycsv pager.obj libc.c');
}

function writeROLinkScript (offset) {
  fs.writeFileSync('build/roorg.inc', `org 0x${('0000' + ((offset | 0)).toString(16)).slice(-4)}`, { encoding: 'utf-8' });
  fs.writeFileSync('build/rodata.lnk', `option map=rodata.map
option stack=512
name ${offset === 0 ? 'rodata' : 'temp'}.bin
output raw
  offset=0x10
file rodata.o
order
 clname CONST
 clname BSS NOEMIT
`, { encoding: 'utf-8' });
  execSync('cd build;wasm rodata.asm -fo=rodata.o;wlink @rodata.lnk;rm rodata.o', { stdio: 'pipe' });
}

/*
  Compile the read only data to a section of memory at the top of the z80 address space.
  This will be appended to each memory page so will effectively be always paged in */
function addROData () {
  console.log('addROData');

  fs.appendFileSync('build/rodata.asm', `    CONST		ENDS
BSS		SEGMENT	WORD PUBLIC USE16 'STACK'
BSS		ENDS
		END rostart`);

  /* build the global data but exclude the hash4 strings */
  writeROLinkScript(0);

  rodataSize = fs.statSync('build/rodata.bin').size;

  pageSize = 16384 - rodataSize;

  /* build the rodata located at the very top of ram */
  writeROLinkScript(codeOffset + pageSize);

  pageSize -= 190; // underfit the pages by 190 bytes as they seem to grow a little bit
  console.log(pageSize);

  execSync('rm build/temp.bin');

  /* add the address of each rodata item as an assembly include file for anything that may need to reference it later */
  fs
    .readFileSync('build/rodata.map', 'utf8')
    .replace(/[:]([0-9a-f]+)[ +*] {5}(.+)/g, (one, three, two, ...arr) => {
      const text = 'IFNDEF ' + two + '\n' +
            two + ' = 0x' + three + '\n' +
          'ENDIF\n';
      fs.writeFileSync(
          `build/ro/${two}.asm`,
          text,
          'utf8'
      );

      fs.appendFileSync('build/rodata2.asm', text);
    });

  getFunctionSizes();
}

function getFunctionSizes () {
  console.log('getFunctionSizes');

  const list = [];

  const walker = walk.walk('./build/s', {});

  walker.on('file', (root, fileStats, next) => {
    if (/\.asm$/.test(fileStats.name)) {
      list.push(fileStats.name.replace(/\.asm$/, '').replace(/^_/, ''));
    }

    next();
  });

  walker.on('errors', (root, nodeStatsArray, next) => {
    next();
  });

  walker.on('end', () => {
    packPages(
      list
        .map(elem => addDefines(elem, [elem], 's'))
        .reduce((obj, elem) => {
          elem.children = elem.children
            .map(elem => elem.replace(/_$/, ''))
            .filter(name => fs.existsSync(path.join(__dirname, 'build', 'bin', name + '.bin')));

          try {
            elem.size = fs.statSync(path.join(__dirname, 'build', 'bin', elem.name + '.bin')).size;
          } catch (e) {}

          obj[elem.name] = elem;

          return obj;
        }, {})
    );
  });
}

function packPages (tree) {
  console.log('packPages');
  /* use a bin packing algorithm to group the functions close
  to their call-stack parents and produce a binary of each 16k page */

  const pages = [[tree.realmain]];
  const remainingSizes = [pageSize];
  let currentPageData = {};
  let currentPageNumber = 0;
  const placedFunctions = [tree.realmain];
  let currentFunctions = [];

  // place the main function to begin with
  tree.realmain.pageNumber = 0;
  tree.realmain.children.forEach(elem => {
    currentPageData[elem] = true;
  });

  do {
    // temporarily re-add functions that've already been placed so we can add their children
    currentFunctions = [].concat(placedFunctions);

    for (let loop = 3; loop; loop--) { // try to keep functions fairly close to
    // something further up the call stack
      // find the children of all the current functions in the array.
      const children = currentFunctions
        .map(elem => elem.children)
        .reduce((obj, elem) => {
          obj = obj.concat(elem);

          return obj;
        }, [])
        .map(elem => tree[elem]);

      // if no children were found then open a new page
      if (children.length === 0) {
        break;
      }

      currentFunctions = currentFunctions.concat(children);

      // sort by size and filter the functions that've already been allocated
      currentFunctions = currentFunctions
        .sort((a, b) => {
          // prioritize functions whose parent function is already in the current page
          const temp = hasProp(currentPageData, a.name) - hasProp(currentPageData, b.name);

          if (!temp) {
            return temp;
          }

          return a.size - b.size;
        })
        .reduce((obj, elem) => {
          if (!(hasProp(elem, 'pageNumber') || hasProp(obj.seenNames, elem.name))) {
            obj.seenNames[elem.name] = true;
            obj.newArr.push(elem);
          }

          return obj;
        }, { seenNames: {}, newArr: [] }).newArr;

      // place the biggest first. if all of then add the children. if none of the children can
      for (let i = 0; i < currentFunctions.length; i++) {
        for (let k = currentPageNumber, m = 3; m > 0; k--, m--) {
          if (remainingSizes[k] - currentFunctions[i].size > 0) {
            remainingSizes[k] -= currentFunctions[i].size;
            pages[k].push(currentFunctions[i]);
            placedFunctions.push(currentFunctions[i]);
            currentFunctions[i].pageNumber = k;
            currentFunctions[i].children.forEach(elem => {
              currentPageData[elem] = true;
            });
          }
        }
      }
    }

    // filter out placed functions
    currentFunctions = currentFunctions
      .filter(elem => !(hasProp(elem, 'pageNumber')));

    // if all functions have been placed then quit
    if (currentFunctions.length === 0) {
      break;
    }
    // else {
    //  console.log(currentFunctions);
    // }

    // otherwise open a new page
    pages.push([]);
    remainingSizes.push(pageSize);
    currentPageData = {};
    currentPageNumber++;
  } while (1);

  compilePages(pages);
}

function compilePages (pages) {
  console.log('compilePages');

  Promise.all(pages.map((elem, index) => (new Promise(resolve => {
    addDefines('qcsv' + ('00' + (index + 1)).slice(-2) + 'pc', elem.map(elem2 => elem2.name), 'h', true);

    /*
    if (fs.statSync('build/obj2/page' + (index + 3) + '_code_compiler.bin').size > (16384 - rodataSize)) {
      console.log('page ' + (index + 3) + ' is too big');
      process.exit(-1);
    }
    */

    const lineReader = readline.createInterface({
      input: fs.createReadStream('build/qcsv' + ('00' + (index + 1)).slice(-2) + 'pc.map')
    });

    lineReader.on('line', line => {
      console.log(line);

      const match = line.match(/[:]([0-9a-f]+)[ *+] {5}(.*)/);

      if (match !== null) {
        const name = match[2];
        console.log(name);
        // name, pageNo, trampolineAddr, pageAddr, callMethod

        if (hasProp(hashMap, name)) {
          functionsList[hashMap[name]][3] = parseInt(match[1], 16);
        } else if (hasProp(hashMap, name + '_')) {
          functionsList[hashMap[name + '_']][3] = parseInt(match[1], 16);
        } else if (hasProp(defines, match[2])) {
          defines[match[2]] = parseInt(match[1], 16);
        }
      }
    });

    lineReader.on('close', () => {
      execSync('dd if=/dev/zero bs=1 count=16384 of=output/qcsv' + ('00' + (index + 1)).slice(-2) + 'pc.ovl');

      execSync('dd if=build/obj2/qcsv' + ('00' + (index + 1)).slice(-2) + 'pc.bin bs=' + codeOffset + ' skip=1 of=output/qcsv' +
      ('00' + (index + 1)).slice(-2) + 'pc.ovl conv=notrunc');

      execSync('dd if=build/rodata.bin of=output/qcsv' +
      ('00' + (index + 1)).slice(-2) + 'pc.ovl bs=1 seek=' + (16384 - rodataSize) + ' conv=notrunc');

      resolve();
    });
  })))).then(() => {
    compileLibC();
  });
}

/* *** HELPER FUNCTIONS AFTER THIS POINT *** */

/*
  Split the specified assembly file into one file for each function's code
  (including the necessary rodata) and append to a global data segment.
  Pause if node.js can't keep up
*/
function splitUpFunctions (filename, callback, append) {
  console.log('splitUpFunctions', filename);

  let j;

  const lineReader = readline.createInterface({
    input: fs.createReadStream('build/' + filename + '.asm')
  });

  const data = fs.createWriteStream('build/data.inc', {
    flags: 'a'
  });

  const code = fs.createWriteStream('build/rodata.asm', {
    flags: 'a'
  });

  const includes = fs.createWriteStream('build/roexports.inc', {
    flags: 'a'
  });

  const rodataOutputStreams = [];

  const functionOutputStreams = [];
  let activeStream = code;
  let state = 0;
  let labelBuffer = '';
  let offset = 0;
  let recentLabel = '';
  let usedRecentLabel = '';

  const smarter = new RegExp(`L\\$1_${filename}-[0-9a-f]+H`);

  lineReader.on('line', line => {
    let name;

    line = line.replace(/(L\$[0-9]+)/, '$1_' + filename).replace(smarter, usedRecentLabel.replace(':', ''));

    switch (state) {
      case 0: {
        if (/^_TEXT\s+SEGMENT\s+[A-Z]+\s+PUBLIC\s+USE16\s+'CODE'/.test(line)) {
          state = 1;
        }
      } break;

      case 1: {
        if (/^\s+DW/.test(line)) {
          if (labelBuffer === '') {
            labelBuffer = recentLabel;
          }

          if (usedRecentLabel !== labelBuffer) {
            name = labelBuffer.match(/^(L\$[^:]+):/)[1];
            usedRecentLabel = labelBuffer;

            rodataOutputStreams.push(fs.createWriteStream('build/ro/' + name.replace('$', '_') + '.asm'));

            /* add to the list of rodata regexes used to add the appropriate rodata to each function */
            rodataLabels.push([
              name,
              false,
              new RegExp('(\\b' + name.replace(matchOperatorsRe, '\\$&') + '\\b)', 'm')
            ]);

            activeStream = rodataOutputStreams[rodataOutputStreams.length - 1];
            writePause(activeStream, // 'IFNDEF ' + name + '\n' +
              labelBuffer + '\n' + line + '\n');
            return;
          }

          writePause(activeStream, line + '\n');
          return;
        }

        if (labelBuffer !== '' && usedRecentLabel !== labelBuffer) {
          writePause(activeStream, labelBuffer + '\n');
          if (activeStream === code) {
            writePause(includes, 'PUBLIC ' + labelBuffer.replace(':', '') + '\n');
          }
          labelBuffer = '';
        }

        if (/^CONST\s+SEGMENT\s+[A-Z]+\s+PUBLIC\s+USE16\s+'DATA'/.test(line)) {
          state = 2;
        } else if (/^L\$[^:]+:/.test(line)) {
          labelBuffer = line;
          recentLabel = line.replace(':', 'A:');
        } else if (!([
          '_TEXT		ENDS',
          '		ASSUME CS:_TEXT, DS:DGROUP, SS:DGROUP'
        ].includes(line))) {
          if (/^([^_](([^_]|_[^:]))+)_:$/.test(line)) {
            name = line.match(/^(([^_]|_[^:])+)_:$/)[1];

            functionOutputStreams.push(fs.createWriteStream('build/s/' + name + '.asm'));

            switch (name) {
              case 'realmain':
              case 'sortCodepoints':
              case 'sortBytes':
                break;

              default:
                hashMap[name + '_'] = functionsList.length;
                functionsList.push([name + '_', 1, currentAddr, 0x0001, 'farcall']);
                currentAddr += 4;
            }

            activeStream = functionOutputStreams[functionOutputStreams.length - 1];
          } else if (/^_(([^_]|_[^:])+):$/.test(line) && !(/^_hash2/.test(line) && line !== '_hash2_1:')) {
            name = line.match(/^([^:]+):$/)[1];

            rodataOutputStreams.push(fs.createWriteStream('build/ro/' + (name === '_hash2_1' ? '_hash2_' : name) + '.asm'));

            /* add to the list of rodata regexes used to add the appropriate rodata to each function */
            rodataLabels.push([
              name,
              false,
              new RegExp('(\\b' + name.replace(matchOperatorsRe, '\\$&') + '\\b)', 'm')
            ]);

            activeStream = rodataOutputStreams[rodataOutputStreams.length - 1];

            // writePause(activeStream, 'IFNDEF ' + name + '\n');
          }

          writePause(
            activeStream,
            line.replace('DGROUP', 'SS') + '\n'
          );
        }
      } break;

      case 2: {
        if (/^_DATA\s+SEGMENT\s+[A-Z]+\s+PUBLIC\s+USE16\s+'DATA'/.test(line)) {
          state = 3;
          activeStream = data;
        } else if ([
          'CONST2		ENDS',
          "_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'",
          '_DATA		ENDS',
          'CONST		ENDS',
          "CONST2		SEGMENT	WORD PUBLIC USE16 'DATA'",
          '		END'].includes(line)) {

        } else {
          if (/^[^:]+:$/.test(line) && !(/^_hash2/.test(line) && line !== '_hash2_1:')) {
            name = line.match(/^([^:]+):$/)[1];

            rodataOutputStreams.push(fs.createWriteStream('build/ro/' + (name === '_hash2_1' ? '_hash2_' : name.replace('$', '_')) + '.asm'));

            /* add to the list of rodata regexes used to add the appropriate rodata to each function */
            rodataLabels.push([
              name,
              false,
              new RegExp('(\\b' + name.replace(matchOperatorsRe, '\\$&') + '\\b)', 'm')
            ]);

            activeStream = rodataOutputStreams[rodataOutputStreams.length - 1];

            // writePause(
            //  activeStream,
            //  'IFNDEF ' + name + '\n'
            // );
          }

          writePause(
            activeStream,
            line +
            '\n'
          );
        }
      } break;

      case 3: {
        if (/^_BSS\s+SEGMENT\s+[A-Z]+\s+PUBLIC\s+USE16\s+'BSS'/.test(line)) {
          state = 4;
        } else if ([
          'CONST2		ENDS',
          "_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'",
          '_DATA		ENDS',
          'CONST		ENDS',
          "CONST2		SEGMENT	WORD PUBLIC USE16 'DATA'",
          '		END'].includes(line)) {

        } else {
          if (/^([^:]+):$/.test(line)) {
            const name = line.match(/^([^:]+):$/)[1];

            defines[name] = 1;
          }

          writePause(
            activeStream,
            line + '\n'
          );
        }
      } break;

      case 4: {
        if (/^L\$[0-9]+/.test(line)) {
          const name = line.match(/^(L\$[0-9]+)/)[1] + '_' + filename;
          defines[name] = 1;

          writePause(
            activeStream,
            name + ':\n'
          );
        } else if ([
          'CONST2		ENDS',
          "_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'",
          '_DATA		ENDS',
          'CONST		ENDS',
          "CONST2		SEGMENT	WORD PUBLIC USE16 'DATA'",
          '		END'].includes(line)) {

        } else if (/ORG/.test(line)) {
          const num = parseInt(line.match(/ORG ([0-9]+)$/)[1] || '0', 10);

          if (offset + num !== 0) {
            writePause(
              activeStream,
              '    DB	' + (num - offset) + ' DUP(0)\n'
            );
          }

          offset = num;
        }
      } break;
    }
  });

  lineReader.on('close', () => {
    j = functionOutputStreams.length + rodataOutputStreams.length + 2;

    data.end(writeFunctionPostfixes);
  });

  function writeFunctionPostfixes () {
    code.end(allStreamsClosed);
    includes.end(allStreamsClosed);

    for (let i = 0; i < rodataOutputStreams.length; i++) {
      /* close current stream */
      // writePause(rodataOutputStreams[i], 'ENDIF\n');
      rodataOutputStreams[i].end(allStreamsClosed);
    }

    for (let i = 0; i < functionOutputStreams.length; i++) {
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

/* updateName adds to rodata values needed by each function to the end of
its assembly source file */
/* function updateName (elem) {
  const text = elem[1];

  let hasMatches = false;

  rodataLabels.forEach(element => {
    if (element[1] = element[1] || element[2].test(text)) {
      hasMatches = true;

      if (element[0] === '_st_in_word_set_a_wordlist') {
        rodataLabels.find(element => element[0] === 'i_1hash4a')[1] = true;
      } else if (element[0] === '_st_in_word_set_b_wordlist') {
        rodataLabels.find(element => element[0] === 'i_1hash4b')[1] = true;
      } else if (element[0] === '_st_in_word_set_c_wordlist') {
        rodataLabels.find(element => element[0] === 'i_1hash4c')[1] = true;
      }
    }
  });

  if (hasMatches) {
    execSync(
      'echo "\tSECTION rodata_compiler" >> build/s/' + elem[0] + ' && ' +
        'cat ' +
          shellEscape(rodataLabels.filter(label => label[1]).map(label => 'build/ro/' + label[0] + '.asm')) +
          ' >> build/s/' + elem[0] + ' && ' +
        'echo "\tSECTION code_compiler" >> build/s/' + elem[0]
    );
  }

  rodataLabels.forEach(element => {
    element[1] = false;
  });
} */

function addDefines (filename, filenames, folderName, pageMode) {
  let arr = [];
  let arr1 = [];
  let arr2 = [];
  const arr3 = [];
  let notQuit = true;

  console.log('addDefines', filenames);
  fs.writeFileSync('build/exports2.inc', filenames.reduce((result, elem) => {
    switch (elem) {
      case 'parse_mbcs':
        result += '  PUBLIC sortCodesParse_\n  PUBLIC sortBytesParse_\n';
        break;

      case 'getBytes':
        result += '  PUBLIC compareCodepoints_\n  PUBLIC sortCodepoints_\n';
        break;

      case 'getCodepointsMbcs':
        result += '  PUBLIC sortBytes_\n';
        break;
    }

    return result + `  PUBLIC ${elem}_` + '\n';
  }, ''), 'utf8');

  fs.writeFileSync('build/function.asm', `.8087
INCLUDE <exports2.inc>
DGROUP		SEGMENT	BYTE PUBLIC USE16 'CODE'
		ASSUME CS:DGROUP, DS:DGROUP, ES:DGROUP, SS:DGROUP
INCLUDE <rodata2.asm>
INCLUDE <defines.inc>
` +
(folderName === 'h'
  ? `ORG ${codeOffset}
${filename}:
`
  : '') +
`INCLUDE <funcdata.inc>
` + filenames.reduce((result, elem) => result + '\n' + `INCLUDE <s/${elem.replace(/_$/, '')}.asm>`, '') +
`
DGROUP ENDS
stack segment 'STACK'
        db 10H dup(0)
stack ends
END ${filename}`, 'utf8');

  fs.writeFileSync('build/defines.inc', (() => {
    let key;
    let acc = '';

    if (folderName === 'h') {
      for (key in defines) {
        if (hasProp(defines, key)) {
          acc += `  ${key} = ${defines[key]}` + '\n';
        }
      }
    }

    return acc;
  })(), 'utf8');
  fs.writeFileSync('build/funcdata.inc', '', 'utf8');

  fs.writeFileSync('build/rodata.lnk', (folderName === 'h' ? 'option map=' + filename + '.map\n' : '') +
`option stack=512
name ${folderName === 'h' ? 'obj2' : 'bin'}/${filename}.bin
output raw
  offset=0x20
file obj/${filename}.obj
order
 clname DGROUP
 clname stack NOEMIT
`, { encoding: 'utf-8' });

  while (notQuit) {
    notQuit = false;

    try {
      execSync(
        'wasm -e10000 -0 ' + ' ' + (pageMode ? '' : '') + '-fo=obj/' + filename + '.obj function.asm',
        {
          cwd: path.join(__dirname, 'build'),
          stdio: 'pipe'
        }
      );

      execSync('wlink @rodata.lnk',
        {
          cwd: path.join(__dirname, 'build')//,
          // stdio: 'pipe'
        });
    } catch (e) {
      notQuit = true;

      // console.log(e.stderr.toString() + e.stdout.toString());

      /*
      if (folderName === 'h') {
        if (++count > 1) {
          process.exit(0);
        }
      }
      */

      // create an array of all the missing symbol names
      arr = Array.from(new Set(arr.concat(matchAll(e.stderr.toString() + e.stdout.toString(), /E551: Symbol ([^' \r\n]+)/g).toArray())));

      arr.forEach(item => {
        // console.log(item);
        (fs.existsSync(`build/ro/${item.replace('$', '_')}.asm`) ? arr2 : arr1).push(item);
      });

      arr1 = arr1.sort().filter((element, index, array) => array.indexOf(element) === index);
      arr2 = arr2.sort().filter((element, index, array) => array.indexOf(element) === index);

      const arr4 = arr1.sort((a, b) => {
        // console.log(a,b);
        const c = a;
        const d = b;

        if (!(hasProp(hashMap, c))) {
          return 1;
        }

        if (!(hasProp(hashMap, d))) {
          return -1;
        }

        return functionsList[hashMap[c]][2] - functionsList[hashMap[d]][2];
      });

      fs.writeFileSync('build/defines.inc', arr4.reduce((acc, item) => {
        if (folderName !== 'h') {
          return acc + '  ' + item + ':\n';
        }

        const name = item;

        if (hasProp(hashMap, name)) {
          const a = acc + `
IFNDEF ${item}
org ${functionsList[hashMap[name]][2]}
${item}:
  db 0
ENDIF
          `;

          return a;
        }

        if (hasProp(hashMap, name + '_')) {
          const a = acc + `
IFNDEF ${item}
org ${functionsList[hashMap[name + '_']][2]}
${item}:
  db 0
ENDIF
          `;

          return a;
        }

        return acc;
      }, '') + (() => {
        let key;
        let acc = '';

        if (folderName === 'h') {
          for (key in defines) {
            if (hasProp(defines, key)) {
              acc += `  ${key} = 0x${(defines[key] + codeOffset).toString(16)}` + '\n';
            }
          }
        }

        return acc;
      })(), 'utf8');

      fs.writeFileSync('build/funcdata.inc', arr2.reduce((acc, item) => acc + `  INCLUDE <ro/${item.replace('$', '_')}.asm>` + '\n', ''), 'utf8');

      for (let i = 0, len = arr.length; i < len; ++i) {
        arr3.push(arr[i]);
      }
    }
  }

  return {
    name: filename,
    children: arr3.sort().filter((element, index, array) => array.indexOf(element) === index)
  };
}

/*
function abort (elem) {
  console.log('symbol not found: ' + elem);
  process.exit(-1);
}
*/
