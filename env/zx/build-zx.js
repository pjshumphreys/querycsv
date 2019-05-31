const child_process = require('child_process');
const execSync = child_process.execSync;
const fs = require('graceful-fs');
const readline = require('readline');
const walk = require('walk');
const shellEscape = require('shell-escape');
const matchAll = require('match-all');

var matchOperatorsRe = /[|\\{}()\[\]^$+*?.]/g;

const hashMap = {};

const functionsList = [
  ['abs', 3, 0x0001, "farCall" ],
  ['atol', 3, 0x0001, "farCall" ],
  ['exit', 3, 0x0001, "farCall" ],
  ['_strtod', 3, 0x0001, "farCall" ],
  ['mallinit', 3, 0x0001, "farCall" ],
  ['sbrk_callee', 3, 0x0001, "farCall" ],
  ['malloc', 3, 0x0001, "farCall" ],
  ['free', 3, 0x0001, "farCall" ],
  ['realloc_callee', 3, 0x0001, "farCall" ],
  ['calloc_callee', 3, 0x0001, "farCall" ],
  ['strcmp_callee', 3, 0x0001, "farCall" ],
  ['stricmp_callee', 3, 0x0001, "farCall" ],
  ['strncmp_callee', 3, 0x0001, "farCall" ],
  ['strlen', 3, 0x0001, "farCall" ],
  ['strstr_callee', 3, 0x0001, "farCall" ],
  ['strcat_callee', 3, 0x0001, "farCall" ],
  ['strncat_callee', 3, 0x0001, "farCall" ],
  ['memcpy_callee', 3, 0x0001, "farCall" ],
  ['memmove_callee', 3, 0x0001, "farCall" ],
  ['memset_callee', 3, 0x0001, "farCall" ],
  ['fopen', 3, 0x0001, "farCall" ],
  ['fclose', 3, 0x0001, "farCall" ],
  ['fread', 3, 0x0001, "farCall" ],
  ['fwrite', 3, 0x0001, "farCall" ],
  ['fflush', 3, 0x0001, "farCall" ],
  ['fgetc', 3, 0x0001, "farCall" ],
  ['ungetc', 3, 0x0001, "farCall" ],
  ['fgets',3, 0x0001, "farCall" ],
  ['fputc_callee', 3, 0x0001, "farCall" ],
  ['fputs_callee', 3, 0x0001, "farCall" ],
  ['feof', 3, 0x0001, "farCall" ],
  ['fputc_cons', 3, 0xbcf5, "farCall" ],
  ['fprintf', 3, 0x0001, "farCall" ],
  ['sprintf', 3, 0x0001, "farCall" ],
  ['vsnprintf', 3, 0x0001, "farCall" ]
];

const rodataLabels = [];

if(fs.existsSync("querycsv.c")) {
  /* The first action to initiate */
  start();
}

function start() {
  console.log("start");

  /* create the necessary folders */
  execSync(
      "mkdir -p build;"+
      "mkdir -p build/bin;"+
      "mkdir -p build/s;"+
      "mkdir -p build/g;"+
      "mkdir -p build/h;"+
      "mkdir -p build/ro;"+
      "mkdir -p build/obj;"+
      "mkdir -p build/obj2"
    );

  compileLexer();
}

function compileLibC() {
  console.log('compileLibC');

  compileLexer();
}

//compile and split up lexer
function compileLexer() {
  console.log('compileLexer');

  execSync(
    'sed "'+
      's/struct yy_trans_info/flex_int8_t yy_accept2\(unsigned int offset\);'+
      'flex_uint16_t yy_nxt2\(unsigned int offset\);'+
      'flex_int16_t yy_chk2\(unsigned int offset\);'+
      'YY_CHAR yy_ec2\(unsigned int offset\);'+
      'struct yy_trans_info/g;'+
      's/flex_int32_t yy_rule_can_match_eol/flex_int8_t yy_rule_can_match_eol/g;'+
      's/flex_int16_t yy_accept/flex_int8_t yy_accept/g;'+
      's/yy_ec\\[YY_SC_TO_UI(\\*yy_cp)\\]/yy_ec2(YY_SC_TO_UI(*yy_cp))/g;'+
      's/yy_nxt\\[yy_base\\[yy_current_state\\] + (flex_int16_t) yy_c\\]/yy_nxt2\\(yy_base\\[yy_current_state\\] + (flex_int16_t) yy_c)/g;'+
      's/yy_chk\\[yy_base\\[yy_current_state\\] + yy_c\\]/yy_chk2\\(yy_base\\[yy_current_state\\] + yy_c\\)/g;'+
      's/yy_accept\\[yy_current_state\\]/yy_accept2\\(yy_current_state\\)/g;'+
    '" lexer.c > build/lexer2.h'
  );

  execSync('zcc +zx -U__STDC_VERSION__ lexer2.c -E -o build/lexer3.c && ../../makeheaders -h build/lexer3.c | grep -v __LIB__| grep -v extern | grep -v \\#define| sort |uniq> build/lexer3.h');


  execSync('zcc +zx -U__STDC_VERSION__ build/lexer3.c -S -o build/lexer.asm;sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0lexer/g" build/lexer.asm');

  splitUpFunctions("lexer", compileParser);
}

function compileParser() {
  console.log('compileParser');

  execSync(
    'sed "'+
      's/static const char \\*const yytname\\[\\] =/static const char ** yytname =/g;'+
      's/YY_INITIAL_VALUE \(static YYSTYPE yyval_default;\)//g;'+
      's/char const/char/g;'+
      's/YYSTYPE yylval YY_INITIAL_VALUE \(= yyval_default\);/static YYSTYPE yylval;/g;'+
      's/yyval = yyvsp\\[1-yylen\\];//g;'+
      's/static const yytype_int16 yypact\\[\\]/yytype_int16 yypact2\(int offset\);static const yytype_int16 yypact[]/g;'+
      '" sql.c > build/sql2.h');

  execSync(
    'sed -i -r "'+
      's/yypact\\[([^]]+)\\]/yypact2(\\1)/gi;'+
      '" build/sql2.h');

  execSync('zcc +zx -U__STDC_VERSION__ sql2.c -E -o build/sql3.c && ../../makeheaders -h build/sql3.c | grep -v __LIB__| grep -v extern | grep -v \\#define| sort| uniq > build/sql3.h');


  execSync('zcc +zx -U__STDC_VERSION__ sql2.c -S -o build/sql.asm;sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0sql/g" build/sql.asm');

  splitUpFunctions("sql", compileQueryCSV, true);
}

function compileQueryCSV() {
  console.log('compileQueryCSV');

  execSync('zcc +zx -U__STDC_VERSION__ querycsv.c -S -o build/querycsv.asm;sed -i -E "'+
      's/jr/jp/gi;s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0querycsv/g" build/querycsv.asm');

  splitUpFunctions("querycsv", compileHash2, true);

}

/* compile the data segment to a memory page.
(this will be copied to ram on startup) */
function compileData() {
  console.log('compileData');
}

function compileHash2() {
  console.log("compileHash2");

  execSync('zcc +zx -U__STDC_VERSION__ hash2dat.c -S -o build/hash2.asm;sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0hash2/g" build/hash2.asm');

  splitUpFunctions("hash2",  compileHash3, true);
}

function compileHash3() {
  console.log("compileHash3");

  execSync('zcc +zx -U__STDC_VERSION__ hash3.c -S -o build/hash3.asm;sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0ghash3/g" build/hash3.asm');

  splitUpFunctions("hash3",  compileHash4a, true);
}

function compileHash4a() {
  console.log("compileHash4a");

  execSync(
    'sed "'+
      's/struct/const struct/gi;'+
      '" hash4a.c > build/hash4a.c');

  execSync('zcc +zx -U__STDC_VERSION__ build/hash4a.c -S -o build/hash4a.asm;sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0hash4a/g" build/hash4a.asm');

  splitUpFunctions("hash4a",  compileHash4b, true);

}

function compileHash4b() {
  console.log("compileHash4b");

  execSync(
    'sed "'+
      's/struct/const struct/gi;'+
      '" hash4b.c > build/hash4b.c');

  execSync('zcc +zx -U__STDC_VERSION__ build/hash4b.c -S -o build/hash4b.asm;sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0hash4b/g" build/hash4b.asm');

  splitUpFunctions("hash4b",  compileHash4c, true);

}

function compileHash4c() {
  console.log("compileHash4c");

  execSync(
    'sed "'+
      's/struct/const struct/gi;'+
      '" hash4c.c > build/hash4c.c');

  execSync('zcc +zx -U__STDC_VERSION__ build/hash4c.c -S -o build/hash4c.asm;sed -i -E "s/\\bi_[0-9]+(_i_[0-9]+)?\\b/\\0hash4c/g" build/hash4c.asm');

  splitUpFunctions("hash4c",  addROData, true);

}

function addROData() {
  console.log("addROData");

  execSync(" mv build/ro/i_1hash4*.asm build/ && cat build/ro/i_1*.asm >> build/data.asm && "+
      'echo "  SECTION code_compiler" >> build/data.asm && mv build/i_1hash4*.asm build/ro/');
  execSync("sort build/globals.asm | uniq > build/globals2.asm && rm build/globals.asm && mv build/globals2.asm build/globals.asm");


  var list = [];
  var walker = walk.walk('./build/s', {});

  walker.on("file", (root, fileStats, next) => {
    list.push([fileStats.name, fs.readFileSync("build/s/"+fileStats.name, {encoding: 'utf8'})]);

    next();
  });

  walker.on("errors", (root, nodeStatsArray, next) => {
    next();
  });

  walker.on("end", () => {
    list.forEach(updateName);

    getFunctionSizes();
  });
}

/* updateName adds to rodata values needed by each function to the end of
its assembly source file */
function updateName(elem) {
  var text = elem[1];

  var name = elem[0].replace(/\.asm$/, "");

  var hasMatches = false;

  rodataLabels.forEach(element => {
    if(element[1] = element[2].test(text)) {
      hasMatches = true;
    }
  });

  if(hasMatches) {
    execSync(
      'echo "\tSECTION rodata_compiler" >> build/s/'+elem[0] + " &&" +
      " cat "+
      shellEscape(rodataLabels.filter(label => label[1]).map(label => "build/ro/"+label[0]+".asm")) +
      ">> build/s/"+elem[0] +
      ' && echo "\tSECTION code_compiler" >> build/s/'+elem[0]
      );
  }
}

function addDefines(filename, filenames, folderName) {
  let arr = [];
  let notQuit = true;

  console.log("addDefines", filenames);

  execSync(
    'printf "\\\n" > ../'+folderName+'/'+filename+'.inc;'+
    'printf "\\\n" > ../'+folderName+'/'+filename+'2.inc;'+
    'echo "  INCLUDE \\\"z80_crt0.hdr\\\"" > ../'+folderName+'/'+filename+'.asm;'+
    filenames.reduce((obj, elem) => {
      obj += 'cat '+elem+'.asm >> ../'+folderName+'/'+filename+'.asm;';
      return obj;
    }, '') +
    'printf " INCLUDE \\\"../globals.asm\\\"\n" >> ../'+folderName+'/'+filename+'.asm;'+
    'printf " INCLUDE \\\"'+filename+'2.inc\\\"\n" >> ../'+folderName+'/'+filename+'.asm;'+
    'printf " INCLUDE \\\"'+filename+'.inc\\\"\n" >> ../'+folderName+'/'+filename+'.asm',
    {
      cwd:  __dirname+'/build/s'
    });

  while(notQuit) {
    notQuit = false;

    try {
      execSync(
        'zcc +zx '+(folderName === 'h'?'-m ':'')+'--no-crt -lm -lndos -U__STDC_VERSION__'+
        ' -o ../obj/'+filename+'.bin ../'+folderName+'/'+filename+'.asm',
        {
          cwd:  __dirname+'/build/s'
        }
      );
    }
    catch(e) {
      notQuit = true;

      arr = Array.from(new Set(arr.concat(matchAll(e.stderr.toString()+ e.stdout.toString(), /symbol '([^']+)/g).toArray())));

      execSync(
        'rm ../'+folderName+'/'+filename+'2.inc;'+
        'rm ../'+folderName+'/'+filename+'.inc;'+
        'printf "\\\n" > ../'+folderName+'/'+filename+'.inc;'+
        'printf "\\\n" > ../'+folderName+'/'+filename+'2.inc;',
        {
          cwd:  __dirname+'/build/s'
        }
      );

      arr.forEach(elem => {
        execSync(
          'printf "  GLOBAL '+elem+'\n" >> ../'+folderName+'/'+filename+'2.inc;'+
          (
            hashMap.hasOwnProperty(elem) && functionsList[hashMap[elem]][2] !== 1 ?
            'printf "' + elem + ' equ 0x' + ((functionsList[hashMap[elem]][2]+0x10000).
              toString(16).substr(-4).toUpperCase()) :
            'printf ".'+elem) +

            '\n" >> ../'+folderName+'/'+filename+'.inc',
          {
            cwd:  __dirname+'/build/s'
          }
        );
      });
    }
  }

  return {
    name: filename,
    children: arr
  };
}

function getFunctionSizes() {
  console.log("getFunctionSizes");

  /*patch compareCodepoints into the functions that need it (so the table is
  always in the same page) */
  execSync("cat build/s/compareCodepoints.asm >> build/s/getBytesCP1252.asm;sed -i 's/_compareCodepoints/compareCP1252/g;s/querycsv/querycsv1/g;' build/s/getBytesCP1252.asm");
  execSync("cat build/s/compareCodepoints.asm >> build/s/getBytesCommon.asm;sed -i 's/_compareCodepoints/compareCommon/g;s/querycsv/querycsv2/g;' build/s/getBytesCommon.asm");
  execSync("cat build/s/compareCodepoints.asm >> build/s/getBytesPetscii.asm;sed -i 's/_compareCodepoints/comparePetscii/g;s/querycsv/querycsv3/g;' build/s/getBytesPetscii.asm");
  execSync("cat build/s/compareCodepoints.asm >> build/s/getBytesAtariST.asm;sed -i 's/_compareCodepoints/compareAtariST/g;s/querycsv/querycsv4/g;' build/s/getBytesAtariST.asm");
  execSync("rm build/s/compareCodepoints.asm");

  var walker = walk.walk('./build/s', {});

  var list = [];

  walker.on("file", (root, fileStats, next) => {
    if(/\.asm$/.test(fileStats.name)) {
      list.push(fileStats.name.replace(/\.asm$/, '').replace(/^_/, ''));
    }

    next();
  });

  walker.on("errors", (root, nodeStatsArray, next) => {
    next();
  });

  walker.on("end", () => {
    packPages(list.map(elem => addDefines(elem, [elem], 'g')).reduce((obj, elem) => {
      elem.children =
        elem.children
        .map(elem => elem.replace(/^_/, ''))
        .filter(name => fs.existsSync(__dirname + '/build/obj/'+name+'.bin'));

      try {
        elem.size = fs.statSync(__dirname + '/build/obj/'+elem.name+'.bin').size;
      }
      catch(e) {}

      obj[elem.name] = elem;

      return obj;
    }, {}));
  });
}

function packPages(tree) {
  /* use a bin packing algorithm to group the functions close
  to their call-stack parents and produce a binary of each 16k page */

  var pageSize = 16900; //16384;

  var pages = [[tree['main']]];
  var remainingSizes = [pageSize];
  var currentPageData = {};
  var currentPageNumber = 0;
  var placedFunctions = [tree['main']];
  var currentFunctions = [];

  //place the main function to begin with
  tree['main'].pageNumber = 0;
  tree['main'].children.forEach(elem => {
    currentPageData[elem] = true;
  });

  for(;;) {
    //temporarily re-add functions that've already been placed so we can add their children
    currentFunctions = [].concat(placedFunctions);

    for(var loop = 3; loop; loop--) { //try to keep functions fairly close to
    //something further up the call stack
      //find the children of all the current functions in the array.
      var children = currentFunctions.
        map(elem => elem.children).
        reduce((obj, elem) => {
          obj = obj.concat(elem);

          return obj;
        }, []).
        map(elem => tree[elem]);

      //if no children were found then open a new page
      if(children.length === 0) {
        break;
      }

      currentFunctions = currentFunctions.concat(children);

      //sort by size and filter the functions that've already allocated
      currentFunctions = currentFunctions
      .sort((a, b) => {
        //prioritize functions whose parent function is already in the current page
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

      //if the array has become empty then all functions were placed

      //place the biggest first. if all of then add the children. if none of the children can
      for(var i = 0; i < currentFunctions.length; i++) {
        if(remainingSizes[currentPageNumber] - currentFunctions[i].size > 0) {
          remainingSizes[currentPageNumber] -= currentFunctions[i].size;
          pages[currentPageNumber].push(currentFunctions[i]);
          placedFunctions.push(currentFunctions[i]);
          currentFunctions[i].pageNumber = currentPageNumber;
          currentFunctions[i].children.forEach(elem => {
            currentPageData[elem] = true;
          });
        }
      }
    }

    //filter out placed functions
    currentFunctions = currentFunctions.
    filter(elem => !(elem.hasOwnProperty('pageNumber')));

    //if all functions have been placed then quit
    if(currentFunctions.length === 0) {
      break;
    }

    //otherwise open a new page
    pages.push([]);
    remainingSizes.push(pageSize);
    currentPageData = {};
    currentPageNumber++;
  }

  //console.log(JSON.stringify(pages, null, 2));
  var location = 0xbcd4; //call_rom3 -4

  pages.forEach(elem =>
    elem.forEach(elem2 => {
      var a = functionsList[hashMap[elem2.name]];
      a[1] = elem2.pageNumber;

      if(a[2] === 1) {
        a[2] = location;
        location -= 4;
      }
    })
  );

  console.log(JSON.stringify(functionsList, null, 2));

  compilePages(pages);
}

function compilePages(pages) {
  pages.forEach((elem, index) => {
    addDefines('page'+(index+6), elem.map(elem2 => elem2.name), 'h');
  });
}

/* *** HELPER FUNCTIONS AFTER THIS POINT *** */

const byteMaps = {};

/*
Split the specified assembly file into one file for each function's code
(including the necessary rodata) and append to a global data segment.
Pause if node.js can't keep up
*/
function splitUpFunctions(filename, callback, append) {
  console.log('splitUpFunctions');

  let j;

  const lineReader = readline.createInterface({
    input: fs.createReadStream('build/'+filename+'.asm')
  });

  const data = fs.createWriteStream('build/data.asm', {
    flags: append?'a':'w'
  });

  const globals = fs.createWriteStream('build/globals.asm', {
    flags: append?'a':'w'
  });

  if(!append) {
      writePause(
          data,
          "\tSECTION\tdata_compiler\n"
        );
  }

  const rodataOutputStreams = [];

  const code = fs.createWriteStream('build/'+filename+'_code.asm');

  const functionOutputStreams = [];
  let activeStream = code;
  let rodataType = 0;

  lineReader.on('line', function(line) {
    var name;

    if(/SECTION/.test(line)) {
      name = line.replace(/^[ \t]*SECTION[ \t]*/, '');
      if(name == 'code_compiler') {
        rodataType = 0;

        if(functionOutputStreams.length) {
          activeStream = functionOutputStreams[functionOutputStreams.length-1];
        }
        else {
          activeStream = code;
        }
      }
      else if(name == 'rodata_compiler') {
        rodataType = 1;
        activeStream = data;

        //don't output this line
      }
      else {
        rodataType = 2;

        activeStream = data;
      }
    }

    else if (/^;[ \t]+Function[ \t]+/.test(line)) {
      name = (line.replace(/^;[ \t]+Function[ \t]+/, "")).match(/[^ \t]+/)[0];
      functionOutputStreams.push(fs.createWriteStream('build/s/'+name+'.asm'));

      activeStream = functionOutputStreams[functionOutputStreams.length-1];

      writePause(
          activeStream,
          "\tSECTION\t code_compiler\n"+
          line+"\n"
        );

      /* add an entry for each into the mapping table */
      if(name !== "_compareCodepoints") { /* compareCodepoints is a bsearch callback that
        needs to be in the same page as the function that called bsearch. it doesn't need
        to be in the jump table */
        hashMap[name] = functionsList.length;
        functionsList.push([name, 0, 0x0001, "farcall"]);
      }
    }

    else if (rodataType && /^\./.test(line)) {
      if(activeStream) {
        name = line.replace(/^\./, '');

        const name2 = name.replace(/[0-9]+$/, '');

        switch(name2) {

          case '_hash3EntryMap':
          case '_hash2_':
          case '_atariBytes':
          case '_commonBytes':
          case '_cp1252Bytes':
          case '_petsciiBytes': {
            /* add to the list of rodata regexes used to add the appropriate rodata to each function */
            rodataLabels.push([name, false, new RegExp("(\\b"+name.replace(matchOperatorsRe, '\\$&')+"\\b)", "m")]);

            if(!byteMaps.hasOwnProperty(name2)) {
              rodataOutputStreams.push(fs.createWriteStream("build/ro/"+name2+'.asm'));

              byteMaps[name2] = rodataOutputStreams.length-1;
              activeStream = rodataOutputStreams[byteMaps[name2]];

              writePause(activeStream, "IFNDEF "+name+"\n");
            }

            activeStream = rodataOutputStreams[byteMaps[name2]];

            rodataType = 4;
          } break;
        }

        if(rodataType == 1 || rodataType == 3) {
          rodataType = 3;

          rodataOutputStreams.push(fs.createWriteStream("build/ro/"+name+(name === 'i_1'?'':'')+'.asm'));

          /* add to the list of rodata regexes used to add the appropriate rodata to each function */
          rodataLabels.push([name, false, new RegExp("(\\b"+(name==='i_70'?'i_70+':name).replace(matchOperatorsRe, '\\$&')+"\\b)", "m")]);

          activeStream = rodataOutputStreams[rodataOutputStreams.length-1];
          writePause(activeStream, "IFNDEF "+name+"\n");
        }

        writePause(activeStream, line+(name === 'i_1'?'':'')+"\n");
      }
    }

    else if(/^\tGLOBAL/.test(line)) {
      writePause(
          globals,
          line+"\n"
        );
    }

    else if(/\bi_1\b/.test(line)) {
      writePause(
          activeStream,
          line.replace(/\bi_1\b/, 'i_1'+filename)+"\n"
        );
    }

    else {
      writePause(
          activeStream,
          line+"\n"
        );
    }
  });

  lineReader.on('close', function() {
    j = functionOutputStreams.length + rodataOutputStreams.length + 2;

    data.end(writeFunctionPostfixes);
  });

  function writeFunctionPostfixes() {
    code.end(allStreamsClosed);
    globals.end(allStreamsClosed);

    for(i = 0; i < rodataOutputStreams.length; i++) {
      /* close current stream */
      writePause(rodataOutputStreams[i], "ENDIF\n");
      rodataOutputStreams[i].end(allStreamsClosed);
    }

    for(i = 0; i < functionOutputStreams.length; i++) {
      /* close current stream */
      functionOutputStreams[i].end(allStreamsClosed);
    }
  }

  /* when all streams have been closed, go to the next step (compiling the
  data segment) */
  function allStreamsClosed() {
    if(--j == 0) {
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