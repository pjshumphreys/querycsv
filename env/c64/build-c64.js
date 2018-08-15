var child_process = require('child_process');
var execSync = child_process.execSync;
var exec = child_process.exec;
var spawn = child_process.spawn;
var spawnSync = child_process.spawnSync;
var fs = require('graceful-fs');
var readline = require('readline');
var walk = require('walk');
var shellEscape = require('shell-escape');
var i;
var files;

var hash2ChunkCount;

var remainder = 16384;

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

var hashMap = {};
var passPostfix = "";

/*functionsList = A mapping of function names to the memory page that
contains them, the address within that page and which trampoline function
in the $C000 range to call to set them up correctly*/
var functionsList = [
  ["BASIC_FAC_to_string", 1, 0xBDDD, "FUNC0"],   /* in: FAC value   out: str at $0100 a/y ptr to str */
  ["BASIC_string_to_FAC", 1, 0xB7B5, "FUNC0"],   /* in: $22/$23 ptr to str,a=strlen out: FAC value */

  ["BASIC_s8_to_FAC",     1, 0xBC3C, "FUNC0"],    /* a: value */
  ["BASIC_u8_to_FAC",     1, 0xB3A2, "FUNC0"],    /* y: value */

  ["BASIC_u16_to_FAC",    1, 0xBC49, "FUNC0"],   /* a/y:lo/hi value (sta $62 sty $63 sec ldx#$90 jsr...) */
  ["BASIC_s16_to_FAC",    1, 0xB395, "FUNC0"],   /* a/y:lo/hi value */
  ["BASIC_FAC_to_u16",    1, 0xBC9B, "FUNC0"],   /* in:FAC out: y/a:lo/hi value */

/*------------------------------------------------------------------------- */
/* these functions take one arg (in FAC) and return result (in FAC) as well */
/*------------------------------------------------------------------------- */

  ["__fabs",              1, 0xBC58, "FUNC1"],   /* in/out: FAC */
  ["__fatn",              1, 0xE30E, "FUNC1"],   /* in/out: FAC */
  ["__fcos",              1, 0xE264, "FUNC1"],   /* in/out: FAC */
  ["__fexp",              1, 0xBFED, "FUNC1"],   /* in/out: FAC */
/* __ffre:    FUNC1 BASIC_FAC_Fre */
  ["__fint",              1, 0xBCCC, "FUNC1"],   /* in/out: FAC */
  ["__flog",              1, 0xB9EA, "FUNC1"],   /* in/out: FAC */
/* __fpos:    FUNC1 BASIC_FAC_Pos */
  ["__frnd",              1, 0xE097, "FUNC1"],   /* in/out: FAC */
  ["__fsgn",              1, 0xBC39, "FUNC1"],   /* in/out: FAC */
  ["__fsin",              1, 0xE26B, "FUNC1"],   /* in/out: FAC */
  ["__fsqr",              1, 0xBF71, "FUNC1"],   /* in/out: FAC */
  ["__ftan",              1, 0xE2B4, "FUNC1"],   /* in/out: FAC */
  ["__fnot",              1, 0xAED4, "FUNC1"],   /* in/out: FAC */
  ["__fround",            1, 0xBC1B, "FUNC1"],   /* in/out: FAC */

/*-------------------------------------------------------------------------- */
/* these functions take two args (in FAC and ARG) and return result (in FAC) */
/*-------------------------------------------------------------------------- */

  ["__fadd",              1, 0xB86A, "FUNC2"],   /* in: ARG,FAC out:FAC */
  ["__fsub",              1, 0xB853, "FUNC2"],   /* in: ARG,FAC out:FAC */
  ["__fmul",              1, 0xBA2B, "FUNC2"],   /* in: ARG,FAC out:FAC */
  ["__fdiv",              1, 0xBB12, "FUNC2"],   /* in: ARG,FAC out:FAC */
  ["__fpow",              1, 0xBF7B, "FUNC2"],   /* in: ARG,FAC out:FAC */

  ["__fand",              1, 0xAFE9, "FUNC3"],   /* in: ARG,FAC out:FAC */
  ["__for",               1, 0xAFE6, "FUNC3"],   /* in: ARG,FAC out:FAC */

  ["BASIC_FAC_cmp",       1, 0xBC5B, "FUNC0"],   /* in: FAC(x1) a/y ptr lo/hi to x2 out: a=0 (x1=x2) a=1 (x1>x2) a=255 (x1<x2) */
  ["BASIC_FAC_testsgn",   1, 0xBC2B, "FUNC0"],   /* in: FAC(x1) out: a=0 (x1=0) a=1 (x1>0) a=255 (x1<0) */
  ["BASIC_FAC_Poly1",     1, 0xE059, "FUNC0"],   /* in: FAC x  a/y ptr to poly (1byte grade,5bytes per coefficient) */

  /*normally these functions are wrapped by more code, but we need to be able
  to call them in a raw form */
  ["BASIC_FAC_Atn",       1, 0xE30E, "FUNC0"],   /* in/out: FAC */
  ["BASIC_ARG_FAC_Div",   1, 0xBB12, "FUNC0"],   /* in: ARG,FAC out:FAC */
  ["BASIC_ARG_FAC_Add",   1, 0xB86A, "FUNC0"],    /* in: ARG,FAC out:FAC */
];

/* these are unused */
/*BASIC_FAC_Poly2   = $e043     ; in: FAC x  a/y ptr to poly (1byte grade,5bytes per coefficient) */

/*BASIC_LoadARG   = $babc     ; a/y:lo/hi ptr to 5-byte float */
/*BASIC_LoadFAC   = $bba2     ; a/y:lo/hi ptr to 5-byte float */

var floatlibFunctionsList = [
  ["__ftostr",            1, 0x0001, "farcall"],
  ["__strtof",            1, 0x0001, "farcall"],
  ["__ctof",              1, 0x0001, "farcall"],
  ["__utof",              1, 0x0001, "farcall"],
  ["__stof",              1, 0x0001, "farcall"],
  ["__itof",              1, 0x0001, "farcall"],
  ["__ftoi",              1, 0x0001, "farcall"],
  ["__fcmp",              1, 0x0001, "farcall"],
  ["__ftestsgn",          1, 0x0001, "farcall"],
  ["__fneg",              1, 0x0001, "farcall"],
  ["__fpoly1",            1, 0x0001, "farcall"],
  ["__fpoly2",            1, 0x0001, "farcall"],
  ["__fatan2",            1, 0x0001, "farcall"],
];

var clibFunctionsList = [
  /* standard C library functions */
  ["_strtod",             1, 0x0001, "farcall"], /*this'll be provided by my
  floatib wrapper */
  ["_ltoa",               2, 0x0001, "farcall2"],
  ["_ultoa",              2, 0x0001, "farcall2"],
  ["_clearerr",           2, 0x0001, "farcall2"],
  ["_fopen",              2, 0x0001, "farcall2"],
  ["_fclose",             2, 0x0001, "farcall2"],
  ["_feof",               2, 0x0001, "farcall2"],
  ["_ferror",             2, 0x0001, "farcall2"],
  ["_fflush",             2, 0x0001, "farcall2"],
  ["_fread",              2, 0x0001, "farcall2"],
  ["_fwrite",             2, 0x0001, "farcall2"],
  ["_fgetc",              2, 0x0001, "farcall2"],
  ["_ungetc",             2, 0x0001, "farcall2"],
  ["_fputc",              2, 0x0001, "farcall2"],
  ["_fgets",              2, 0x0001, "farcall2"],
  ["_malloc",             2, 0x0001, "farcall2"],
  ["_calloc",             2, 0x0001, "farcall2"],
  ["_realloc",            2, 0x0001, "farcall2"],
  ["_free",               2, 0x0001, "farcall2"],
  ["_memcpy",             2, 0x0001, "farcall2"],
  ["_memmove",            2, 0x0001, "farcall2"],
  ["_memset",             2, 0x0001, "farcall2"],
  ["_strcat",             2, 0x0001, "farcall2"],
  ["_strcmp",             2, 0x0001, "farcall2"],
  ["_strcpy",             2, 0x0001, "farcall2"],
  ["_stricmp",            2, 0x0001, "farcall2"],
  ["_strlen",             2, 0x0001, "farcall2"],
  ["_strncat",            2, 0x0001, "farcall2"],
  ["_strncmp",            2, 0x0001, "farcall2"],
  ["_strnicmp",           2, 0x0001, "farcall2"],
  ["_strstr",             2, 0x0001, "farcall2"],
  ["_sprintf",            2, 0x0001, "farcall2"],
  ["_vsnprintf",          2, 0x0001, "farcall2"],
  ["_vsprintf",           2, 0x0001, "farcall2"],

  //other special functions we'll need
  ["_isCombiningChar",    0, 0x0001, "farcall2"],
  ["_in_word_set_a",      0, 0x0001, "farcall2"],
  ["_in_word_set_b",      0, 0x0001, "farcall2"],
  ["_in_word_set_c",      0, 0x0001, "farcall2"]
];

var defines = {
  "_stderr": 0,
  "_stdin": 0,
  "_stdout": 0,
  "zerobss2": 0,
  "initlib2": 0,
  "donelib2": 0,
  "___float_fac_to_float": 0,
  "___float_float_to_fac": 0,
  "___float_float_to_fac_arg": 0,
  "aRegBackup": 0,
  "spRegBackup": 0,
  "xRegBackup": 0
};

var rodataLabels = [];

/* The first action to initiate */
start();

function start() {
  console.log("start");

  /* create the necessary folders */
  execSync(
      "mkdir -p bin;"+
      "mkdir -p s;"+
      "mkdir -p g;"+
      "mkdir -p ro;"+
      "mkdir -p obj;"+
      "mkdir -p obj2"
    );

  createFloatLibInclude();
}

/* create the cc65-floatlib include */
function createFloatLibInclude() {
  console.log('createFloatLibInclude');

  var tables = fs.createWriteStream('floatlib/floatlib.inc');

  for(i = 0; i < functionsList.length; i++) {
    hashMap[functionsList[i][0]] = i;

    tables.write(
        functionsList[i][0] + ":\n" +
        "  stx xRegBackup\n" +
        "  ldx #$"+("00"+(i.toString(16).toUpperCase())).substr(-2)+"\n" +
        "  jmp "+functionsList[i][3]+"\n"
      );
  }

  tables.write("\nhighAddressTable:\n");
  for(i = 0; i < functionsList.length; i++) {
    tables.write(
        ".byte $"+
        ("0000"+(functionsList[i][2]-1).toString(16).toUpperCase()).
        substr(-4).
        substring(0,2)+
        "\n"
      );
  }

  tables.write("\nlowAddressTable:\n");
  for(i = 0; i < functionsList.length; i++) {
    tables.write(
        ".byte $" +
        ("00"+(functionsList[i][2]-1).toString(16).toUpperCase()).
        substr(-2)+
        "\n"
      );
  }

  for(i = 0; i < floatlibFunctionsList.length; i++) {
    tables.write(".export "+floatlibFunctionsList[i][0]+"\n");
  }

  tables.end(compileFloatLib);
}

/* compile cc65-floatlib. */
function compileFloatLib() {
  console.log('compileFloatLib');

  /* compile the floatlib portion written in C (strtod) */
  execSync("cc65 -T -t c64 -I ./floatlib floatlib.c");

  /* link into a binary */
  execSync("cl65 -T -t c64 "+
      "-I ./floatlib "+
      "-Ln floatlib.lbl "+
      "--config floatlib"+passPostfix+".cfg "+
      "floatlib/float.s floatlib.s"
    );

  /*create an zeroed binary the same size as the total ram used */
  execSync(
      "("+
        "echo -n \"ibase=16;scale=16;\" && "+
        "((grep __RAM2_LAST__ floatlib.lbl|"+
        "sed -n \"s/al \\([^ ]*\\).*/\\1/p\")|tr -d '\\n')"+
        " && echo -n \"-\" && "+
        "grep __RAM2_START__ floatlib.lbl|"+
        "sed -n \"s/al \\([^ ]*\\).*/\\1/p\""+
      ")|bc|"+
      "xargs -I {} dd if=/dev/zero bs=1 count={} of=floatlibdata.bin"
    );

  /* Create a combined binary of the data and bss
  segments so that they can be copied as one blob */
  execSync(
      "dd if=floatlibdata2.bin of=floatlibdata.bin conv=notrunc;"+
      "rm floatlibdata2.bin"
    );

  if(passPostfix === "") {
    /* add the float lib functions to the hashmap */
    floatlibFunctionsList.forEach(function(v) {
      hashMap[v[0]] = this.length;
      this.push(v);
    }, functionsList);

    clibFunctionsList.forEach(function(v) {
      hashMap[v[0]] = this.length;
      this.push(v);
    }, functionsList);

    hash2ChunkCount = 15;

    for(i = 0; i < hash2ChunkCount; i++) {
      /*
      put an entry in the functions list for each file.
      as the code in each of them doesn't need to call any other page
      we can probably use farret2
      */
      var name = '_isInHash2_'+i;
      hashMap[name] = functionsList.length;
      functionsList.push([
          name,
          0,
          0,
          "farcall2"
        ]);
    }
  }

  /* read the label file and use its contents to
  update each function address in the hashmap */
  var lineReader = readline.createInterface({
      input: fs.createReadStream('floatlib.lbl')
    });

  lineReader.on('line', updateFunctionAddress.bind(1));

  /* when finished, start the next step (compiling libc) */
  lineReader.on('close', compileLibC);
}

/* compile cc65 standard library */
function compileLibC() {
  console.log('compileLibC');

  /* compile a fake program that uses c library */
  execSync(
      "cl65 -T -t c64 -O -Os "+
      "-Ln libc.lbl "+
      "--static-locals "+
      "--config libc"+passPostfix+".cfg "+
      "libc.c"
    );

  /*create an zeroed binary the same size as the total ram used */
  execSync(
      "("+
        "echo -n \"ibase=16;scale=16;\" && "+
        "((grep __RAM2_LAST__ libc.lbl|"+
        "sed -n \"s/al \\([^ ]*\\).*/\\1/p\")|tr -d '\n')"+
        " && echo -n \"-\" && "+
        "grep __RAM2_START__ libc.lbl|"+
        "sed -n \"s/al \\([^ ]*\\).*/\\1/p\""+
      ")|bc|"+
      "xargs -I {} dd if=/dev/zero bs=1 count={} of=libcdata.bin"
    );

  /* Create a combined binary of the ram segments
  so that they can be copied as one blob */
  execSync(
      "dd if=libcdata2.bin of=libcdata.bin conv=notrunc;"+
      "rm libcdata2.bin"
    );

  /* read the label file and use its contents to
  update each function address in the hashmap */
  var lineReader = readline.createInterface({
    input: fs.createReadStream('libc.lbl')
  });

  lineReader.on('line', updateFunctionAddress.bind(2));

  /* when finished, start the next step (compiling the functions that
  make up my program) */
  lineReader.on('close', passPostfix===""?compileLexer:compileData);
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
        's/%s/%S/g;'+
        's/yyconst char msg\\[\\]/yyconst char *msg/g;'+
        's/flex_int32_t yy_rule_can_match_eol/flex_int8_t yy_rule_can_match_eol/g;'+
        's/flex_int16_t yy_accept/flex_int8_t yy_accept/g;'+
        's/yy_ec\\[YY_SC_TO_UI(\\*yy_cp)\\]/yy_ec2(YY_SC_TO_UI(*yy_cp))/g;'+
        's/yy_nxt\\[yy_base\\[yy_current_state\\] + (flex_int16_t) yy_c\\]/yy_nxt2\\(yy_base\\[yy_current_state\\] + (flex_int16_t) yy_c)/g;'+
        's/yy_chk\\[yy_base\\[yy_current_state\\] + yy_c\\]/yy_chk2\\(yy_base\\[yy_current_state\\] + yy_c\\)/g;'+
        's/yy_accept\\[yy_current_state\\]/yy_accept2\\(yy_current_state\\)/g;'+
      '" lexer.c > lexer2.h'
    );

  /* compile functions into assembly language. use our own
  patched cc65 executable that does "jmp farret" instead of "rts" */
  execSync('./cc65_2 -T -t c64 lexer2.c -O -Os --static-locals --writable-strings');

  splitUpFunctions("lexer2", compileParser);
}

function compileParser() {
  console.log('compileParser');

  execSync(
    'sed "'+
      's/YY_INITIAL_VALUE \(static YYSTYPE yyval_default;\)//g;'+
      's/define YY_LAC_ESTABLISH/define YY_LAC_OESTABLISH/g;'+
      's/%s/%S/g;'+
      's/%d/%D/g;'+
      's/%lu/%LU/g;'+
      's/\'s\'/\'S\'/g;'+
      's/YYSTYPE yylval YY_INITIAL_VALUE \(= yyval_default\);/static YYSTYPE yylval;/g;'+
      's/static const yytype_int16 yypact\\[\\]/yytype_int16 yypact2\(int offset\);static const yytype_int16 yypact[]/g;'+
      '" sql.c > sql2.h');

  execSync(
    'sed -i -r "'+
      's/yypact\\[([^]]+)\\]/yypact2(\\1)/gi;'+
      '" sql2.h');

  execSync('./cc65_2 -T -t c64 -O -Os sql2.c --static-locals --writable-strings');

  //split parser up into 1 function per .s file
  //(including all rodata. add all data vars to a single .s file)
  splitUpFunctions("sql2", compileC64, true);
}

function compileC64() {
  console.log('compileC64');

  execSync("./cc65_2 -T -t c64 c64.c --writable-strings");

  splitUpFunctions("c64", compileQueryCSV, true);
}

function compileQueryCSV() {
  console.log('compileQueryCSV');

  execSync("./cc65_2 -T -t c64 querycsv.c --writable-strings");

  splitUpFunctions("querycsv", compileData, true);
}

/* compile the data segment to a memory page.
(this will be copied to ram on startup) */
function compileData() {
  console.log('compileData');

  /* compile data and bss segments. generate an assembly
  language include of all the labels */

  execSync(
      "cl65 "+
      "-Ln data.lbl "+
      "-C data"+passPostfix+".cfg "+
      "data.s -vm -m data.map"
    );

  execSync(
      "("+
        "echo -n \"ibase=16;scale=16;\" && "+
        "((grep __RAM2_LAST__ data.lbl|"+
        "sed -n \"s/al \\([^ ]*\\).*/\\1/p\")|tr -d '\n')"+
        " && echo -n \"-\" && "+
        "grep __RAM2_START__ data.lbl|"+
        "sed -n \"s/al \\([^ ]*\\).*/\\1/p\""+
      ")|bc|"+
      "xargs -I {} dd if=/dev/zero bs=1 count={} of=data.bin"
    );

  /*TODO: confirm this: Create a combined binary of the ram segments
  so that they can be copied as one blob? */
  execSync("dd if=data2.bin of=data.bin conv=notrunc;rm data2.bin");

  var lineReader2 = readline.createInterface({
    input: fs.createReadStream('data.lbl')
  });

  var labels = fs.createWriteStream('labels.s');

  /* read the resultant memory locations and make an assembly include
  containing just their addresses */
  lineReader2.on('line', line => {
    var name = line.replace(/^al\s+[0-9A-F]+ \./, "");

    if(name.match(/l[0-9a-f]{4}/)) {
      var address = parseInt(line.match(/[0-9A-F]+/), 16);

      labels.write(
          name+" = $"+
          ("0000"+address.toString(16).toUpperCase()).substr(-4)+
          "\n"
        );
    }
    //starts with an underscore (name defined by the c source code)
    else if(name.match(/\b_[0-9a-zA-Z]/) || name === "jmpvec") {// && name !== '_main') {
      var address = parseInt(line.match(/[0-9A-F]+/), 16);

      labels.write(".export "+name+"\n");

      labels.write(
          name+" = $"+
          ("0000"+address.toString(16).toUpperCase()).substr(-4)+
          "\n"
        );
    }
  });

  lineReader2.on('close', () => {
    labels.end(compileHash2);
  });
}

function compileHash2() {
  var i, name;

  console.log("compileHash2");

  for(i = 0; fs.existsSync('./hash2in'+i+'.c'); i++) {
    execSync(
        "cl65 -T -t c64 "+
        "-o obj2/hash2in"+i+".bin "+
        "-Ln obj2/hash2in"+i+".lbl "+
        "-C rodata-page.cfg "+
        "hash2in"+i+".c labels.s;"+
        "rm *.o"
      );

    /*
      put an entry in the functions list for each file.
      as the code in each of them doesn't need to call any other page
      we can probably use farret2
    */
    name = '_isInHash2_'+i;
    functionsList[hashMap[name]][2] = parseInt(execSync(
      'sh -c "(echo -n \\"ibase=16;scale=16;\\" && (grep _isInHash2_'+
      i+
      ' obj2/hash2in'+
      i+
      '.lbl|sed -n \\"s/al \\([^ ]*\\).*/\\1/p\\"))|bc"'
    ).toString(), 10);
  }

  if(passPostfix === "") {
    /* allow the hash2 function to be packed also */
    execSync('./cc65_2 -T -t c64 hash2out.c');
    splitUpFunctions("hash2out", compileHash3And4, true);
  }
  else {
      compileHash3And4();
  }
}

function compileHash3And4() {
  console.log("compileHash4");

  execSync(
      "cl65 -T -t c64 "+
      "-o obj2/hash4a.bin "+
      "-Ln obj2/hash4a.lbl "+
      "-C rodata-page.cfg "+
      "hash4a.c labels.s;"+
      "rm *.o");

  functionsList[hashMap['_in_word_set_a']][2] = parseInt(execSync(
      'sh -c "(echo -n \\"ibase=16;scale=16;\\" && (grep _in_word_set_a'+
      ' obj2/hash4a.lbl|sed -n \\"s/al \\([^ ]*\\).*/\\1/p\\"))|bc"'
    ).toString(), 10);

  execSync(
      "cl65 -T -t c64 "+
      "-o obj2/hash4b.bin "+
      "-Ln obj2/hash4b.lbl "+
      "-C rodata-page.cfg "+
      "hash4b.c labels.s;"+
      "rm *.o"
    );

  functionsList[hashMap['_in_word_set_b']][2] = parseInt(execSync(
      'sh -c "(echo -n \\"ibase=16;scale=16;\\" && (grep _in_word_set_b'+
      ' obj2/hash4b.lbl|sed -n \\"s/al \\([^ ]*\\).*/\\1/p\\"))|bc"'
    ).toString(), 10);

  execSync(
      "cl65 -T -t c64 "+
      "-o obj2/hash4c.bin "+
      "-Ln obj2/hash4c.lbl "+
      "-C rodata-page.cfg "+
      "hash4c.c hash3.c labels.s;"+
      "rm *.o"
    );

  functionsList[hashMap['_in_word_set_c']][2] = parseInt(execSync(
      'sh -c "(echo -n \\"ibase=16;scale=16;\\" && (grep _in_word_set_c'+
      ' obj2/hash4c.lbl|sed -n \\"s/al \\([^ ]*\\).*/\\1/p\\"))|bc"'
    ).toString(), 10);

  functionsList[hashMap['_isCombiningChar']][2] = parseInt(execSync(
      'sh -c "(echo -n \\"ibase=16;scale=16;\\" && (grep _isCombiningChar'+
      ' obj2/hash4c.lbl|sed -n \\"s/al \\([^ ]*\\).*/\\1/p\\"))|bc"'
    ).toString(), 10);

  createTrampolinesInclude();
}

function createTrampolinesInclude() {
  console.log("createTrampolinesInclude");

  var tables = fs.createWriteStream('tables.inc');

  for(i in defines) {
    tables.write(
        ".export "+i+"\n"+
        i+" = $"+
        ("0000"+((defines[i]).toString(16).toUpperCase())).substr(-4)+
        "\n"
      );
  }

  for(i = 0; i < functionsList.length; i++) {
    let secondTable = i > 255;

    if(secondTable && functionsList[i][3] == 'farcall') {
      functionsList[i][3] = 'farcall3';
    }

    tables.write(
        ".export "+functionsList[i][0]+"\n"+

        functionsList[i][0]+":\n"+
        "  stx xRegBackup\n"+
        "  ldx #$"+("00"+(i.toString(16).toUpperCase())).substr(-2)+"\n"+
        "  jmp "+functionsList[i][3]+"\n"
      );
  }

  tables.write("\nhighAddressTable:\n");
  for(i = 0; i < Math.min(256, functionsList.length); i++) {
    tables.write(
        ".byte $"+
        ("0000"+((functionsList[i][2]-1).toString(16).toUpperCase())).
        substr(-4).
        substring(0,2)+
        " ;"+functionsList[i][0]+
        "\n"
      );
  }

  tables.write("\nlowAddressTable:\n");
  for(i = 0; i < Math.min(256, functionsList.length); i++) {
    tables.write(
        ".byte $"+
        ("00"+((functionsList[i][2]-1).toString(16).toUpperCase())).
        substr(-2)+
        " ;"+functionsList[i][0]+
        "\n"
      );
  }

  tables.write("\nbankTable:\n");
  for(i = 0; i < Math.min(256, functionsList.length); i++) {
    tables.write(
        ".byte $"+
        ("00"+(functionsList[i][1].toString(16).toUpperCase())).
        substr(-2)+
        " ;"+functionsList[i][0]+
        "\n"
      );
  }

  tables.write("\nhighAddressTable2:\n");
  for(i = 256; i < functionsList.length; i++) {
    tables.write(
        ".byte $"+
        ("0000"+((functionsList[i][2]-1).toString(16).toUpperCase())).
        substr(-4).
        substring(0,2)+
        " ;"+functionsList[i][0]+
        "\n"
      );
  }

  tables.write("\nlowAddressTable2:\n");
  for(i = 256; i < functionsList.length; i++) {
    tables.write(
        ".byte $"+
        ("00"+((functionsList[i][2]-1).toString(16).toUpperCase())).
        substr(-2)+
        " ;"+functionsList[i][0]+
        "\n"
      );
  }

  tables.write("\nbankTable2:\n");
  for(i = 256; i < functionsList.length; i++) {
    tables.write(
        ".byte $"+
        ("00"+(functionsList[i][1].toString(16).toUpperCase())).
        substr(-2)+
        " ;"+functionsList[i][0]+
        "\n"
      );
  }

  tables.end(compileMain);
}

function compileMain() {
  console.log("compileMain");

  execSync("cl65 -T -t c64 -Ln main.lbl --config main"+(passPostfix===""?"":"a")+".cfg crt0.s");

  execSync(
      "("+
        "echo -n \"ibase=16;scale=16;\" && "+
        "((grep __RAM2_LAST__ main.lbl|"+
        "sed -n \"s/al \\([^ ]*\\).*/\\1/p\")|tr -d '\\n')"+
        " && echo -n \"-\" && "+
        "grep __RAM2_START__ main.lbl|"+
        "sed -n \"s/al \\([^ ]*\\).*/\\1/p\""+
      ")|bc|"+
      "xargs -I {} dd if=/dev/zero bs=1 count={} of=maindata.bin"
    );

  /*read the label file and update each function address */

  var lineReader = readline.createInterface({
    input: fs.createReadStream('main.lbl')
  });

  execSync("cat labels.s > labels2.s");

  var labels = fs.createWriteStream('labels2.s', { flags: 'a' });

  for(i in defines) {
    labels.write(
        i+" = $"+
        ("0000"+((defines[i]).toString(16).toUpperCase())).substr(-4)+
        "\n"
      );
  }

  labels.write(".export _main\n");
  labels.write(".import pushl0\n");

  lineReader.on('line', function(line) {
    var name = line.replace(/^al\s+[0-9A-F]+ \./, "");
    var address = parseInt(line.match(/[0-9A-F]+/), 16);

    if(name == "farret") {
      labels.write(
          "farret = $"+
          ("0000"+(address.toString(16).toUpperCase())).substr(-4)+
          "\n"
        );
    }

    if(name == "_exit") {
      labels.write(
          "_exit = $"+
          ("0000"+(address.toString(16).toUpperCase())).substr(-4)+
          "\n"
        );
    }

    if(hashMap.hasOwnProperty(name)) {
      labels.write(
          name+" = $"+
          ("0000"+(address.toString(16).toUpperCase())).substr(-4)+
          "\n"
        );
    }
  });

  lineReader.on('close', function() {
    if(passPostfix === "") {
      labels.end(calculateSizes);
    }
    else if(passPostfix === "a") {
      labels.end(compileYYParse);
    }
    else {
      labels.end(glueTogetherBinary);
    }
  });
}

/* compile yyparse twice, first to get its size, then to locate it in the proper place */
function calculateSizes() {
  console.log("calculateSizes");

  updateName(["_yyparse.s", fs.readFileSync("s/_yyparse.s", {encoding: 'utf8'})]);

  execSync(
      "echo \".include \\\"../header.inc\\\"\" > ./g/_yyparse.s;"+
      "echo \".include \\\"../labels2.s\\\"\" >> ./g/_yyparse.s;"+
      "cat ./s/_yyparse.s >> ./g/_yyparse.s;"+
      "cl65 -T -t c64 "+
      "-o ./obj2/yyparse.bin "+
      "-Ln ./obj2/yyparse.lbl "+
      "-C ./yyparse.cfg ./g/_yyparse.s"
    );

  passPostfix = "a";

  var yyparse_size = Math.max(0x2000, fs.statSync("obj2/yyparse.bin").size);

  var yyparse_start = 0xC000 - yyparse_size;

  var data_size = fs.statSync("data.bin").size;

  var data_start = yyparse_start - data_size;

  var libc_size = fs.statSync("libcdata.bin").size;

  var libc_start = data_start - libc_size;

  var floatlib_size = fs.statSync("floatlibdata.bin").size;

  var floatlib_start = libc_start - floatlib_size;

  var main_size = fs.statSync("maindata.bin").size;

  var main_start = floatlib_start - main_size;

  var heap_size = main_start - 0x081A; /* 0x1a bytes are taken up by working variables */

  remainder -= (yyparse_size+data_size+libc_size+floatlib_size+main_size);

  execSync(
      "sed 's/ROMH:   file = %O, start = $8000, size = $4000/"+
      "ROMH:   file = %O, start = $"+
      (yyparse_start.toString(16).toUpperCase())+
      ", size = $"+
      (yyparse_size.toString(16).toUpperCase())+
      "/g;"+
      's/RAM:    file = "", start = $0800, size = $9800/'+
      'RAM:    file = "", start = $0800, size = $'+
      (heap_size.toString(16).toUpperCase())+"/g;"+
      "' yyparse.cfg > yyparsea.cfg"
    );

  execSync(
      "sed 's/data2.bin\\\", start = $0800, size = $9800/"+
      "data2.bin\\\", start = $"+
      (data_start.toString(16).toUpperCase())+
      ", size = $"+
      (data_size.toString(16).toUpperCase())+
      "/g;"+
      's/RAM:    file = "", start = $0800, size = $9800/'+
      'RAM:    file = "", start = $0800, size = $'+
      (heap_size.toString(16).toUpperCase())+"/g;"+
      "' data.cfg > dataa.cfg"
    );

  execSync(
      "sed 's/floatlibdata2.bin\\\", start = \$0800, size = \$9800/"+
      "floatlibdata2.bin\\\", start = $"+
      (floatlib_start.toString(16).toUpperCase())+
      ", size = $"+
      (floatlib_size.toString(16).toUpperCase())+
      "/g;"+
      's/RAM:    file = "", start = $0800, size = $9800/'+
      'RAM:    file = "", start = $0800, size = $'+
      (heap_size.toString(16).toUpperCase())+"/g;"+
      "' floatlib.cfg > floatliba.cfg"
    );

  execSync(
      "sed 's/libcdata2.bin\\\", start = \$0800, size = \$9800/"+
      "libcdata2.bin\\\", start = $"+
      (libc_start.toString(16).toUpperCase())+
      ", size = $"+
      (libc_size.toString(16).toUpperCase())+
      "/g;"+
      's/RAM:      file = "", start = $0800, size = $9800/'+
      'RAM:      file = "", start = $0800, size = $'+
      (heap_size.toString(16).toUpperCase())+"/g;"+
      "' libc.cfg > libca.cfg"
    );

  execSync(
      "sed 's/RAM2:     file = \\\"\\\", start = $0800, size = $9800/"+
      "RAM2:     file = \\\"\\\", start = $"+
      (main_start.toString(16).toUpperCase())+
      ", size = $"+
      (main_size.toString(16).toUpperCase())+
      "/g;"+
      's/RAM:      file = "", start = $0800, size = $9800/'+
      'RAM:      file = "", start = $0800, size = $'+
      (heap_size.toString(16).toUpperCase())+"/g;"+
      "' main.cfg > maina.cfg"
    );

  execSync(
      "sed '"+
      's/RAM:     file = "", start = $0800, size = $9800/'+
      'RAM:     file = "", start = $0800, size = $'+
      (heap_size.toString(16).toUpperCase())+"/g;"+
      "' page.cfg > pagea.cfg"
    );

  compileFloatLib();
}

function compileYYParse() {
  console.log("compileYYParse");

  /*yyparse goes directly into an oversized page of its own */
  execSync(
      "cl65 -T -t c64 "+
      "-o ./obj2/yyparse.bin "+
      "-Ln ./obj2/yyparse.lbl "+
      "-C ./yyparsea.cfg ./g/_yyparse.s"
    );

  execSync("mv s/_yyparse.s ./_yyparse.s");

  /* read the label file and use its contents to
  update each function address in the hashmap */
  var lineReader = readline.createInterface({
    input: fs.createReadStream('./obj2/yyparse.lbl')
  });

  lineReader.on('line', updateFunctionAddress.bind(3));

  /* when finished, start the next step (compiling the functions that
  make up my program) */
  lineReader.on('close', addROData);
}

/* pack most function code into a set of 8k memory pages */
function addROData() {
  console.log("addROData");

  var list = [];
  var walker = walk.walk('./s', {});

  walker.on("file", (root, fileStats, next) => {
    list.push([fileStats.name, fs.readFileSync("s/"+fileStats.name, {encoding: 'utf8'})]);

    next();
  });

  walker.on("errors", (root, nodeStatsArray, next) => {
    next();
  });

  walker.on("end", () => {
    list.forEach(updateName);

    packPages();
  });
}

function packPages() {
  console.log("packPages");

  /*patch compareCodepoints into the functions that need it (so the table is
  always in the same page) */
  execSync("sed -i 's/jmp     farret/rts/g;s/__compareCodepoints/_compareCodepoints/g;' s/_compareCodepoints.s");

  execSync("cat s/_compareCodepoints.s >> s/_getBytesCP1252.s;sed -i 's/_compareCodepoints/_compareCP1252/g;s/querycsv/querycsv1/g;' s/_getBytesCP1252.s");
  execSync("cat s/_compareCodepoints.s >> s/_getBytesCommon.s;sed -i 's/_compareCodepoints/_compareCommon/g;s/querycsv/querycsv2/g;' s/_getBytesCommon.s");
  execSync("cat s/_compareCodepoints.s >> s/_getBytesPetscii.s;sed -i 's/_compareCodepoints/_comparePetscii/g;s/querycsv/querycsv3/g;' s/_getBytesPetscii.s");
  execSync("cat s/_compareCodepoints.s >> s/_getBytesAtariST.s;sed -i 's/_compareCodepoints/_compareAtariST/g;s/querycsv/querycsv4/g;' s/_getBytesAtariST.s");
  execSync("rm s/_compareCodepoints.s");

  execSync(
    "pushd s;"+
      "find * -name \"*.s\" ! -name _yyparse.s -print0|"+
      "sed -z \"s/\\.s$//g\"|"+
      "xargs -0 -I {} sh -c '"+
        "echo \".include \\\"../header.inc\\\"\" > ../g/'{}'.s;"+
        "echo \".include \\\"../labels2.s\\\"\" >> ../g/'{}'.s;"+
        "cat '{}'.s >> ../g/'{}'.s;"+
        ""+
        "cl65 -T -t c64 "+
        "-o ../obj/'{}'.bin "+
        "-C ../function.cfg "+
        "-Ln ../g/'{}'.lbl ../g/'{}'.s;"+
        "rm ../g/'{}'.o;"+
      "';"+
    "popd"
  );

  /* use a bin packing algorithm to group the functions and
  produce a binary of each 8k page */
  var list = exec(
      "sh -c '"+
        "pushd obj > /dev/null;"+
          "find * -type f ! -name _yyparse.bin -print0|"+
          "xargs -0 stat --printf=\"%s %n\\n\"|"+
          "sort -rh;"+
        "popd > /dev/null"+
      "'"
    );

  var lineReader = readline.createInterface({
    input: list.stdout
  });

  var maxSize = 8277;//8277;

  files = [];
  var totalSizes = [];

  lineReader.on('line', function(line) {
    var size = parseInt(line.match(/^[0-9]+/)[0], 10);
    var name = line.replace(/^[0-9]+ /,"");
    var count =-1;

    if(size > maxSize) {
      throw "file too big";
    }

    for(i=0;;i++) {
      if(i==files.length) {
        /*the function won't fit in any of the existing pages. Add a new page for it instead */
        files.push([]);
        count++;
        totalSizes.push(0);
      }

      if(totalSizes[i]+size < maxSize) {
        files[i].push(name);
        totalSizes[i]+=size;
        break;
      }
    }
  });

  lineReader.on('close', compilePages);
}

function compilePages() {
  console.log("compilePages");

  var i = 0;

  /*compile each page and then update the addresses and
  page numbers of each function in the table*/
  for(i = 0; i < files.length; i++) {
    var regexes = files[i].
      map(x => x.replace(/\.bin$/g, "")).
      reduce((a,b) => a +
        (a === "(" ? "" : "|") +
        "(\\b"+b.replace(matchOperatorsRe, '\\$&')+"\\b)"
      , "(")+")";

    var names = files[i].
      map(x => JSON.stringify(x.replace(/\.bin$/g, ".s"))).
      join(" ");

    execSync(
        "pushd s;"+
        'echo ".include \\"header.inc\\"" > ../page'+(i+1)+'.s;'+
        'echo ".include \\"labels2.s\\"" >> ../page'+(i+1)+'.s;'+
        "cat "+names+" >> ../page"+(i+1)+".s;"+
          ""+
          ""+
          "cl65 -T -t c64 "+
            "-o ../obj2/page"+(i+1)+".bin "+
            "-Ln ../obj2/page"+(i+1)+".lbl "+
            "-C ../pagea.cfg ../page"+(i+1)+".s;"+
          ""+
        "popd"
      );
  }

  updatePageFunctionAddresses(4);
}

function updatePageFunctionAddresses(pageNumber) {
  console.log("updatePageFunctionAddresses: ", pageNumber-3);

  var stream = fs.createReadStream("./obj2/page"+(pageNumber-3)+".lbl");

  var lineReader = readline.createInterface({
    input: stream
  });

  stream.on('error', err => {
    if(err.code == 'ENOENT') {
      passPostfix = "b";

      functionsList[hashMap["_in_word_set_a"]][1] = pageNumber++;
      functionsList[hashMap["_in_word_set_b"]][1] = pageNumber++;
      functionsList[hashMap["_isCombiningChar"]][1] = pageNumber;
      functionsList[hashMap["_in_word_set_c"]][1] = pageNumber++;

      for (i = 0; i < hash2ChunkCount; i++) {
        let name = '_isInHash2_'+i;
        functionsList[hashMap[name]][1] = pageNumber+i;
      }

      createTrampolinesInclude();
    }
    else {
      console.log("Caught", err);
    }
  });

  lineReader.on('line', updateFunctionAddress.bind(pageNumber));

  /* when finished, start the next step (compiling libc) */
  lineReader.on('close', () => {
    updatePageFunctionAddresses(pageNumber+1);
  });
}

function glueTogetherBinary() {
  console.log("glueTogetherBinary");

  execSync('dd if=/dev/zero bs=1 count='+remainder+' | tr "\\000" "\\377" >padding.bin');

  execSync("cat padding.bin maindata.bin floatlibdata.bin libcdata.bin data.bin obj2/yyparse.bin > output.bin");

  execSync('dd if=/dev/zero bs=1 count=8192 | tr "\\000" "\\377" >8k.bin');

  execSync("cat 8k.bin querycsv.bin >full.bin");
  execSync("cat 8k.bin floatlib.bin >>full.bin");
  execSync("cat 8k.bin libc.bin >>full.bin");
  execSync("cat output.bin >>full.bin");

  for(i = 0; i < files.length; i++) {
    execSync("cat 8k.bin ./obj2/page"+(i+1)+".bin >>full.bin");
  }

  for (i = 0; i < hash2ChunkCount; i++) {
    execSync("cat 8k.bin ./obj2/hash2in"+i+".bin >>full.bin");
  }

  execSync("cat 8k.bin ./obj2/hash4a.bin >>full.bin");
  execSync("cat 8k.bin ./obj2/hash4b.bin >>full.bin");
  execSync("cat 8k.bin ./obj2/hash4c.bin >>full.bin");

  execSync("bin2efcrt full.bin full.crt");

  //all done (hooray!)
}

/* *** HELPER FUNCTIONS AFTER THIS POINT *** */

/*
Split the specified assembly file into one file for each function's code
(including the necessary rodata) and append to a global data segment.
Pause if node.js can't keep up
*/
function splitUpFunctions(filename, callback, append) {
  console.log('splitUpFunctions');

  var j;
  var notAddedJmpVec = true;

  var lineReader = readline.createInterface({
    input: fs.createReadStream(filename+'.s')
  });

  var data = fs.createWriteStream('data.s', {
    flags: append?'a':'w'
  });

  var rodataOutputStreams = [];

  var code = fs.createWriteStream(filename+'_code.s');

  var functionOutputStreams = [];
  var activeStream = code;
  var rodataType = 0;

  lineReader.on('line', function(line) {
    var name;

    if(/^\.segment\s+"[_0-9A-Z]+"/.test(line)) {
      name = line.replace(/\.segment\s+"/, "").match(/[_0-9A-Z]+/)[0];

      if(name == "CODE") {
        rodataType = 0;

        if(functionOutputStreams.length) {
          activeStream = functionOutputStreams[functionOutputStreams.length-1];
        }
        else {
          activeStream = code;
        }
      }
      else if(name == "RODATA") {
        rodataType = 1;
        activeStream = data;

        //don't output this line
      }
      else {
        rodataType = 2;

        activeStream = data;

        if(name !== "BSS") {
          writePause(activeStream, line+"\n");
        }

        /*explicitly add jmpvec, so it works with rom paging*/
        if(name === "DATA" && !append && notAddedJmpVec) {
          notAddedJmpVec = false;
          writePause(activeStream, ".export jmpvec\njmpvec: jmp $ffff\n");
        }


      }
    }
    else if(/\.export/.test(line) || /\.endproc/.test(line)) {
      //do nothing
    }
    else if (/^\.proc\s+/.test(line)) {
      name = (line.replace(/^\.proc\s+/, "")).match(/[^:]+/)[0];
      functionOutputStreams.push(fs.createWriteStream('s/'+name+'.s'));

      activeStream = functionOutputStreams[functionOutputStreams.length-1];

      writePause(
          activeStream,
          ".segment \"CODE\"\n"+
          ".export _"+name+"\n"+
          line.replace(name, "_"+name)+"\n"
        );

      /* add an entry for each into the mapping table */
      if(name !== "_compareCodepoints") { /* compareCodepoints is a bsearch callback that
        needs to be in the same page as the function that called bsearch. it doesn't need
        to be in the jump table */
        hashMap[name] = functionsList.length;
        functionsList.push([name, 0, 0x0001, "farcall"]);
      }
    }
    else if (rodataType && /^[a-z0-9A-Z]+:/.test(line)) {
      if(activeStream) {
        name = line.match(/^[a-z0-9A-Z]+/)[0];

        if(rodataType == 1 || rodataType == 3) {
          rodataType = 3;

          rodataOutputStreams.push(fs.createWriteStream("ro/"+name+'.s'));

          /* add to the list of rodata regexes used to add the appropriate rodata to each function */
          rodataLabels.push([name, false, new RegExp("(\\b"+name.replace(matchOperatorsRe, '\\$&')+"\\b)", "m")]);

          activeStream = rodataOutputStreams[rodataOutputStreams.length-1];
        }
        else {
          writePause(activeStream, ".export "+name.toLowerCase()+filename+"\n");
        }

        writePause(activeStream, line.replace(name, name.toLowerCase()+filename)+"\n");
      }
    }
    else if (rodataType && /^[_a-z0-9A-Z]+\s*:/.test(line)) {
      if(activeStream) {
        name = line.match(/^[_a-z0-9A-Z]+/)[0];

        if(rodataType == 1 || rodataType == 3) {
          rodataType = 3;

          rodataOutputStreams.push(fs.createWriteStream("ro/"+name+'.s'));
          rodataLabels.push([
              name,
              false,
              new RegExp("(\\b"+name.replace(matchOperatorsRe, '\\$&')+"\\b)", "m")
            ]);

          activeStream = rodataOutputStreams[rodataOutputStreams.length-1];
          writePause(activeStream, ".ifndef "+name+"\n");
        }
        else {
          writePause(activeStream, ".export "+name+"\n");
        }
        writePause(activeStream, line+"\n");
      }
    }
    else if (rodataType && /^[a-z0-9A-Z]+\s+:=/.test(line)) {
      if(activeStream) {
        name = line.match(/^[a-z0-9A-Z]+/)[0];

        if(rodataType == 1 || rodataType == 3) {
          rodataType = 3;

          rodataOutputStreams.push(fs.createWriteStream("ro/"+name+'.s'));
          rodataLabels.push([name, false, new RegExp("(\\b"+name.replace(matchOperatorsRe, '\\$&')+"\\b)", "m")]);
          activeStream = rodataOutputStreams[rodataOutputStreams.length-1];
        }
        else {
          writePause(activeStream, ".export "+name.toLowerCase()+filename+"\n");
        }

        line = line.replace(name, name.toLowerCase()+filename)
        var name2 = line.match(/L[0-9A-F]+/);
        if(name2) {
          writePause(
              activeStream,
              line.replace(name2[0], name2[0].toLowerCase()+filename)+
              "\n"
            );
        }
        else {
          writePause(activeStream, line+"\n");
        }
      }
    }
    else {
      writePause(
          activeStream,
          line.
            replace(/L[0-9A-F]{4}/g, match => match.toLowerCase()+filename)+
            "\n"
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
      writePause(rodataOutputStreams[i], ".endif\n");
      rodataOutputStreams[i].end(allStreamsClosed);
    }

    for(i = 0; i < functionOutputStreams.length; i++) {
      writePause(functionOutputStreams[i], ".endproc\n");

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

/*
  update the addresses for functions in the functionslist array with
  their resultant values after compilation
*/
function updateFunctionAddress(line) {
  var name = line.replace(/^al\s+[0-9A-F]+ \./, "");

  var address = parseInt(line.match(/[0-9A-F]+/), 16);
  var pageNumber = this+0;

  var name2 = name.replace(/^_/, "");

  //the libc page is page 2
  if(pageNumber === 2 && name == "initlib") {
    defines["initlib2"] = address;
  }
  else if(pageNumber === 2 && name == "zerobss") {
    defines["zerobss2"] = address;
  }
  else if(pageNumber === 2 && name == "donelib") {
    defines["donelib2"] = address;
  }
  else if(defines.hasOwnProperty(name)) {
    defines[name] = address;
  }

  if(
      name !== '_main' &&  // don't use the wrong main
      pageNumber < 3 &&
      hashMap.hasOwnProperty(name)
  ) {
    functionsList[hashMap[name]][2] = address;
    functionsList[hashMap[name]][1] = pageNumber;
  }
  else if(
      hashMap.hasOwnProperty(name2) &&
      functionsList[hashMap[name2]][2] == 1 /*address 1 means it we don't yet
      have the real value*/
  ) {
    functionsList[hashMap[name2]][2] = address;
    functionsList[hashMap[name2]][1] = pageNumber;
  }
}

/* updateName adds to rodata values needed by each function to the end of
its assembly source file */
function updateName(elem) {
  var text = elem[1];

  var name = elem[0].replace(/\.s$/, "");

  var hasMatches = false;

  rodataLabels.forEach(element => {
    if(element[1] = element[2].test(text)) {
      hasMatches = true;
    }
  });

  execSync(
    'echo ".segment \\"INIT\\"" >> s/'+elem[0] + " &&" +
    'echo ".segment \\"STARTUP\\"" >> s/'+elem[0] + " &&" +
    'echo ".segment \\"ONCE\\"" >> s/'+elem[0] + (!hasMatches?'':(" &&" +
    'echo ".segment \\"RODATA\\"" >> s/'+elem[0] + " &&" +
    "cat "+
    shellEscape(rodataLabels.filter(label => label[1]).map(label => "ro/"+label[0]+".s")) +
    ">> s/"+elem[0]))
  );
}

//add all functions found to the jump table. generate the jump table as a binary file, including the trampoline code to compute the offsets the code will use

//compute the output size of each c file (code+rodata) and the total size of static vars. use a modified cc65 that adds an underscore to the start of each called function and does "jmp farret" instead of rts

//each c file will complain about missing functions. create an assembler include that satisfies these with addresses within the jump table and compile the page again

//group the c files together to attempt to fill 8k ram pages. keep a running total of the amount of memory bytes used for static variables and update the config file accordingly. update the addesses in the jump table as we go along

//create a new jump table with the correct offsets

//create a binary file that contains the inital values for all static variables and an assembler function that copies the data to the right location

//glue all the binaries together

//figure out the total ram used and alter the config file so stack and heap start in a memory area above static variables

//update the start address for ram

/*compile the main page again, using our updated table */

/*glue all the memory bank dumps together */
