/*
wasm -0 pager.asm
wcl -ms -0 -os -fpc -s -fm=qrycsv16 -fe=qrycsv16 pager.obj libc.c

wcl -mt -0 -os -s -fm dosload.c
hexdump -v -e '8/1 "0x%02X, "' -e '"\n"' ./dosload.com | sed -e 's/^/db /g;s/, 0x  //g;s/, /,/g;s/,$//g' >dosload.inc
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
  ['realmain_', 3, 0x0001, 0x4000, 'farcall'],
  ['exit_', 1, 0x0001, 0x0001, 'farcall'],
  ['strcmp_', 1, 0x0001, 0x0001, 'farcall'],
  ['stricmp_', 1, 0x0001, 0x0001, 'farcall'],
  ['strlen_', 1, 0x0001, 0x0001, 'farcall'],
  ['strstr_', 1, 0x0001, 0x0001, 'farcall'],
  ['strcat_', 1, 0x0001, 0x0001, 'farcall'],
  ['strncat_', 1, 0x0001, 0x0001, 'farcall'],
  ['strnicmp_', 1, 0x0001, 0x0001, 'farcall'],
  ['strcpy_', 1, 0x0001, 0x0001, 'farcall'],
  ['strncpy_', 1, 0x0001, 0x0001, 'farcall'],
  ['memcpy_', 1, 0x0001, 0x0001, 'farcall'],
  ['memmove_', 1, 0x0001, 0x0001, 'farcall'],
  ['memset_', 1, 0x0001, 0x0001, 'farcall'],
  ['fopen_dos_', 1, 0x0001, 0x0001, 'farcall'],
  ['fclose_', 1, 0x0001, 0x0001, 'farcall'],
  ['fread_', 1, 0x0001, 0x0001, 'farcall'],
  ['fwrite_dos_', 1, 0x0001, 0x0001, 'farcall'],
  ['macYield_', 1, 0x0001, 0x0001, 'farcall'],
//  ['_logNum', 1, 0x0001, 0x0001, 'farcall'],
  ['fprintf_dos_', 1, 0x0001, 0x0001, 'farcall'],
  ['fputs_dos_', 1, 0x0001, 0x0001, 'farcall'],
  ['getenv_', 1, 0x0001, 0x0001, 'farcall'],
  ['putenv_', 1, 0x0001, 0x0001, 'farcall'],
  ['fputs_', 1, 0x0001, 0x0001, 'farcall'],
  ['malloc_', 1, 0x0001, 0x0001, 'farcall'],
  ['free_', 1, 0x0001, 0x0001, 'farcall'],
  ['realloc_', 1, 0x0001, 0x0001, 'farcall'],
  ['atexit_', 1, 0x0001, 0x0001, 'farcall'],
  ['atexit_dos_', 1, 0x0001, 0x0001, 'farcall'],
  ['fseek_', 1, 0x0001, 0x0001, 'farcall'],
  ['fgetc_', 1, 0x0001, 0x0001, 'farcall'],
  ['ungetc_', 1, 0x0001, 0x0001, 'farcall'],
  ['sprintf_', 1, 0x0001, 0x0001, 'farcall'],
  ['abs_', 1, 0x0001, 0x0001, 'farcall'],
  ['strrchr_', 1, 0x0001, 0x0001, 'farcall'],
  ['atol_', 1, 0x0001, 0x0001, 'farcall'],
  ['ltoa_', 1, 0x0001, 0x0001, 'farcall']
];

const env = process.env;


const os = require("os");

env['WATCOM'] = path.resolve(os.homedir(), '.wine/drive_c/WATCOM');
env['PATH'] = path.resolve(env['WATCOM'], 'binl') + ':' + env['PATH'];
env['INCLUDE'] = path.resolve(env['WATCOM'], 'h');

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

const libCDefines = [];

const rodataLabels = [];
const byteMaps = {};
let pageSize;

const cflags = '-dMICROSOFT=1 -dDOS_DAT=1 -mc -zt1 -fpc -0 -ot -zc -ob';

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
    'rm -f build/data.asm;' +
    'touch build/data.asm'
  );

  // update the jump table locations, starting at call_rom3 -4 and working downward in memory
  functionsList.forEach((item, index) => {
    hashMap[item[0].replace(/_$/, '')] = index;
    item[2] = currentAddr;
    currentAddr += 4;
  });

  compileLexer();
}


// compile and split up lexer
function compileLexer () {
  console.log('compileLexer');

  execSync(
    'wcc ' + cflags + ' lexer.c;wdis -s -a -l=build/lexer.asm lexer.obj'
  );

  splitUpFunctions('lexer', compileParser);
}

function compileParser () {
  console.log('compileParser');

  execSync(
    'wcc ' + cflags + ' sql.c;wdis -s -a -l=build/sql.asm sql.obj'
  );

  splitUpFunctions('sql', compileQueryCSV, true);
}

function compileQueryCSV () {
  console.log('compileQueryCSV');

  execSync(
    'wcc ' + cflags + ' querycsv.c;wdis -s -a -l=build/querycsv.asm querycsv.obj'
  );

  splitUpFunctions('querycsv', compileHash2, true);
}

function compileHash2 () {
  console.log('compileHash2');

  execSync(
    'wcc ' + cflags + ' hash2dat.c;wdis -s -a -l=build/hash2dat.asm hash2dat.obj'
  );

  splitUpFunctions('hash2dat', compileHash3, true);
}

function compileHash3 () {
  console.log('compileHash3');

  execSync(
    'wcc ' + cflags + ' hash3.c;wdis -s -a -l=build/hash3.asm hash3.obj'
  );

  splitUpFunctions('hash3', compileHash4a, true);
}

function compileHash4a () {
  console.log('compileHash4a');

  execSync(
    'wcc ' + cflags + ' hash4a.c;wdis -s -a -l=build/hash4a.asm hash4a.obj'
  );

  splitUpFunctions('hash4a', compileHash4b, true);
}

function compileHash4b () {
  console.log('compileHash4b');

  execSync(
    'wcc ' + cflags + ' hash4b.c;wdis -s -a -l=build/hash4b.asm hash4b.obj'
  );

  splitUpFunctions('hash4b', compileHash4c, true);
}

function compileHash4c () {
  console.log('compileHash4c');

  execSync(
    'wcc ' + cflags + ' hash4c.c;wdis -s -a -l=build/hash4c.asm hash4c.obj'
  );

  splitUpFunctions('hash4c', addROData, true);
}

/*
  Compile the read only data to a section of memory at the top of the z80 address space.
  This will be appended to each memory page so will effectively be always paged in */
function addROData () {
  console.log('addROData');

  process.exit(0);

  /* build the global data but exclude the hash4 strings */

  execSync('wasm build/rodata.asm');

  rodataSize = fs.statSync('build/rodata.bin').size;


  pageSize = /*16644*/ /*16644*/ 16383 - rodataSize; // should be 16384 - rodataSize but if we overfit the pages they squash down to within the limit due to the sharing of runtime code between functions which reduces the resultant output binary size
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

  for (i = 0, j = pages.length + 2; i < j; i++) {
    foo += 'defb 0b11111111 ; ' + (i + 1) + '\n';
  }

  // build the asm includes
  ['fcb', 'msx2'].forEach((name, index) => {
    execSync('cp libc.c pager.asm serialLnBC.asm Makefile build/' + name + '/');

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
execSync('wcl -mt -0 -os -s -fm dosload.c');
execSync(`hexdump -v -e '8/1 "0x%02X, "' -e '"\n"' ./dosload.com`+" | sed -e 's/^/db /g;s/, 0x  //g;s/, /,/g;s/,$//g' >dosload.inc");
execSync('wasm -0 pager.asm');
execSync('wcl -ms -0 -os -fpc -s -fm=qrycsv16 -fe=qrycsv16 pager.obj libc.c');

  execSync(
    'wcl -dMICROSOFT=1 -dDOS_DAT=1 -ms -fpc -0 -s -fm=qrycsv16 -fe=qrycsv16 pager.asm libc.c'
  );

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

  const rodataOutputStreams = [];

  const code = fs.createWriteStream('build/' + filename + '_code.asm');

  const functionOutputStreams = [];
  let activeStream = code;
  let rodataType = 0;
  let state = 0;
  let labelBuffer = '';
  let offset = 0;

  lineReader.on('line', line => {
    let name;

    switch (state) {
      case 0:
        if (/^_TEXT\s+SEGMENT\s+[A-Z]+\s+PUBLIC\s+USE16\s+'CODE'/.test(line)) {
          state = 1;
        }
      break;

      case 1:
        if(labelBuffer !== '') {
          if(/^\s+DW/.test(line)) {
            name = labelBuffer.match(/^(L\$[0-9]+):/)[1];

            rodataOutputStreams.push(fs.createWriteStream('build/ro/' + name + '.asm'));

            /* add to the list of rodata regexes used to add the appropriate rodata to each function */
            rodataLabels.push([
              name,
              false,
              new RegExp('(\\b' + name.replace(matchOperatorsRe, '\\$&') + '\\b)', 'm')
            ]);

            activeStream = rodataOutputStreams[rodataOutputStreams.length - 1];
          }

          writePause(activeStream, labelBuffer + '\n' + line + '\n');
          labelBuffer = '';

          return;
        }
        else if (/^CONST\s+SEGMENT\s+[A-Z]+\s+PUBLIC\s+USE16\s+'DATA'/.test(line)) {
          state = 2;
        }
        else if(/^L\$[0-9]+:/.test(line)) {
          labelBuffer = line;

          return;
        }
        else if(!/^_TEXT		ENDS/.test(line)) {
          labelBuffer = '';
          
          if (/^(([^_]|_[^:])+)_:$/.test(line)) {
            name = line.match(/^(([^_]|_[^:])+)_:$/)[1];

            functionOutputStreams.push(fs.createWriteStream('build/s/' + name + '.asm'));

            activeStream = functionOutputStreams[functionOutputStreams.length - 1];
          }

          writePause(
            activeStream,
            line + '\n'
          );
        }
      break;

      case 2:
        if (/^_DATA\s+SEGMENT\s+[A-Z]+\s+PUBLIC\s+USE16\s+'DATA'/.test(line)) {
          state = 3;
          activeStream = data;
        }
        else if([
'CONST2		ENDS',
"_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'",
'_DATA		ENDS',
'CONST		ENDS',
"CONST2		SEGMENT	WORD PUBLIC USE16 'DATA'",
'		END'].includes(line)) {

        }
        else {
          if(/^[^:]+:$/.test(line) && !(/^_hash2/.test(line) && line !== '_hash2_1:')) {
            name = line.match(/^([^:]+):$/)[1];
            
            rodataOutputStreams.push(fs.createWriteStream('build/ro/' + (name === '_hash2_1' ? '_hash2_' : name ) + '.asm'));

            /* add to the list of rodata regexes used to add the appropriate rodata to each function */
            rodataLabels.push([
              name,
              false,
              new RegExp('(\\b' + name.replace(matchOperatorsRe, '\\$&') + '\\b)', 'm')
            ]);

            activeStream = rodataOutputStreams[rodataOutputStreams.length - 1];
          }
          
          writePause(
            activeStream,
            line.replace('DGROUP:', '') + '\n'
          );
        }
      break;

      case 3:
        if(/^_BSS\s+SEGMENT\s+[A-Z]+\s+PUBLIC\s+USE16\s+'BSS'/.test(line)) {
          state = 4;
        }
        else if([
'CONST2		ENDS',
"_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'",
'_DATA		ENDS',
'CONST		ENDS',
"CONST2		SEGMENT	WORD PUBLIC USE16 'DATA'",
'		END'].includes(line)) {

        }
        else {
          writePause(
            activeStream,
            line + '\n'
          );
        }
      break;
      
      case 4:
        if(/^L\$[0-9]+/.test(line)) {
          writePause(
            activeStream,
            line.match(/^(L\$[0-9]+)/)[1] + ':\n'
          );
        }
        else if([
'CONST2		ENDS',
"_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'",
'_DATA		ENDS',
'CONST		ENDS',
"CONST2		SEGMENT	WORD PUBLIC USE16 'DATA'",
'		END'].includes(line)) {

        }
        else if(/ORG/.test(line)){
          let num = parseInt(line.match(/ORG ([0-9]+)$/)[1] || '0',10);
          
          
          if(offset+num !== 0) {
            writePause(
              activeStream,
              '    DB	' + (num - offset) + ' DUP(0)\n'
            );
          }
          
          offset = num;
        }
      break;
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
      //writePause(rodataOutputStreams[i], 'ENDIF\n');
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
      'sed -i "s/;INCLUDE/INCLUDE/g;s/call\\t\\(minusfa\\|ifix\\\\)/call \\1/g;s/jp\\texit/jp\\taexit/g;s/call\\t\\([^dl]\\)/call\\ta\\1/g;s/\\,_\\(get\\|outputResult\\|groupResultsInner\\)/\\,a_\\1/g" ../' + folderName + '/' + filename + '.asm',
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
