/*
  we'll need to generate the loaders and c library pages 5 times

  esxdos 48k
  esxdos 128k
  residos 48k
  residos 128k
  plus3dos 128k

all other pages should be sharable
*/

const childProcess = require('child_process');
const spawnSync = childProcess.spawnSync;
const execSync = childProcess.execSync;
const fs = require('graceful-fs');
const readline = require('readline');
const walk = require('walk');
const shellEscape = require('shell-escape');
const matchAll = require('match-all');

const matchOperatorsRe = /[|\\{}()[\]^$+*?.]/g;

const hashMap = {};

let rodataSize = 0;

const functionsList = [
  ['abs', 3, 0x0001, 0x0001, 'farcall'],
  ['atol', 3, 0x0001, 0x0001, 'farcall'],
  ['_strtod', 3, 0x0001, 0x0001, 'farcall'],
  ['exit', 3, 0x0001, 0x0001, 'farcall'],
  ['mallinit', 3, 0x0001, 0x0001, 'farcall'],
  ['sbrk', 3, 0x0001, 0x0001, 'farcall'],
  ['malloc', 3, 0x0001, 0x0001, 'farcall'],
  ['free', 3, 0x0001, 0x0001, 'farcall'],
  ['realloc_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['calloc_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['strcmp_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['stricmp_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['strncmp_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['strlen', 3, 0x0001, 0x0001, 'farcall'],
  ['strstr_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['strcat_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['strncat_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['strnicmp_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['strncpy_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['memcpy_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['memmove_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['memset_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['fopen', 3, 0x0001, 0x0001, 'farcall'],
  ['fclose', 3, 0x0001, 0x0001, 'farcall'],
  ['fread', 3, 0x0001, 0x0001, 'farcall'],
  ['fwrite', 3, 0x0001, 0x0001, 'farcall'],
  ['fflush', 3, 0x0001, 0x0001, 'farcall'],
  ['fseek', 3, 0x0001, 0x0001, 'farcall'],
  ['fgetc', 3, 0x0001, 0x0001, 'farcall'],
  ['ungetc', 3, 0x0001, 0x0001, 'farcall'],
  ['fgets', 3, 0x0001, 0x0001, 'farcall'],
  ['fputc_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['fputs_callee', 3, 0x0001, 0x0001, 'farcall'],
  ['feof', 3, 0x0001, 0x0001, 'farcall'],
  ['fprintf', 3, 0x0001, 0x0001, 'farcall'],
  ['sprintf', 3, 0x0001, 0x0001, 'farcall'],
  ['vsnprintf', 3, 0x0001, 0x0001, 'farcall']
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

let currentAddr = 0xbd00 - 4;

/* contains a map of the addresses of all global variables */
const defines = {};

const rodataLabels = [];
const byteMaps = {};
let pageSize;

if(fs.existsSync('querycsv.c')) {
  /* The first action to initiate */
  start();
}

function start() {
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
      'cp stdio.asm build/data.asm'
    );

  /* generate the large static arrays in the format z88dk needs */
  execSync('node ../../generateMappings.js true');

  // update the jump table locations, starting at call_rom3 -4 and working downward in memory
  functionsList.forEach((item, index) => {
    hashMap[item[0].replace(/^_/, '')] = index;
    item[2] = currentAddr;
    currentAddr -= 4;
  });

  compileLexer();
}

// compile and split up lexer
function compileLexer() {
  console.log('compileLexer');

  execSync(
      'sed "' +
        's/struct yy_trans_info/' +
          'flex_int8_t yy_accept2(unsigned int offset);' +
          'flex_uint16_t yy_nxt2(unsigned int offset);' +
          'flex_int16_t yy_chk2(unsigned int offset);' +
          'YY_CHAR yy_ec2(unsigned int offset);' +
          'struct yy_trans_info/g;' +
        's/flex_int32_t yy_rule_can_match_eol/' +
          'flex_int8_t yy_rule_can_match_eol/g;' +
        's/flex_int16_t yy_accept/' +
          'flex_int8_t yy_accept/g;' +
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
      'zcc +zx -Cc-base=6 -U__STDC_VERSION__ lexer2.c -E -o build/lexer3.c && ' +
      '../../makeheaders -h build/lexer3.c | grep -v __LIB__ | grep -v extern | grep -v \\#define | sort | uniq > build/lexer3.h'
    );

  execSync(
      'zcc +zx -Cc-base=6 -U__STDC_VERSION__ build/lexer3.c -S -o build/lexer.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0lexer/g" build/lexer.asm'
    );

  splitUpFunctions('lexer', compileParser);
}

function compileParser() {
  console.log('compileParser');

  execSync(
      'sed "' +
        's/static const char \\*const yytname\\[\\] =/' +
          'static const char ** yytname =/g;' +
        's/YY_INITIAL_VALUE (static YYSTYPE yyval_default;)/' +
          '/g;' +
        's/char const/' +
          'char/g;' +
        's/YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);/' +
          'static YYSTYPE yylval;/g;' +
        's/yyval = yyvsp\\[1-yylen\\];/' +
          '/g;' +
        's/static const yytype_int16 yypact\\[\\]/'+
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
      'zcc +zx -Cc-base=6 -U__STDC_VERSION__ sql2.c -E -o build/sql3.c && ' +
      '../../makeheaders -h build/sql3.c | grep -v __LIB__ | grep -v extern | grep -v \\#define | sort | uniq > build/sql3.h'
    );

  execSync(
      'zcc +zx -Cc-base=6 -U__STDC_VERSION__ sql2.c -S -o build/sql.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0sql/g" build/sql.asm'
    );

  splitUpFunctions('sql', compileQueryCSV, true);
}

function compileQueryCSV() {
  console.log('compileQueryCSV');

  execSync(
      'zcc +zx -Cc-base=6 -U__STDC_VERSION__ querycsv.c -S -o build/querycsv.asm;' +
      'sed -i -E "s/jr/jp/gi;s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0querycsv/g" build/querycsv.asm'
    );

  splitUpFunctions('querycsv', compileHash2, true);
}

function compileHash2() {
  console.log('compileHash2');

  execSync(
      'zcc +zx -Cc-base=6 -U__STDC_VERSION__ hash2dat.c -S -o build/hash2.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0hash2/g" build/hash2.asm'
    );

  splitUpFunctions('hash2', compileHash3, true);
}

function compileHash3() {
  console.log('compileHash3');

  execSync(
      'zcc +zx -Cc-base=6 -U__STDC_VERSION__ hash3.c -S -o build/hash3.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0ghash3/g" build/hash3.asm'
    );

  splitUpFunctions('hash3', compileHash4a, true);
}

function compileHash4a() {
  console.log('compileHash4a');

  execSync(
      'sed "' +
        '1s/^/extern struct hash4Entry hash4export;\\n/;' +
        's/static struct hash4Entry/'+
          'static const struct hash4Entry/gi;' +
        's/static unsigned short/'+
          'static const unsigned short/gi;' +
        's/if (\\*str == \\*s \&\& \!strncmp (str + 1, s + 1, len - 1) \&\& s\\[len\\]/'+
          'while(len \\&\\& *str \\&\\& (*str == *s)) { ++str; ++s; --len; } if(len == 0 \\&\\& *s/gi;' +
        's/return \\&wordlist\\[key\\];/'+
          '{' +
            'hash4export.script = wordlist[key].script;' +
            'hash4export.index = wordlist[key].index;' +
            'hash4export.isNotLower = wordlist[key].isNotLower;'+
            'return \\&hash4export;' +
          '}/gi;' +
      '" hash4a.c > build/hash4a.c'
    );

  execSync(
      'zcc +zx -Cc-base=6 -U__STDC_VERSION__ build/hash4a.c -S -o build/hash4a.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0hash4a/g" build/hash4a.asm'
    );

  splitUpFunctions('hash4a', compileHash4b, true);
}

function compileHash4b() {
  console.log('compileHash4b');

  execSync(
      'sed "' +
        '1s/^/extern struct hash4Entry hash4export;\\n/;' +
        's/static struct hash4Entry/'+
          'static const struct hash4Entry/gi;' +
        's/static unsigned short/'+
          'static const unsigned short/gi;' +
        's/if (\\*str == \\*s \&\& \!strncmp (str + 1, s + 1, len - 1) \&\& s\\[len\\]/'+
          'while(len \\&\\& *str \\&\\& (*str == *s)) { ++str; ++s; --len; } if(len == 0 \\&\\& *s/gi;' +
        's/return \\&wordlist\\[key\\];/'+
          '{' +
            'hash4export.script = wordlist[key].script;' +
            'hash4export.index = wordlist[key].index;' +
            'hash4export.isNotLower = wordlist[key].isNotLower;' +
            'return \\&hash4export;' +
          '}/gi;' +
      '" hash4b.c > build/hash4b.c'
    );

  execSync(
      'zcc +zx -Cc-base=6 -U__STDC_VERSION__ build/hash4b.c -S -o build/hash4b.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0hash4b/g" build/hash4b.asm'
    );

  splitUpFunctions('hash4b', compileHash4c, true);
}

function compileHash4c() {
  console.log('compileHash4c');

  execSync(
      'sed "' +
        '1s/^/extern struct hash4Entry hash4export;\\n/;' +
        's/static struct hash4Entry/'+
          'static const struct hash4Entry/gi;' +
        's/static unsigned short/'+
          'static const unsigned short/gi;' +
        's/if (\\*str == \\*s \&\& \!strncmp (str + 1, s + 1, len - 1) \&\& s\\[len\\]/'+
          'while(len \\&\\& *str \\&\\& (*str == *s)) { ++str; ++s; --len; } if(len == 0 \\&\\& *s/gi;' +
        's/return \\&wordlist\\[key\\];/'+
          '{' +
            'hash4export.script = wordlist[key].script;' +
            'hash4export.index = wordlist[key].index;' +
            'hash4export.isNotLower = wordlist[key].isNotLower;' +
            'return \\&hash4export;' +
          '}/gi;' +
      '" hash4c.c > build/hash4c.c'
    );

  execSync(
      'zcc +zx -Cc-base=6 -U__STDC_VERSION__ build/hash4c.c -S -o build/hash4c.asm;' +
      'sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0hash4c/g" build/hash4c.asm'
    );

  splitUpFunctions('hash4c', addROData, true);
}

/*
  Compile the read only data to a section of memory at the top of the z80 address space.
  This will be appended to each memory page so will effectively be always paged in */
function addROData() {
  console.log('addROData');

  /* build the global data but exclude the hash4 strings */
  execSync(
      'mv build/ro/i_1hash4*.asm build/ && '+
      'cat build/ro/i_1*.asm >> build/rodata.asm && ' +
      'echo "  SECTION code_compiler" >> build/rodata.asm && ' +
      'rm build/ro/i_1*.asm && ' +
      'mv build/i_1hash4*.asm build/ro/'
    );

  execSync('z80asm -b build/rodata.asm');

  rodataSize = fs.statSync('build/rodata.bin').size;
  pageSize = 16644  - rodataSize; //should be 16384 - rodataSize but if we overfit the pages they squash down to within the limit due to the sharing of runtime code between functions reducing the resultant output binary size

  /* build the rodata located at the very top of ram */
  execSync(`z80asm -b -m -r=${65535 - rodataSize} build/rodata.asm`);

  /* add the address of each rodata item as an assembly include file for anything that may need to reference it later */
  fs.
    readFileSync('build/rodata.map', 'utf8').
    replace(/(i_1[a-zA-Z0-9]+)[^$]+\$([0-9a-fA-F]+)/g, (one, two, three, ...arr) => {
      fs.writeFileSync(
          `build/ro/${two}.asm`,
          'IFNDEF ' + two +'\n' +
            two + ' = $' + three + '\n'+
          'ENDIF\n',
          'utf8'
        );
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
        [ fileStats.name, fs.readFileSync('build/s/' + fileStats.name, { encoding: 'utf8' }) ]
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

function getFunctionSizes() {
  let name;
  console.log('getFunctionSizes');

  /* patch compareCodepoints into the functions that need it (so the table is
  always in the same page) */
  name = 'compareCP1252';
  execSync(
      'cat build/s/compareCodepoints.asm >> build/s/getBytesCP1252.asm;' +
      "sed -i 's/_compareCodepoints/_"+name+"/g;s/querycsv/querycsv1/g;' build/s/getBytesCP1252.asm"
    );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 0, currentAddr, 0x0001, 'farcall2']);
  currentAddr -=4;

  name = 'compareCommon';
  execSync(
      'cat build/s/compareCodepoints.asm >> build/s/getBytesCommon.asm;' +
      "sed -i 's/_compareCodepoints/_"+name+"/g;s/querycsv/querycsv2/g;' build/s/getBytesCommon.asm"
    );

  //replace compareCodepoints with compareCommon
  hashMap[name] = hashMap['compareCodepoints'];
  delete hashMap['compareCodepoints'];
  functionsList[hashMap[name]][0] = name;

  name = 'comparePetscii';
  execSync(
      'cat build/s/compareCodepoints.asm >> build/s/getBytesPetscii.asm;' +
      "sed -i 's/_compareCodepoints/_"+name+"/g;s/querycsv/querycsv3/g;' build/s/getBytesPetscii.asm"
    );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 0, currentAddr,  0x0001,'farcall2']);
  currentAddr -=4;

  name = 'compareAtariST';
  execSync(
      'cat build/s/compareCodepoints.asm >> build/s/getBytesAtariST.asm;' +
      "sed -i 's/_compareCodepoints/_"+name+"/g;s/querycsv/querycsv4/g;' build/s/getBytesAtariST.asm"
    );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 0, currentAddr, 0x0001, 'farcall2']);
  currentAddr -=4;

  name = 'compareZX';
  execSync(
      'cat build/s/compareCodepoints.asm >> build/s/getBytesZXCommon.asm;' +
      "sed -i 's/_compareCodepoints/_"+name+"/g;s/querycsv/querycsv5/g;' build/s/getBytesZXCommon.asm"
    );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 0, currentAddr, 0x0001, 'farcall2']);
  currentAddr -=4;

  name = 'compareCP1047';
  execSync(
      'cat build/s/compareCodepoints.asm >> build/s/getBytesCP1047.asm;' +
      "sed -i 's/_compareCodepoints/_"+name+"/g;s/querycsv/querycsv6/g;' build/s/getBytesCP1047.asm"
    );

  hashMap[name] = functionsList.length;
  functionsList.push([name, 0, currentAddr,  0x0001,'farcall2']);
  currentAddr -=4;

  execSync('rm build/s/compareCodepoints.asm');

  /*compile the data immediately above the function jump table */
  execSync('z80asm -b -r=49152 build/data.asm');

  spawnSync(
    'sh',
    [
      '-c',
      "z80asm -m -b -r=`ls -nl build/data.bin | awk '{print " + (currentAddr + 3) + " - $5}'` build/data.asm"
    ],
    {
      stdio: 'inherit'
    }
  );

  fs.
    readFileSync('build/data.map', 'utf8').
    replace(/(^|\n)([_a-zA-Z0-9]+)[^$]+\$([0-9a-fA-F]+)/g, (one, blah, two, three, ...arr) => {
      defines[two] = three;
  });

  const list = [];

  const walker = walk.walk('./build/s', {});

  walker.on('file', (root, fileStats, next) => {
    if(/\.asm$/.test(fileStats.name)) {
      list.push(fileStats.name.replace(/\.asm$/, '').replace(/^_/, ''));
    }

    next();
  });

  walker.on('errors', (root, nodeStatsArray, next) => {
    next();
  });

  walker.on('end', () => {
    packPages(
      list.
        map(elem => addDefines(elem, [elem], 'g')).
        reduce((obj, elem) => {
          elem.children = elem.children.
            map(elem => elem.replace(/^_/, '')).
            filter(name => fs.existsSync(__dirname + '/build/obj/' + name + '.bin'));

          try {
            elem.size = fs.statSync(__dirname + '/build/obj/' + elem.name + '.bin').size;
          }
          catch(e) {}

          obj[elem.name] = elem;

          return obj;
        }, {})
    );
  });
}

function packPages(tree) {
  /* use a bin packing algorithm to group the functions close
  to their call-stack parents and produce a binary of each 16k page */

  const pages = [[tree['main']]];
  const remainingSizes = [pageSize];
  let currentPageData = {};
  let currentPageNumber = 0;
  const placedFunctions = [tree['main']];
  let currentFunctions = [];

  // place the main function to begin with
  tree['main'].pageNumber = 0;
  tree['main'].children.forEach(elem => {
    currentPageData[elem] = true;
  });

  do {
    // temporarily re-add functions that've already been placed so we can add their children
    currentFunctions = [].concat(placedFunctions);

    for(let loop = 3; loop; loop--) { // try to keep functions fairly close to
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
      if(children.length === 0) {
        break;
      }

      currentFunctions = currentFunctions.concat(children);

      // sort by size and filter the functions that've already been allocated
      currentFunctions = currentFunctions
        .sort((a, b) => {
          // prioritize functions whose parent function is already in the current page
          const temp = currentPageData.hasOwnProperty(a.name) - currentPageData.hasOwnProperty(b.name);

          if(!temp) {
            return temp;
          }

          return a.size - b.size;
        })
        .reduce((obj, elem) => {
          if(!(elem.hasOwnProperty('pageNumber') || obj.seenNames.hasOwnProperty(elem.name))) {
            obj.seenNames[elem.name] = true;
            obj.newArr.push(elem);
          }

          return obj;
        }, { seenNames: {}, newArr: [] }).newArr;

      // place the biggest first. if all of then add the children. if none of the children can
      for(let i = 0; i < currentFunctions.length; i++) {
        for(let k = currentPageNumber, m = 3; m > 0; k--, m--) {
          if(remainingSizes[k] - currentFunctions[i].size > 0) {
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
      .filter(elem => !(elem.hasOwnProperty('pageNumber')));

    // if all functions have been placed then quit
    if(currentFunctions.length === 0) {
      break;
    }

    // otherwise open a new page
    pages.push([]);
    remainingSizes.push(pageSize);
    currentPageData = {};
    currentPageNumber++;
  } while(1);

  compilePages(pages);
}

function compilePages(pages) {
  pages.forEach((elem, index) => {
    addDefines('page' + (index + 6), elem.map(elem2 => elem2.name), 'h', true);

    fs.
      readFileSync('build/obj2/page' + (index + 6) + '.map', 'utf8').
      replace(/(^|\n)([_a-zA-Z0-9]+)[^$]+\$([0-9a-fA-F]+)/g, (one, blah, two, three, ...arr) => {
        //console.log(two,hashMap.hasOwnProperty(two), hashMap.hasOwnProperty(two.replace(/^_/, '')));
        two = two.replace(/^_/, '');
        const item = parseInt(three, 16);
        if(hashMap.hasOwnProperty(two) && item !== functionsList[hashMap[two]][2]) {
          functionsList[hashMap[two]][3] = item;
          functionsList[hashMap[two]][1] = index + 6;
        }
      });

    if(fs.statSync('build/obj2/page'+(index+6)+'_code_compiler.bin').size > (16384 - rodataSize)) {
      console.log('page '+ (index+6) + ' is too big');
      process.exit(-1);
    }

    execSync('dd if=/dev/zero bs=1 count=16384 of=build/obj2/qcsv' + (('00' + (index+6)).substr(-2)) + 'zx.ovl');

    execSync('dd if=build/obj2/page'+(index+6)+'_code_compiler.bin of=build/obj2/qcsv' + (('00' + (index+6)).substr(-2)) + 'zx.ovl conv=notrunc');

    execSync('dd if=build/rodata.bin of=build/obj2/qcsv' + (('00' + (index+6)).substr(-2)) + 'zx.ovl bs=1 seek='+(16384  - rodataSize)+' conv=notrunc');
  });

  execSync('rm build/obj2/*.bin');

  functionsList.sort((a,b) => (a[1] === b[1] ? 0 : (a[1] > b[1] ? -1 : 1)));

  //end of the runtime of this build-zx.js file. Log what we did for now
  //console.log(JSON.stringify(hashMap, null, 2));
  //console.log(JSON.stringify(functionsList, null, 2));

  compileLibC();
}

function compileLibC() {
  console.log('compileLibC');

  let foo = '', i;

  for(i = 0; i < parseInt(functionsList[0][1], 10); i++) {
    foo += 'defb 0b11111111 ; ' + (i+1) + '\n';
  }

  //build the asm includes
  ['plus3dos', 'residos48', 'residos128', 'esxdos48', 'esxdos128'].forEach((name, index) => {
    fs.writeFileSync(name + '/lookupTable.inc', functionsList.map(item =>
      item[1] === 3 ? '  EXTERN ' + item[0] + '\n  defw ' + item[0] : '  defw 0x' + ('0000' + item[3].toString(16)).substr(-4).toUpperCase()
    ).join('\n'));

    fs.writeFileSync(name + '/functions.inc', functionsList.map(item =>
      '  call ' + item[4] + '\n  defb ' + (item[1] == 3 ? index + 1 : item[1])
    ).join('\n'));

    fs.writeFileSync(name + '/pages.inc', foo);
  });

  process.exit(0);

  //plus3dos
  execSync('zcc +zx --no-crt -lm -lp3 -pragma-define:CRT_ORG_CODE=0xc000 '+
  '-pragma-define:CRT_ORG_DATA=0 -pragma-define:CRT_ORG_BSS=0x8000 '+
  '-pragma-define:CLIB_MALLOC_HEAP_SIZE=0 -pragma-define:CLIB_FOPEN_MAX=10 '+
  '-pragma-define:CRT_ON_EXIT=0x10002 -pragma-define:CRT_ENABLE_COMMANDLINE=2 '+
  '-pragma-redirect:fputc_cons=myfputc_cons -U__STDC_VERSION__ plus3dos/plus3dos.asm libc.c '+
  '-m -o qcsv01zx.ovl');

  //residos 48k
  execSync('zcc +zx --no-crt -lm -DRESIDOS -lp3 -pragma-define:CRT_ORG_CODE=0xc000 '+
  '-pragma-define:CRT_ORG_DATA=0 -pragma-define:CRT_ORG_BSS=0x8000 '+
  '-pragma-define:CLIB_MALLOC_HEAP_SIZE=0 -pragma-define:CLIB_FOPEN_MAX=10 '+
  '-pragma-define:CRT_ON_EXIT=0x10002 -pragma-define:CRT_ENABLE_COMMANDLINE=2 '+
  '-pragma-redirect:fputc_cons=fputc_cons_rom_rst -U__STDC_VERSION__ residos48/residos48.asm libc.c '+
  '-m -o qcsv02zx.ovl');

  //residos 128k
  execSync('zcc +zx --no-crt -lm -DRESIDOS -lp3 -pragma-define:CRT_ORG_CODE=0xc000 '+
  '-pragma-define:CRT_ORG_DATA=0 -pragma-define:CRT_ORG_BSS=0x8000 '+
  '-pragma-define:CLIB_MALLOC_HEAP_SIZE=0 -pragma-define:CLIB_FOPEN_MAX=10 '+
  '-pragma-define:CRT_ON_EXIT=0x10002 -pragma-define:CRT_ENABLE_COMMANDLINE=2 '+
  '-pragma-redirect:fputc_cons=myfputc_cons -U__STDC_VERSION__ residos128/residos128.asm libc.c '+
  '-m -o qcsv03zx.ovl');

  //esxdos 48k
  execSync('zcc +zx --no-crt -lm -lesxdos -pragma-define:CRT_ORG_CODE=0xc000 '+
  '-pragma-define:CRT_ORG_DATA=0 -pragma-define:CRT_ORG_BSS=0x8000 '+
  '-pragma-define:CLIB_MALLOC_HEAP_SIZE=0 -pragma-define:CLIB_FOPEN_MAX=10 '+
  '-pragma-define:CRT_ON_EXIT=0x10002 -pragma-define:CRT_ENABLE_COMMANDLINE=2 '+
  '-pragma-redirect:fputc_cons=fputc_cons_rom_rst -U__STDC_VERSION__ esxdos48/esxdos48.asm libc.c '+
  '-m -o qcsv04zx.ovl');

  //esxdos 128k
  execSync('zcc +zx --no-crt -lm -lesxdos -pragma-define:CRT_ORG_CODE=0xc000 '+
  '-pragma-define:CRT_ORG_DATA=0 -pragma-define:CRT_ORG_BSS=0x8000 '+
  '-pragma-define:CLIB_MALLOC_HEAP_SIZE=0 -pragma-define:CLIB_FOPEN_MAX=10 '+
  '-pragma-define:CRT_ON_EXIT=0x10002 -pragma-define:CRT_ENABLE_COMMANDLINE=2 '+
  '-pragma-redirect:fputc_cons=myfputc_cons -U__STDC_VERSION__ esxdos128/esxdos128.asm libc.c '+
  '-m -o qcsv05zx.ovl');
}


/* *** HELPER FUNCTIONS AFTER THIS POINT *** */

/*
  Split the specified assembly file into one file for each function's code
  (including the necessary rodata) and append to a global data segment.
  Pause if node.js can't keep up
*/
function splitUpFunctions(filename, callback, append) {
  console.log('splitUpFunctions');

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

  const rodataOutputStreams = [];

  const code = fs.createWriteStream('build/' + filename + '_code.asm');

  const functionOutputStreams = [];
  let activeStream = code;
  let rodataType = 0;

  lineReader.on('line', line => {
    let name;

    if(/SECTION/.test(line)) {
      name = line.replace(/^[ \t]*SECTION[ \t]*/, '');
      if(name === 'code_compiler') {
        rodataType = 0;

        if(functionOutputStreams.length) {
          activeStream = functionOutputStreams[functionOutputStreams.length - 1];
        }
        else {
          activeStream = code;
        }
      }
      else if(name === 'rodata_compiler') {
        rodataType = 1;
        activeStream = data;

        // don't output this line
      }
      else {
        rodataType = 2;

        activeStream = data;
      }
    }
    else if(/^;[ \t]+Function[ \t]+/.test(line)) {
      name = (line.replace(/^;[ \t]+Function[ \t]+/, '')).match(/[^ \t]+/)[0];
      if(ignoreFunctions.hasOwnProperty(name)) {
        functionOutputStreams.push(fs.createWriteStream('/dev/null', { flags: 'a' }));

        activeStream = functionOutputStreams[functionOutputStreams.length - 1];
      }
      else {
        functionOutputStreams.push(fs.createWriteStream('build/s/' + name + '.asm'));

        activeStream = functionOutputStreams[functionOutputStreams.length - 1];

        writePause(
          activeStream,
          '\tSECTION\t code_compiler\n' +
            line + '\n'
        );

        /* add an entry for each into the mapping table */
        if(name !== '_compareCodepoints') { /* compareCodepoints is a bsearch callback that
          needs to be in the same page as the function that called bsearch. it doesn't need
          to be in the jump table */
          hashMap[name] = functionsList.length;
          functionsList.push([name, 0, currentAddr, 0, 'farcall2']);
          currentAddr -=4;
        }
      }
    }
    else if(rodataType && /^\./.test(line)) {
      if(activeStream) {
        name = line.replace(/^\./, '');

        const name2 = name.replace(/[0-9]+$/, '');

        switch(name2) {
          case '_hash3EntryMap':
          case '_hash2_':
          case '_atariBytes':
          case '_zxBytes':
          case '_commonBytes':
          case '_cp1252Bytes':
          case '_petsciiBytes': {
            /* add to the list of rodata regexes used to add the appropriate rodata to each function */
            rodataLabels.push([name, false, new RegExp('(\\b' + name.replace(matchOperatorsRe, '\\$&') + '\\b)', 'm')]);

            if(!byteMaps.hasOwnProperty(name2)) {
              rodataOutputStreams.push(fs.createWriteStream('build/ro/' + name2 + '.asm'));

              byteMaps[name2] = rodataOutputStreams.length - 1;
              activeStream = rodataOutputStreams[byteMaps[name2]];

              writePause(activeStream, 'IFNDEF ' + name + '\n');
            }

            activeStream = rodataOutputStreams[byteMaps[name2]];

            rodataType = 4;
          } break;
        }

        if(rodataType === 1 || rodataType === 3) {
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
        }

        writePause(activeStream, line + (name === 'i_1' ? '' : '') + '\n');
      }
    }
    else if(/^\tGLOBAL/.test(line)) {
      writePause(
        globals,
        line + '\n'
      );
    }
    else if(/\bi_1[^0-9]/.test(line)) {
      writePause(
        activeStream,
        line.replace(/\bi_1\b/, 'i_1' + filename) + '\n'
      );
    }
    else {
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

  function writeFunctionPostfixes() {
    code.end(allStreamsClosed);
    globals.end(allStreamsClosed);

    for(let i = 0; i < rodataOutputStreams.length; i++) {
      /* close current stream */
      writePause(rodataOutputStreams[i], 'ENDIF\n');
      rodataOutputStreams[i].end(allStreamsClosed);
    }

    for(let i = 0; i < functionOutputStreams.length; i++) {
      /* close current stream */
      functionOutputStreams[i].end(allStreamsClosed);
    }
  }

  /* when all streams have been closed, go to the next step (compiling the
  data segment) */
  function allStreamsClosed() {
    if(--j === 0) {
      callback();
    }
  }

  function writePause(stream, text) {
    if(!stream.write(text)) {
      lineReader.pause();

      stream.on('drain', () => {
        lineReader.resume();
      });
    }
  }
}

/* updateName adds to rodata values needed by each function to the end of
its assembly source file */
function updateName(elem) {
  const text = elem[1];
  const name = elem[0].replace(/\.asm$/, '');

  let hasMatches = false;

  rodataLabels.forEach(element => {
    if(element[1] = element[1] || element[2].test(text)) {
      hasMatches = true;

      if(element[0] === '_st_in_word_set_a_wordlist') {
        rodataLabels.find(element => element[0] === 'i_1hash4a')[1] = true;
      }
      else if(element[0] === '_st_in_word_set_b_wordlist') {
        rodataLabels.find(element => element[0] === 'i_1hash4b')[1] = true;
      }
      else if(element[0] === '_st_in_word_set_c_wordlist') {
        rodataLabels.find(element => element[0] === 'i_1hash4c')[1] = true;
      }
    }
  });

  if(hasMatches) {
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

function addDefines(filename, filenames, folderName, pageMode) {
  let arr = [];
  let notQuit = true;

  console.log('addDefines', filenames);

  execSync(
      'printf "\\\n" > ../' + folderName + '/' + filename + '.inc;' +
      'printf "\\\n" > ../' + folderName + '/' + filename + '2.inc;' +
      'printf "\\\n" > ../' + folderName + '/' + filename + '.asm;' +
      (pageMode ? 'printf "  SECTION bss_error\n  org 0x' + defines['_myerrno'] + '\n  SECTION code_compiler\n  org 0xc000\n" >> ../' + folderName + '/' + filename + '.asm;' : '') +
      'printf "  INCLUDE \\"z80_crt0.hdr\\"\n" >> ../' + folderName + '/' + filename + '.asm;' +
      filenames.reduce((obj, elem) => {
        obj += 'cat ' + elem + '.asm >> ../' + folderName + '/' + filename + '.asm;';
        return obj;
      }, '') +
      'printf " INCLUDE \\"../globals.asm\\"\n" >> ../' + folderName + '/' + filename + '.asm;' +
      'printf " INCLUDE \\"' + filename + '2.inc\\"\n" >> ../' + folderName + '/' + filename + '.asm;' +
      'printf " INCLUDE \\"' + filename + '.inc\\"\n" >> ../' + folderName + '/' + filename + '.asm',
      {
        cwd: __dirname + '/build/s'
      }
    );

  if(pageMode) {
    execSync(
      'sed -i "s/call\\tminusfa/call minusfa/g;s/jp\\texit/jp\\taexit/g;s/call\\t\\([^dl]\\)/call\\ta\\1/g;s/\\,_\\(get\\|outputResult\\|groupResultsInner\\)/\\,a_\\1/g" ../' + folderName + '/' + filename + '.asm',
      {
        cwd: __dirname + '/build/s'
      }
    );
  }

  while(notQuit) {
    notQuit = false;

    try {
      execSync(
          'zcc +zx ' + (folderName === 'h' ? '-m ' : '') + '--no-crt' +
          ' -pragma-define:CRT_ORG_DATA=0 -lm -lndos -U__STDC_VERSION__' +
          ' -o ../obj' + (pageMode ? '2' : '') + '/' + filename + '.bin ../' + folderName + '/' + filename + '.asm',
          {
            cwd: __dirname + '/build/s'
          }
        );
    }
    catch(e) {
      notQuit = true;

      /* create an array of all the missing symbol names */
      arr = Array.from(new Set(arr.concat(matchAll(e.stderr.toString() + e.stdout.toString(), /symbol '([^']+)/g).toArray())));

      execSync(
          'rm ../' + folderName + '/' + filename + '2.inc;' +
          'rm ../' + folderName + '/' + filename + '.inc;' +
          'printf "\\\n" > ../' + folderName + '/' + filename + '.inc;' +
          'printf "\\\n" > ../' + folderName + '/' + filename + '2.inc;',
          {
            cwd: __dirname + '/build/s'
          }
        );

      arr.forEach(elem => {
        if(!pageMode || ({ 'a': 1, '_': 1, 'i': 1 }).hasOwnProperty(elem.charAt(0))) {
          const elem2 = elem.replace(/^(_|(a(_)?))/, '');

          execSync(
              'printf "  GLOBAL ' + elem + '\n" >> ../' + folderName + '/' + filename + '2.inc;' +
              (
                hashMap.hasOwnProperty(elem2)
                  ? 'printf "' + elem + ' equ 0x' + ((functionsList[hashMap[elem2]][2])
                    .toString(16).substr(-4).toUpperCase())
                  : (defines.hasOwnProperty(elem) ? 'printf "' + elem + ' equ 0x' + defines[elem] : (pageMode?abort(elem):'printf ".' + elem))) +

                '\n" >> ../' + folderName + '/' + filename + '.inc',
              {
                cwd: __dirname + '/build/s'
              }
            );
        }
        else {
          execSync(
              'printf "  EXTERN ' + elem + '\n" >> ../' + folderName + '/' + filename + '2.inc',
              {
                cwd: __dirname + '/build/s'
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

function abort(elem) {
  console.log("symbol not found: "+ elem);
  process.exit(-1);
}
