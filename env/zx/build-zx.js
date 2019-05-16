const child_process = require('child_process');
const execSync = child_process.execSync;
const fs = require('graceful-fs');
const readline = require('readline');
const walk = require('walk');
const shellEscape = require('shell-escape');

var matchOperatorsRe = /[|\\{}()\[\]^$+*?.]/g;
var replaceAll = (str, mapObj) => str.replace(
    new RegExp(
        Object.
          keys(mapObj).
          map(x => "\\b"+(x.replace(matchOperatorsRe, "\\$&"))+"\\b").
          join("|"),
        "gi"
      ),
    matched => mapObj[matched]
  );

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
  ['fgets','_fgets', 3, 0x0001, "farCall" ],
  ['fputc_callee', 3, 0x0001, "farCall" ],
  ['fputs_callee', 3, 0x0001, "farCall" ],
  ['feof', 3, 0x0001, "farCall" ],
  ['fprintf', 3, 0x0001, "farCall" ],
  ['sprintf', 3, 0x0001, "farCall" ],
  ['vsnprintf', 3, 0x0001, "farCall" ],

  ["_in_word_set_a", 0, 0x0001, "farCall"],
  ["_in_word_set_b", 0, 0x0001, "farCall"],
  ["_in_word_set_c", 0, 0x0001, "farCall"]
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
    '" lexer.c > build/lexer2.c'
  );

  execSync('zcc +zx -U__STDC_VERSION__ build/lexer2.c -S -o build/lexer.asm');

  splitUpFunctions("lexer", compileParser);
}

function compileParser() {
  console.log('compileParser');

  execSync(
    'sed "'+
      's/static const char \\*const yytname\\[\\] =/static char ** yytname =/g;'+
      's/const//g;'+
      's/YY_INITIAL_VALUE \(static YYSTYPE yyval_default;\)//g;'+
      's/YYSTYPE yylval YY_INITIAL_VALUE \(= yyval_default\);/static YYSTYPE yylval;/g;'+
      's/yyval = yyvsp\\[1-yylen\\];//g;'+
      's/static const yytype_int16 yypact\\[\\]/yytype_int16 yypact2\(int offset\);static const yytype_int16 yypact[]/g;'+
      '" sql.c > build/sql2.c');

  execSync('zcc +zx -U__STDC_VERSION__ build/sql2.c -S -o build/sql.asm');

  splitUpFunctions("sql", compileQueryCSV);
}

function compileQueryCSV() {
  console.log('compileQueryCSV');

  execSync('zcc +zx -U__STDC_VERSION__ querycsv.c -S -o build/querycsv.asm');

  splitUpFunctions("querycsv", compileData);

}

/* compile the data segment to a memory page.
(this will be copied to ram on startup) */
function compileData() {
  console.log('compileData');
}

function compileHash2() {
  console.log("compileHash2");
}

function compileHash3And4() {
  console.log("compileHash3And4");
}

function compileYYParse() {
  console.log("compileYYParse");
}

function packPages() {
  console.log("packPages");
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
    input: fs.createReadStream('build/'+filename+'.asm')
  });

  const data = fs.createWriteStream('build/data.asm', {
    flags: append?'a':'w'
  });

  const rodataOutputStreams = [];

  const code = fs.createWriteStream('build/'+filename+'_code.s');

  const functionOutputStreams = [];
  let activeStream = code;
  let rodataType = 0;

  lineReader.on('line', function(line) {
    var name;

    if(/SECTION/.test(line)) {
      name = line.replace(/^[ \t]*SECTION[ \t]*/, '');
      console.log('here', name);

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

        if(name !== "bss_compiler") {
          writePause(activeStream, line+"\n");
        }
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


        if(rodataType == 1 || rodataType == 3) {
          rodataType = 3;

          rodataOutputStreams.push(fs.createWriteStream("build/ro/"+name+(name === 'i_1'?filename:'')+'.asm'));

          /* add to the list of rodata regexes used to add the appropriate rodata to each function */
          rodataLabels.push([name, false, new RegExp("(\\b"+name.replace(matchOperatorsRe, '\\$&')+"\\b)", "m")]);

          activeStream = rodataOutputStreams[rodataOutputStreams.length-1];
        }

        writePause(activeStream, line.replace(name, name.toLowerCase()+filename)+"\n");
      }
    }

    else {
      writePause(
          activeStream,
          line+"\n"
        );
    }
  });

  lineReader.on('close', function() {
    j = functionOutputStreams.length + rodataOutputStreams.length + 1;

    data.end(writeFunctionPostfixes);
  });

  function writeFunctionPostfixes() {
    code.end(allStreamsClosed);

    for(i = 0; i < rodataOutputStreams.length; i++) {
      /* close current stream */
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
