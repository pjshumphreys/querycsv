/*
  we'll need to generate the loaders and c library pages 2 times

  CP/M & MSXDOS 1
  MSXDOS 2

all other pages should be sharable

-O0 --c-code-in-asm
*/

const childProcess = require('child_process');
const spawnSync = childProcess.spawnSync;
const execSync = childProcess.execSync;
const fs = require('graceful-fs');
const path = require('path');
const readline = require('readline');
const walk = require('walk');
const shellEscape = require('shell-escape');
const matchAll = require('match-all');

const hasProp = (obj, prop) =>
  Object.prototype.hasOwnProperty.bind(obj)(prop);

const matchOperatorsRe = /[|\\{}()[\]^$+*?.]/g;

const hashMap = {};

let rodataSize = 0;

const functionsList = [
  ['main', 3, 0x0001, 0x4000, 'farcall'],
  ['exit', 1, 0x0001, 0x0001, 'farcall2'],
  ['strcmp_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['stricmp_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['strlen', 1, 0x0001, 0x0001, 'farcall2'],
  ['strstr_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['strcat_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['strncat_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['strnicmp_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['strcpy_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['strncpy_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['memcpy_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['memmove_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['memset_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['_fopen_z80', 1, 0x0001, 0x0001, 'farcall2'],
  ['fclose', 1, 0x0001, 0x0001, 'farcall2'],
  ['fread', 1, 0x0001, 0x0001, 'farcall2'],
  ['_fwrite_z80', 1, 0x0001, 0x0001, 'farcall2'],
  ['_macYield', 1, 0x0001, 0x0001, 'farcall2'],
  ['_fprintf_z80', 1, 0x0001, 0x0001, 'farcall2'],
  ['_fputs_z80', 1, 0x0001, 0x0001, 'farcall2'],
  ['fputs_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['_malloc_z80', 1, 0x0001, 0x0001, 'farcall2'],
  ['_free_z80', 1, 0x0001, 0x0001, 'farcall2'],
  ['_realloc_z80', 1, 0x0001, 0x0001, 'farcall2'],
  ['_reallocMsg', 1, 0x0001, 0x0001, 'farcall2'],
  ['atexit', 1, 0x0001, 0x0001, 'farcall2'],
  ['_initMapper', 1, 0x0001, 0x0001, 'farcall2'],
  ['_cleanup_z80', 1, 0x0001, 0x0001, 'farcall2'],
  ['_atexit_z80', 1, 0x0001, 0x0001, 'farcall2'],
  ['fseek', 1, 0x0001, 0x0001, 'farcall2'],
  ['fgetc', 1, 0x0001, 0x0001, 'farcall2'],
  ['ungetc', 1, 0x0001, 0x0001, 'farcall2'],
  ['feof', 1, 0x0001, 0x0001, 'farcall2'],
  ['sprintf', 1, 0x0001, 0x0001, 'farcall2'],
  ['isspace', 1, 0x0001, 0x0001, 'farcall2'],
  ['isdigit', 1, 0x0001, 0x0001, 'farcall2'],
  ['abs', 1, 0x0001, 0x0001, 'farcall2'],
  ['bdos', 1, 0x0001, 0x0001, 'farcall2'],
  ['strrchr_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['atol', 1, 0x0001, 0x0001, 'farcall2'],
  ['ltoa_callee', 1, 0x0001, 0x0001, 'farcall2'],
  ['ftoa', 1, 0x0001, 0x0001, 'farcall2']
];

/* don't include these functions in the output files as they are never invoked (dead code elimination) */
const ignoreFunctions = [
  'yyunput',
  'input',
  'yypush_buffer_state',
  'yyget_extra',
  'yyget_lineno',
  'yyget_column',
  'yyget_in',
  'yyget_out',
  'yyget_leng',
  'yyget_text',
  'yyset_lineno',
  'yyset_column',
  'yyset_out',
  'yyget_debug',
  'yyset_debug',
  'yyget_lval',
  'yyset_lval',
  'yylex_init'
].reduce((acc, item) => {
  acc[item] = !0;
  return acc;
}, {});

const startOffset = 0x1c5;//0x0200;

let currentAddr = startOffset;

/* contains a map of the addresses of all global variables */
const defines = {};

const libCDefines = ['_myHeap', '_next', '_current', '__sgoioblk'];

const rodataLabels = [];
const byteMaps = {};
let pageSize;

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
    'mkdir -p build/msx2;' +
    'mkdir -p output;' +
    'rm -f build/rodata2.asm;' +
    'rm -f build/data.asm;' +
    'touch build/data.asm'
  );

  /* generate the large static arrays in the format z88dk needs */
  execSync('node ../../generateMappings.js true');

  // update the jump table locations, starting at call_rom3 -4 and working downward in memory
  functionsList.forEach((item, index) => {
    hashMap[item[0].replace(/^_/, '')] = index;
    item[2] = currentAddr;
    currentAddr += 4;
  });

  compileLexer();
}

// compile and split up lexer
function compileLexer () {
  console.log('compileLexer');

  execSync(
    'sed "' +
      's/flex_int32_t yy_rule_can_match_eol/' +
        'flex_int8_t yy_rule_can_match_eol/g;' +
      's/flex_int16_t yy_accept/' +
        'flex_int8_t yy_accept/g;' +
      's/struct yy_trans_info/' +
        'flex_int16_t yy_accept2(unsigned int offset);' +
        'flex_int16_t yy_nxt2(unsigned int offset);' +
        'flex_int16_t yy_chk2(unsigned int offset);' +
        'YY_CHAR yy_ec2(unsigned int offset);' +
        'struct yy_trans_info/g;' +
      's/yy_ec\\[YY_SC_TO_UI(\\*yy_cp)\\]/' +
        'yy_ec2(YY_SC_TO_UI(*yy_cp))/g;' +
      's/yy_nxt\\[yy_base\\[yy_current_state\\] + yy_c\\]/' +
        'yy_nxt2\\(yy_base\\[yy_current_state\\] + yy_c\\)/g;' +
      's/yy_nxt\\[yy_base\\[yy_current_state\\] + (flex_int16_t) yy_c\\]/' +
        'yy_nxt2\\(yy_base\\[yy_current_state\\] + (flex_int16_t) yy_c\\)/g;' +
      's/yy_chk\\[yy_base\\[yy_current_state\\] + yy_c\\]/' +
        'yy_chk2\\(yy_base\\[yy_current_state\\] + yy_c\\)/g;' +
      's/yy_accept\\[yy_current_state\\]/' +
        'yy_accept2\\(yy_current_state\\)/g;' +
      '" lexer.c > build/lexer2.h'
  );

  execSync(
    'zcc +cpm -O0 --c-code-in-asm -Cc-params-offset=6 -D__DISABLE_BUILTIN -U__STDC_VERSION__ lexer2.c -E -o build/lexer3.c && ' +
      '../../makeheaders -h build/lexer3.c | grep -v __LIB__ | grep -v extern | grep -v \\#define | sort | uniq > build/lexer3.h'
  );

  execSync(
    'zcc +cpm -O0 --c-code-in-asm -Cc-params-offset=6 -D__DISABLE_BUILTIN -U__STDC_VERSION__ build/lexer3.c -S -o build/lexer.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0lexer/g" build/lexer.asm'
  );

  splitUpFunctions('lexer', compileParser);
}

function compileParser () {
  console.log('compileParser');

  execSync(
    'sed "' +
      's/\\[[+][*]/\\[0+*/g;' +
      's/[(][+][*]/(0+*/g;' +
      's/YY_INITIAL_VALUE (static YYSTYPE yyval_default;)/' +
        'static YYSTYPE yyval_default;/g;' +
      's/YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);/' +
        'YYSTYPE yylval;/g;' +
      's/typedef enum yysymbol_kind_t yysymbol_kind_t;//g;' +
      's/enum yysymbol_kind_t/enum yysymbol_kind_e/g;' +
      's/#ifndef YYPTRDIFF_T/typedef yytype_int16 yysymbol_kind_t;\\n#ifndef YYPTRDIFF_T/g;' +
      's/yycheck\\[\\(.[^]]*\\)\\]/yycheck2(\\1)/g;' +
      's/#define YY_LAC_ESTABLISH/yytype_int16 yycheck2(int offset);\\n#define YY_LAC_ESTABLISH/g;'+
      's/char const/' +
        'char/g;' +
      's/yyssp = yyss = yyssa;/yylval = yyval_default;yyssp = yyss = yyssa;/g;' +
      's/static const yytype_int16 yypact\\[\\]/' +
        'yytype_int16 yypact2(int offset);' +
        'static const yytype_int16 yypact[]/g;' +
    '" sql.c > build/sql2.h'
  );

  execSync(
    'sed -i -r "' +
      's/yypact\\[([^]]+)\\]/yypact2(\\1)/gi;' +
      '" build/sql2.h'
  );

  execSync(
    'zcc +cpm -O0 --c-code-in-asm -Cc-params-offset=6 -D__DISABLE_BUILTIN -U__STDC_VERSION__ sql2.c -E -o build/sql3.c && ' +
      '../../makeheaders -h build/sql3.c | grep -v __LIB__ | grep -v extern | grep -v \\#define | sort | uniq > build/sql3.h'
  );

  execSync(
    'zcc +cpm -O0 --c-code-in-asm -Cc-params-offset=6 -D__DISABLE_BUILTIN -U__STDC_VERSION__ sql2.c -S -o build/sql.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0sql/g" build/sql.asm'
  );

  splitUpFunctions('sql', compileLibC2, true);
}

function compileLibC2 () {
  console.log('compileLibC2');

  execSync(
    'zcc +cpm -O0 --c-code-in-asm -Cc-params-offset=6 -D__DISABLE_BUILTIN -U__STDC_VERSION__ libc2.c -S -o build/libc2.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0libc2/g" build/libc2.asm'
  );

  splitUpFunctions('libc2', compileQueryCSV, true);
}

function compileQueryCSV () {
  console.log('compileQueryCSV');

  execSync(
    'zcc +cpm -O0 --c-code-in-asm -Cc-params-offset=6 -D__DISABLE_BUILTIN -U__STDC_VERSION__ querycsv.c -S -o build/querycsv.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0querycsv/g" build/querycsv.asm'
  );

  splitUpFunctions('querycsv', compileHash2, true);
}

function compileHash2 () {
  console.log('compileHash2');

  execSync(
    'zcc +cpm -O0 --c-code-in-asm -Cc-params-offset=6 -D__DISABLE_BUILTIN -U__STDC_VERSION__ hash2dat.c -S -o build/hash2.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0hash2/g" build/hash2.asm'
  );

  splitUpFunctions('hash2', compileHash3, true);
}

function compileHash3 () {
  console.log('compileHash3');

  execSync(
    'zcc +cpm -O0 --c-code-in-asm -Cc-params-offset=6 -D__DISABLE_BUILTIN -U__STDC_VERSION__ hash3.c -S -o build/hash3.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0ghash3/g" build/hash3.asm'
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
    'zcc +cpm -O0 --c-code-in-asm -Cc-params-offset=6 -D__DISABLE_BUILTIN -U__STDC_VERSION__ build/hash4a.c -S -o build/hash4a.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0hash4a/g" build/hash4a.asm'
  );

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
    'zcc +cpm -O0 --c-code-in-asm -Cc-params-offset=6 -D__DISABLE_BUILTIN -U__STDC_VERSION__ build/hash4b.c -S -o build/hash4b.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0hash4b/g" build/hash4b.asm'
  );

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
    'zcc +cpm -O0 --c-code-in-asm -Cc-params-offset=6 -D__DISABLE_BUILTIN -U__STDC_VERSION__ build/hash4c.c -S -o build/hash4c.asm;' +
    'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0hash4c/g" build/hash4c.asm'
  );

  splitUpFunctions('hash4c', addROData, true);
}

/*
  Compile the read only data to a section of memory at the top of the z80 address space.
  This will be appended to each memory page so will effectively be always paged in */
function addROData () {
  console.log('addROData');

  /* build the global data but exclude the hash4 strings */
  execSync(
    'mv build/ro/i_1hash4*.asm build/ && ' +
    'cat build/ro/i_1*.asm >> build/rodata.asm && ' +
    'echo "  SECTION code_compiler" >> build/rodata.asm && ' +
    'rm build/ro/i_1*.asm && ' +
    'mv build/i_1hash4*.asm build/ro/'
  );

  execSync('z88dk-z80asm -b build/rodata.asm');

  rodataSize = fs.statSync('build/rodata.bin').size;
  pageSize = /*16644*/ 16644 /*16383*/ - rodataSize; // should be 16384 - rodataSize but if we overfit the pages they squash down to within the limit due to the sharing of runtime code between functions which reduces the resultant output binary size
  console.log(pageSize);

  /* build the rodata located at the very top of ram */
  execSync(`z88dk-z80asm -b -m -r=${32768 - rodataSize} build/rodata.asm`);

  /* add the address of each rodata item as an assembly include file for anything that may need to reference it later */
  fs
    .readFileSync('build/rodata.map', 'utf8')
    .replace(/(i_1[a-zA-Z0-9]+)[^$]+\$([0-9a-fA-F]+)/g, (one, two, three, ...arr) => {
      const text = 'IFNDEF ' + two + '\n' +
            two + ' = $' + three + '\n' +
          'ENDIF\n';
      fs.writeFileSync(
          `build/ro/${two}.asm`,
          text,
          'utf8'
      );

      fs.appendFileSync('build/rodata2.asm', text);
    });

  /* de-duplicate the global variables */
  execSync(
    'sort build/globals.asm | uniq > build/globals2.asm && ' +
    'rm build/globals.asm && ' +
    'mv build/globals2.asm build/globals.asm'
  );

  var list = [];
  var walker = walk.walk('./build/s', {});

  walker.on('file', (root, fileStats, next) => {
    list.push(
      [fileStats.name, fs.readFileSync('build/s/' + fileStats.name, { encoding: 'utf8' })]
    );

    next();
  });

  walker.on('errors', (root, nodeStatsArray, next) => {
    next();
  });

  walker.on('end', () => {
    list.forEach(updateName);

    getFunctionSizes();
  });
}

function getFunctionSizes () {
  let name;
  console.log('getFunctionSizes');

  /* patch compareCodepoints into the functions that need it (so the table is
  always in the same page) */
  name = 'compareCommon';
  execSync(
    'cat build/s/compareCodepoints.asm >> build/s/getBytes.asm;' +
    "sed -i 's/_compareCodepoints/_" + name + "/g;s/querycsv/querycsv1/g;' build/s/getBytes.asm"
  );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 0, currentAddr, 0x0001, 'farcall']);
  currentAddr += 4;

  execSync('rm build/s/compareCodepoints.asm');

  name = 'sortCodepoints';
  execSync(
    'cat build/s/sortCodepoints.asm >> build/s/getBytes.asm;' +
    "sed -i 's/querycsv/querycsv1/g;' build/s/getBytes.asm"
  );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 0, currentAddr, 0x0001, 'farcall']);
  currentAddr += 4;

  name = 'sortCodesParse';
  execSync(
    'cat build/s/sortCodepoints.asm >> build/s/parse_mbcs.asm;' +
    "sed -i 's/_sortCodepoints/_" + name + "/g;' build/s/parse_mbcs.asm"
  );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 0, currentAddr, 0x0001, 'farcall']);
  currentAddr += 4;

  execSync('rm build/s/sortCodepoints.asm');

  name = 'sortBytesParse';
  execSync(
    'cat build/s/sortBytes.asm >> build/s/parse_mbcs.asm;' +
    "sed -i 's/_sortBytes/_" + name + "/g;s/querycsv/querycsv2/g;' build/s/parse_mbcs.asm"
  );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 0, currentAddr, 0x0001, 'farcall']);
  currentAddr += 4;

  name = 'sortBytes';
  execSync(
    'cat build/s/sortBytes.asm >> build/s/getCodepointsMbcs.asm;' +
    "sed -i 's/querycsv/querycsv3/g;' build/s/getCodepointsMbcs.asm"
  );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 0, currentAddr, 0x0001, 'farcall']);
  currentAddr += 4;

  execSync('rm build/s/sortBytes.asm');

  /* compile the data immediately above the function jump table */
  execSync('z88dk-z80asm -b -r=49152 build/data.asm');

  spawnSync(
    'sh',
    [
      '-c',
      "z88dk-z80asm -m -b -r=`ls -nl build/data.bin | awk '{print " + currentAddr + "}'` build/data.asm"
    ],
    {
      stdio: 'inherit'
    }
  );

  fs
    .readFileSync('build/data.map', 'utf8')
    .replace(/(^|\n)([_a-zA-Z0-9]+)[^$]+\$([0-9a-fA-F]+)/g, (one, blah, two, three, ...arr) => {
      defines[two] = three;
    });

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
        .map(elem => addDefines(elem, [elem], 'g'))
        .reduce((obj, elem) => {
          elem.children = elem.children
            .map(elem => elem.replace(/^_/, ''))
            .filter(name => fs.existsSync(path.join(__dirname, 'build', 'obj', name + '.bin')));

          try {
            elem.size = fs.statSync(path.join(__dirname, 'build', 'obj', elem.name + '.bin')).size;
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

  const pages = [[tree.main]];
  const remainingSizes = [pageSize];
  let currentPageData = {};
  let currentPageNumber = 0;
  const placedFunctions = [tree.main];
  let currentFunctions = [];

  // place the main function to begin with
  tree.main.pageNumber = 0;
  tree.main.children.forEach(elem => {
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
    //else {
    //  console.log(currentFunctions);
    //}

    // otherwise open a new page
    pages.push([]);
    remainingSizes.push(pageSize);
    currentPageData = {};
    currentPageNumber++;
  } while (1);

  compileLibC(pages);
}

function compilePages (pages) {
  console.log('compilePages');

  pages.forEach((elem, index) => {
    addDefines('page' + (index + 3), elem.map(elem2 => elem2.name), 'h', true);

    fs
      .readFileSync('build/obj2/page' + (index + 3) + '.map', 'utf8')
      .replace(/(^|\n)([_a-zA-Z0-9]+)[^$]+\$([0-9a-fA-F]+)/g, (one, blah, two, three, ...arr) => {
        // console.log(two,hashMap.hasOwnProperty(two), hashMap.hasOwnProperty(two.replace(/^_/, '')));
        two = two.replace(/^_/, '');
        const item = parseInt(three, 16);

        if (hasProp(hashMap, two) && item !== functionsList[hashMap[two]][2]) {
          functionsList[hashMap[two]][3] = item;
          functionsList[hashMap[two]][1] = index + 3;
        }
      });

    if (fs.statSync('build/obj2/page' + (index + 3) + '_code_compiler.bin').size > (16384 - rodataSize)) {
      console.log('page ' + (index + 3) + ' is too big');
      process.exit(-1);
    }

    execSync('dd if=/dev/zero bs=1 count=16384 of=build/obj2/qrycsv' + (('00' + (index + 3)).substr(-2)) + '.ovl');

    execSync('dd if=build/obj2/page' + (index + 3) + '_code_compiler.bin of=build/obj2/qrycsv' +
    (('00' + (index + 3)).substr(-2)) + '.ovl conv=notrunc');

    execSync('dd if=build/rodata.bin of=build/obj2/qrycsv' +
    (('00' + (index + 3)).substr(-2)) + '.ovl bs=1 seek=' + (16384 - rodataSize) + ' conv=notrunc');
  });

  execSync('rm build/obj2/*.bin');
}

function compileLibC (pages) {
  console.log('compileLibC');

  let foo = ''; let i, j;

  for (i = 0, j =
  15 // TODO: make this (numberOfPages+1) not be hard coded
  ; i < j; i++) {
    foo += 'defb 0b11111111 ; ' + (i + 1) + '\n';
  }

  // build the asm includes
  ['fcb', 'msx2'].forEach((name, index) => {
    execSync('cp libc.c pager.asm Makefile build/' + name + '/');

    fs.writeFileSync('build/' + name + '/defines.inc',
      Object.keys(defines).map(item => (/^_+([^_]+)?(_head|_tail|_size)$/).test(item)
        ? ''
        : '  PUBLIC ' + item + '\n  ' + item + ' equ 0x' + ('0000' + defines[item].toString(16)).substr(-4).toUpperCase()
      ).join('\n')
    );

    fs.writeFileSync('build/' + name + '/lookupTable.inc', functionsList.map(item =>
       (item[1] === 1 ? '  GLOBAL ' : '  ;') + item[0] + '\n' +
      '  defw 0x' + ('0000' + item[3].toString(16)).substr(-4).toUpperCase()
    ).join('\n'));

    fs.writeFileSync('build/' + name + '/functions.inc', functionsList.map(item =>
      (item[0] === 'main' ? '  PUBLIC _main\n_main:\n' : '') +
      '  call ' + item[4] + '  ;' + item[0] + '\n  defb ' + (item[1] === 1 ? index + 1 : item[1])
    ).join('\n'));

    fs.writeFileSync('build/' + name + '/pages.inc', foo);
  });

  compileLibC3(true);

  compilePages(pages);

  compileLibC3(false);
}

function compileLibC3(fixDefines) {
  ['fcb', 'msx2'].forEach((name, index) => {
    spawnSync(
      'make',
      ['qrycsv0' + (index + 1) + '.ovl'],
      {
        stdio: 'inherit',
        cwd: path.resolve(__dirname, 'build', name)
      });

    if(fixDefines) {
      /*
        Force the symbol named funcstart to be located at startOffset by
        adding some padding. The code below is a bit of a strange way to do this,
        but it's the only way that seems to work reliably whilst minimising
        the padding needed.
      */
      fs
        .readFileSync('build/' + name + '/qrycsv0' + (index + 1) + '.map', 'utf8')
        .replace(/(^|\n)([_a-zA-Z0-9]+)[^$]+\$([0-9a-fA-F]+)/g, (one, blah, two, three, ...arr) => {
          const four = two.replace(/^_/, '');

          if(four === 'funcstart') {
            execSync(`sed -i '1s/^/defs ${startOffset - parseInt(three, 16)}, 0\\n/' build/${name}/defines.inc`);
          }
        });
    }

    execSync('rm -f build/' + name + '/qrycsv0' + (index + 1) + '.ovl');

    spawnSync(
      'make',
      ['qrycsv0' + (index + 1) + '.ovl'],
      {
        stdio: 'inherit',
        cwd: path.resolve(__dirname, 'build', name)
      });

    fs
      .readFileSync('build/' + name + '/qrycsv0' + (index + 1) + '.map', 'utf8')
      .replace(/(^|\n)([_a-zA-Z0-9]+)[^$]+\$([0-9a-fA-F]+)/g, (one, blah, two, three, ...arr) => {
        const four = two.replace(/^_/, '');

        if(four === 'main') {
          /* do nothing */
        }
        else if(hasProp(hashMap, four)) {
          //console.log(four);
          functionsList[hashMap[four]][3] = parseInt(three, 16);
        }
        else if(libCDefines.includes(two) && !hasProp(defines, two)) {
          defines[two] = three;
        }
      });

    fs.writeFileSync('build/' + name + '/lookupTable.inc', functionsList.map(item =>
       (item[1] === 1 ? '  GLOBAL ':'  ;') + item[0] + '\n' +
      '  defw 0x' + ('0000' + item[3].toString(16)).substr(-4).toUpperCase()
    ).join('\n'));

    fs.writeFileSync('build/' + name + '/functions.inc', functionsList.map(item =>
      (item[0] === 'main' ? '  PUBLIC _main\n_main:\n' : '') +
      '  call ' + item[4] + '\n  defb ' + (item[1] === 1 ? index + 1 : item[1])
    ).join('\n'));

    execSync('rm -f build/' + name + '/qrycsv0' + (index + 1) + '.ovl');

    spawnSync(
      'make',
      ['qrycsv0' + (index + 1) + '.ovl'],
      {
        stdio: 'inherit',
        cwd: path.resolve(__dirname, 'build', name)
      });

    if(!fixDefines) {
      execSync('dd if=/dev/zero bs=1 count=16128 of=build/obj2/qrycsv0' + (index + 1) + '.ovl');

      execSync('dd if=build/' + name + '/qrycsv0' + (index + 1) + '.ovl of=build/obj2/qrycsv0' +
        + (index + 1) + '.ovl conv=notrunc');
    }

    //functionsList.sort((a, b) => (a[1] === b[1] ? 0 : (a[1] > b[1] ? -1 : 1)));
    //console.log(JSON.stringify(hashMap, null, 2));
    //console.log(JSON.stringify(functionsList, null, 2));
    //process.exit(0);
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

  const data = fs.createWriteStream('build/data.asm', {
    flags: 'a'
  });

  const globals = fs.createWriteStream('build/globals.asm', {
    flags: append ? 'a' : 'w'
  });

  if (!append) {
    writePause(
      data,
      '\tSECTION\tBSS\n'
    );
  }

  const rodataOutputStreams = [];

  const code = fs.createWriteStream('build/' + filename + '_code.asm');

  const functionOutputStreams = [];
  let activeStream = code;
  let rodataType = 0;

  lineReader.on('line', line => {
    let name;

    if (/SECTION/.test(line)) {
      name = line.replace(/^[ \t]*SECTION[ \t]*/, '');
      if (name === 'code_compiler') {
        rodataType = 0;

        if (functionOutputStreams.length) {
          activeStream = functionOutputStreams[functionOutputStreams.length - 1];
        } else {
          activeStream = code;
        }
      } else if (name === 'rodata_compiler') {
        rodataType = 1;
        activeStream = data;

        // don't output this line
      } else {
        rodataType = 2;

        activeStream = data;
      }
    } else if (/^;[ \t]+Function[ \t]+/.test(line)) {
      name = (line.replace(/^;[ \t]+Function[ \t]+/, '')).match(/[^ \t]+/)[0];
      if (hasProp(ignoreFunctions, name)) {
        functionOutputStreams.push(fs.createWriteStream('/dev/null', { flags: 'a' }));

        activeStream = functionOutputStreams[functionOutputStreams.length - 1];
      } else {
        functionOutputStreams.push(fs.createWriteStream('build/s/' + name + '.asm'));

        activeStream = functionOutputStreams[functionOutputStreams.length - 1];

        writePause(
          activeStream,
          '\tSECTION\t code_compiler\n' +
            line + '\n'
        );

        /* add an entry for each into the mapping table */
        if (
          name !== '_compareCodepoints' &&
          name !== '_sortCodepoints' &&
          name !== '_sortBytes' &&
          name !== 'main'
        ) { /* Filter out bsearch callbacks that
          need to be in the same page as the function that called bsearch. They don't need
          to be in the jump table. We want the main function to be this first item in the jump table to
          simplify debugging */
          hashMap[name] = functionsList.length;
          functionsList.push([name, 0, currentAddr, 0x0001, 'farcall']);
          currentAddr += 4;
        }
      }
    } else if (rodataType && /^\./.test(line)) {
      if (activeStream) {
        name = line.replace(/^\./, '');

        const name2 = name.replace(/[0-9]+$/, '');

        switch (name2) {
          case '_hash3EntryMap':
          case '_hash2_':
          case '_atariBytes':
          case '_zxBytes':
          case '_commonBytes':
          case '_cp1252Bytes':
          case '_petsciiBytes':
            /* add to the list of rodata regexes used to add the appropriate rodata to each function */
            rodataLabels.push([name, false, new RegExp('(\\b' + name.replace(matchOperatorsRe, '\\$&') + '\\b)', 'm')]);

            if (!hasProp(byteMaps, name2)) {
              rodataOutputStreams.push(fs.createWriteStream('build/ro/' + name2 + '.asm'));

              byteMaps[name2] = rodataOutputStreams.length - 1;
              activeStream = rodataOutputStreams[byteMaps[name2]];

              writePause(activeStream, 'IFNDEF ' + name + '\n');
            }

            activeStream = rodataOutputStreams[byteMaps[name2]];

            rodataType = 4;
            break;
        }

        if (rodataType === 1 || rodataType === 3) {
          rodataType = 3;

          rodataOutputStreams.push(fs.createWriteStream('build/ro/' + name + (name === 'i_1' ? '' : '') + '.asm'));

          /* add to the list of rodata regexes used to add the appropriate rodata to each function */
          rodataLabels.push([
            name,
            false,
            new RegExp('(\\b' + (name === 'i_70' ? 'i_70+' : name).replace(matchOperatorsRe, '\\$&') + '\\b)', 'm')
          ]);

          activeStream = rodataOutputStreams[rodataOutputStreams.length - 1];
          writePause(activeStream, 'IFNDEF ' + name + '\n');

          if(!/^i_1[a-zA-Z]+$/.test(name)) {
            writePause(activeStream, ';INCLUDE "../rodata2.asm"\n');
          }
        }

        writePause(activeStream, line + (name === 'i_1' ? '' : '') + '\n');
      }
    } else if (/^\tGLOBAL/.test(line)) {
      writePause(
        globals,
        line + '\n'
      );
    } else if (/\bi_1[^0-9]/.test(line)) {
      writePause(
        activeStream,
        line.replace(/\bi_1\b/, 'i_1' + filename) + '\n'
      );
    } else {
      writePause(
        activeStream,
        line + '\n'
      );
    }
  });

  lineReader.on('close', () => {
    j = functionOutputStreams.length + rodataOutputStreams.length + 2;

    data.end(writeFunctionPostfixes);
  });

  function writeFunctionPostfixes () {
    code.end(allStreamsClosed);
    globals.end(allStreamsClosed);

    for (let i = 0; i < rodataOutputStreams.length; i++) {
      /* close current stream */
      writePause(rodataOutputStreams[i], 'ENDIF\n');
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
function updateName (elem) {
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
}

function addDefines (filename, filenames, folderName, pageMode) {
  let arr = [];
  let notQuit = true;

  console.log('addDefines', filenames);

  execSync(
    'printf "\\\n" > ../' + folderName + '/' + filename + '.inc;' +
      'printf "\\\n" > ../' + folderName + '/' + filename + '2.inc;' +
      'printf "\\\n" > ../' + folderName + '/' + filename + '.asm;' +
      (pageMode ? 'printf "EXTERN extra\nEXTERN fa\nEXTERN fasign\n  SECTION code_compiler\n  org 0x4000\n" >> ../' + folderName + '/' + filename + '.asm;' : '') +
      'printf "  INCLUDE \\"z80_crt0.hdr\\"\n" >> ../' + folderName + '/' + filename + '.asm;' +
      filenames.reduce((obj, elem) => {
        obj += 'cat ' + elem + '.asm >> ../' + folderName + '/' + filename + '.asm;';
        return obj;
      }, '') +
      'printf " INCLUDE \\"../globals.asm\\"\n" >> ../' + folderName + '/' + filename + '.asm;' +
      'printf " INCLUDE \\"' + filename + '2.inc\\"\n" >> ../' + folderName + '/' + filename + '.asm;' +
      'printf " INCLUDE \\"' + filename + '.inc\\"\n" >> ../' + folderName + '/' + filename + '.asm',
    {
      cwd: path.join(__dirname, 'build', 's')
    }
  );

  if (pageMode) {
    execSync(
      'sed -i "s/;INCLUDE/INCLUDE/g;s/call\\t\\(minusfa\\|qsort_sccz80_callee\\|_logNum\\|ifix\\\\)/call \\1/g;s/jp\\texit/jp\\taexit/g;s/call\\t\\([^dl]\\)/call\\ta\\1/g;s/\\,_\\(get\\|outputResult\\|groupResultsInner\\)/\\,a_\\1/g" ../' + folderName + '/' + filename + '.asm',
      {
        cwd: path.join(__dirname, 'build', 's')
      }
    );
  }

  while (notQuit) {
    notQuit = false;

    try {
      execSync(
        'zcc +cpm ' + (folderName === 'h' ? '-m ' : '') + '--no-crt' +
          ' -O0 --c-code-in-asm -pragma-define:CRT_ORG_DATA=0 -lm -lndos -D__DISABLE_BUILTIN -U__STDC_VERSION__' +
          ' -o ../obj' + (pageMode ? '2' : '') + '/' + filename + '.bin ../' + folderName + '/' + filename + '.asm',
        {
          cwd: path.join(__dirname, 'build', 's'),
          stdio: 'pipe'
        }
      );
    } catch (e) {
      notQuit = true;

      /* create an array of all the missing symbol names */
      arr = Array.from(new Set(arr.concat(matchAll(e.stderr.toString() + e.stdout.toString(), /undefined symbol: ([^' \r\n]+)/g).toArray())));

      execSync(
        'rm ../' + folderName + '/' + filename + '2.inc;' +
          'rm ../' + folderName + '/' + filename + '.inc;' +
          'printf "\\\n" > ../' + folderName + '/' + filename + '.inc;' +
          'printf "\\\n" > ../' + folderName + '/' + filename + '2.inc;',
        {
          cwd: path.join(__dirname, 'build', 's')
        }
      );

      arr.forEach(elem => {
        if (!pageMode || hasProp({ a: 1, _: 1, i: 1 }, elem.charAt(0))) {
          const elem2 = elem.replace(/^(_|(a(_)?))/, '');

          execSync(
            'printf "  GLOBAL ' + elem + '\n" >> ../' + folderName + '/' + filename + '2.inc;' +
              (
                hasProp(hashMap, elem2)
                  ? 'printf "' + elem + ' equ 0x' + ((functionsList[hashMap[elem2]][2])
                    .toString(16).substr(-4).toUpperCase())
                  : (hasProp(defines, elem) ? 'printf "' + elem + ' equ 0x' + defines[elem] : (pageMode ? abort(elem) : 'printf ".' + elem))) +

                '\n" >> ../' + folderName + '/' + filename + '.inc',
            {
              cwd: path.join(__dirname, 'build', 's')
            }
          );
        } else {
          execSync(
            'printf "  EXTERN ' + elem + '\n" >> ../' + folderName + '/' + filename + '2.inc',
            {
              cwd: path.join(__dirname, 'build', 's')
            }
          );
        }
      });
    }
  }

  return {
    name: filename,
    children: arr
  };
}

function abort (elem) {
  console.log('symbol not found: ' + elem);
  process.exit(-1);
}
