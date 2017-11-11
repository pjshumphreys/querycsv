var child_process = require('child_process');
var execSync = child_process.execSync;
var exec = child_process.exec;
var spawn = child_process.spawn;
var fs = require('graceful-fs');
var readline = require('readline');
var i;
var code;
var labels;
var files;

var hashMap = {};

/*functionsList = A mapping of function names to the memory page that
contains them, the address within that page and which trampoline function
in the $C000 range to call to set them up correctly*/
var functionsList = [
  ["BASIC_FAC_to_string", 0, 0xBDDD, "FUNC0"],   /* in: FAC value   out: str at $0100 a/y ptr to str */
  ["BASIC_string_to_FAC", 0, 0xB7B5, "FUNC0"],   /* in: $22/$23 ptr to str,a=strlen out: FAC value */

  ["BASIC_s8_to_FAC",     0, 0xBC3C, "FUNC0"],	  /* a: value */
  ["BASIC_u8_to_FAC",     0, 0xB3A2, "FUNC0"],	  /* y: value */

  ["BASIC_u16_to_FAC",    0, 0xBC49, "FUNC0"],   /* a/y:lo/hi value (sta $62 sty $63 sec ldx#$90 jsr...) */
  ["BASIC_s16_to_FAC",    0, 0xB395, "FUNC0"],   /* a/y:lo/hi value */
  ["BASIC_FAC_to_u16",    0, 0xBC9B, "FUNC0"],   /* in:FAC out: y/a:lo/hi value */

/*------------------------------------------------------------------------- */
/* these functions take one arg (in FAC) and return result (in FAC) as well */
/*------------------------------------------------------------------------- */

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

/*-------------------------------------------------------------------------- */
/* these functions take two args (in FAC and ARG) and return result (in FAC) */
/*-------------------------------------------------------------------------- */

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

  /*normally these functions are wrapped by more code, but we need to be able
  to call them in a raw form */
  ["BASIC_FAC_Atn",       0, 0xE30E, "FUNC0"],   /* in/out: FAC */
  ["BASIC_ARG_FAC_Div",   0, 0xBB12, "FUNC0"],   /* in: ARG,FAC out:FAC */
  ["BASIC_ARG_FAC_Add",   0, 0xB86A, "FUNC0"],    /* in: ARG,FAC out:FAC */
];

/* these are unused */
/*BASIC_FAC_Poly2 	= $e043     ; in: FAC x  a/y ptr to poly (1byte grade,5bytes per coefficient) */

/*BASIC_LoadARG		= $babc	    ; a/y:lo/hi ptr to 5-byte float */
/*BASIC_LoadFAC		= $bba2	    ; a/y:lo/hi ptr to 5-byte float */

var floatlibFunctionsList = [
  ["__ftostr",            2, 0x0001, "farcall"],
  ["__strtof",            2, 0x0001, "farcall"],
  ["__ctof",              2, 0x0001, "farcall"],
  ["__utof",              2, 0x0001, "farcall"],
  ["__stof",              2, 0x0001, "farcall"],
  ["__itof",              2, 0x0001, "farcall"],
  ["__ftoi",              2, 0x0001, "farcall"],
  ["__fcmp",              2, 0x0001, "farcall"],
  ["__ftestsgn",          2, 0x0001, "farcall"],
  ["__fneg",              2, 0x0001, "farcall"],
  ["__fpoly1",            2, 0x0001, "farcall"],
  ["__fpoly2",            2, 0x0001, "farcall"],
  ["__fatan2",            2, 0x0001, "farcall"]
];

var clibFunctionsList = [
  /* standard C library functions */
  ["_strtod",             2, 0x0001, "farcall"], /*this'll be provided by my
  floatib wrapper */
  ["_calloc",             2, 0x0001, "farcall2"],
  ["_clearerr",           2, 0x0001, "farcall2"],
  ["_close",              2, 0x0001, "farcall2"],
  //["_exit",               2, 0x0001, "farcall2"],
  ["_fclose",             2, 0x0001, "farcall2"],
  ["_feof",               2, 0x0001, "farcall2"],
  ["_ferror",             2, 0x0001, "farcall2"],
  ["_fflush",             2, 0x0001, "farcall2"],
  ["_fgetc",              2, 0x0001, "farcall2"],
  ["_fopen",              2, 0x0001, "farcall2"],
  ["_fprintf",            2, 0x0001, "farcall2"],
  ["_fputc",              2, 0x0001, "farcall2"],
  ["_fputs",              2, 0x0001, "farcall2"],
  ["_fread",              2, 0x0001, "farcall2"],
  ["_free",               2, 0x0001, "farcall2"],
  ["_fwrite",             2, 0x0001, "farcall2"],
  //["_getenv",             2, 0x0001, "farcall2"],
  ["_ltoa",               2, 0x0001, "farcall2"],
  ["_malloc",             2, 0x0001, "farcall2"],
  //["_memcpy",             2, 0x0001, "farcall2"],
  ["_memmove",            2, 0x0001, "farcall2"],
  ["_memset",             2, 0x0001, "farcall2"],
  ["_open",               2, 0x0001, "farcall2"],
  ["_printf",             2, 0x0001, "farcall2"],
  //["_putenv",             2, 0x0001, "farcall2"],
  ["_read",               2, 0x0001, "farcall2"],
  ["_realloc",            2, 0x0001, "farcall2"],
  //["_setlocale",          2, 0x0001, "farcall2"],
  ["_sprintf",            2, 0x0001, "farcall2"],
  ["_strcat",             2, 0x0001, "farcall2"],
  ["_strcmp",             2, 0x0001, "farcall2"],
  ["_strcpy",             2, 0x0001, "farcall2"],
  ["_stricmp",            2, 0x0001, "farcall2"],
  ["_strlen",             2, 0x0001, "farcall2"],
  ["_strlower",           2, 0x0001, "farcall2"],
  ["_strncat",            2, 0x0001, "farcall2"],
  ["_strncmp",            2, 0x0001, "farcall2"],
  ["_strstr",             2, 0x0001, "farcall2"],
  ["_ultoa",              2, 0x0001, "farcall2"],
  ["_ungetc",             2, 0x0001, "farcall2"],
  //["_vfprintf",           2, 0x0001, "farcall2"],
  ["_vsnprintf",          2, 0x0001, "farcall2"],
  ["_vsprintf",           2, 0x0001, "farcall2"],
  ["_write",              2, 0x0001, "farcall2"]
];

var functionsList4 = [
  ["_getHash1",           0, 0x0001, "farcall"],
  ["_isInHash2",          0, 0x0001, "farcall"],
  ["_isCombiningChar",    0, 0x0001, "farcall"],
  ["_in_word_set_a",      0, 0x0001, "farcall2"],
  ["_in_word_set_b",      0, 0x0001, "farcall2"],
  ["_in_word_set_c",      0, 0x0001, "farcall2"],
  ["_yylex_destroy",      0, 0x0001, "farcall"],
  ["_yylex_init",         0, 0x0001, "farcall"],
  ["_yyparse",            0, 0x0001, "farcall"],
  ["_yyset_in",           0, 0x0001, "farcall"]
];

var defines = {
  "_stderr": 0,
  "_stdin": 0,
  "_stdout": 0,
  "initlib2": 0,
  "___float_fac_to_float": 0,
  "___float_float_to_fac": 0,
  "___float_float_to_fac_arg": 0,
  "aRegBackup": 0,
  "xRegBackup": 0
};

/* The first action to initiate */
createFloatLibInclude();

/* create the cc65-floatlib include */
function createFloatLibInclude() {
  console.log('createFloatLibInclude');
  
  var tables = fs.createWriteStream('floatlib/floatlib.inc');

  for(i = 0; i < functionsList.length; i++) {
    /* TODO: what is the hashmap for? */
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
  execSync(
      "cl65 -T -t c64 "+
      "-I ./floatlib "+
      "-Ln floatlib.lbl "+
      "--config floatlib.cfg "+
      "floatlib/float.s floatlib.s"
    );

  /*create an zeroed binary the same size as the total ram used */
  execSync(
      "(("+
        "echo -n \"ibase=16; \" && "+
        "grep __RAM2_TOTAL__ floatlib.lbl | "+
        "sed -n \"s/al \([^ ]*\).*/\1/p\""+
      ")|bc)|"+
      "xargs -I {} dd if=/dev/zero bs=1 count={} of=floatlibdata2.bin"
    );

  /*TODO: confirm this: Create a combined binary of the ram and read
  only data so that they can be copied as one blob? */
  execSync("dd if=floatlibdata.bin of=floatlibdata2.bin conv=notrunc");

  /* add the float lib functions to the hashmap */
  floatlibFunctionsList.forEach(function(v) {
    hashMap[v[0]] = this.length;
    this.push(v);
  }, functionsList);

  /* clib functions added now also as strtod is provided by floatllib */
  clibFunctionsList.forEach(function(v) {
    hashMap[v[0]] = this.length;
    this.push(v);
  }, functionsList); 

  /* read the label file and use its contents to
  update each function address in the hashmap */
  var lineReader = readline.createInterface({
      input: fs.createReadStream('floatlib.lbl')
    });
  lineReader.on('line', updateFunctionAddress);

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
      "--config libc.cfg "+
      "libc.c"
    );

  /*create an zeroed binary the same size as the total ram used */
  execSync(
      "(("+
        "echo -n \"ibase=16; \" && "+
        "grep __RAM2_TOTAL__ libc.lbl | "+
        "sed -n \"s/al \([^ ]*\).*"+"/\1/p\""+
      ")|bc)|"+
      "xargs -I {} dd if=/dev/zero bs=1 count={} of=padded.bin"
    );

  /*TODO: confirm this: Create a combined binary of the ram and read
  only data so that they can be copied as one blob? */
  execSync("dd if=libcdata.bin of=padded.bin conv=notrunc");

  /* read the label file and use its contents to
  update each function address in the hashmap */
  var lineReader = readline.createInterface({
    input: fs.createReadStream('libc.lbl')
  });
  lineReader.on('line', updateFunctionAddress);

  /* when finished, start the next step (compiling the functions that
  make up my program) */
  lineReader.on('close', splitUpFunctions);
}

/*
compile all the functions that easily fit in a memory page (which is most of
them). Split the output assembly file into one file for each function's code
and another one for its data. Pause if node.js can't keep up
*/
function splitUpFunctions() {
  console.log('splitUpFunctions');

  var j;

  /* compile functions into assembly language. use our own
  patched cc65 executable that does "jmp farret" instead of "rts" */
  execSync("./cc65_2 -T -t c64 -O -Os querycsv.c");

  var lineReader = readline.createInterface({
    input: fs.createReadStream('querycsv.s')
  });

  var data = fs.createWriteStream('data.s');
  code = fs.createWriteStream('code.s');
  labels = fs.createWriteStream('labels.s');

  var functionOutputStreams = [];
  var activeStream = code;

  lineReader.on('line', function(line) {
    var name;

    if(/^\.segment	"[_0-9A-Z]+"/.test(line)) {
      name = line.replace(/\.segment	"/, "").match(/[_0-9A-Z]+/)[0];
      
      if(name == "CODE") {
        if(functionOutputStreams.length) {
          activeStream = functionOutputStreams[functionOutputStreams.length-1];
        }
        else {
          activeStream = code;
        }
      }
      else {
        activeStream = data;
        writePause(activeStream, line+"\n");
      }
    }
    else if(/\.export/.test(line) || /\.endproc/.test(line)) {
      //do nothing
    }
    else if (/^\.proc\s+/.test(line)) {
      name = (line.replace(/^\.proc\s+/, "")).match(/[^:]+/)[0];
      functionOutputStreams.push(fs.createWriteStream('s/'+name+'.s'));
      
      activeStream = functionOutputStreams[functionOutputStreams.length-1];

      if(name == '_main') {
        writePause(
            activeStream,
            ".include \"../code.s\"\n"+
            ".export "+name+"\n"+
            line+"\n"
          );
      }
      else {
        writePause(
            activeStream,
            ".include \"../code2.s\"\n"+
            ".export "+name+"\n"+
            line+"\n"
          );
      }

      /* add an entry for each into the mapping table */
      if(name !== "_main") {
        hashMap[name] = functionsList.length;
        functionsList.push([name, 0, 0x0001, "farcall"]);
      }
    }
    else if (activeStream === data && /^[a-z0-9A-Z]+:/.test(line)) {
      if(activeStream) {
        name = line.match(/^[a-z0-9A-Z]+/)[0];

        writePause(activeStream, ".export "+name.toLowerCase()+"\n");
        writePause(activeStream, line.replace(name, name.toLowerCase())+"\n");
      }
    }
    else if (activeStream === data && /^_[a-z0-9A-Z]+\s*:/.test(line)) {
      if(activeStream) {
        name = line.match(/^_[a-z0-9A-Z]+/)[0];

        writePause(activeStream, ".export "+name+"\n");
        writePause(activeStream, line+"\n");
      }
    }
    else if (activeStream === data && /^[a-z0-9A-Z]+\s+:=/.test(line)) {
      if(activeStream) {
        name = line.match(/^[a-z0-9A-Z]+/)[0];

        writePause(activeStream, ".export "+name.toLowerCase()+"\n");

        line = line.replace(name, name.toLowerCase())
        var name2 = line.match(/L[0-9A-F]+/);
        if(name2) {
          writePause(
              activeStream,
              line.replace(name2[0], name2[0].toLowerCase())+
              "\n"
            );
        }
        else {
          writePause(activeStream, line+"\n");
        }
      }
    }
    else {
      //if(activeStream) {
        name = line.match(/L[0-9A-F]+[^:]/);
        if(name && activeStream === data) {
          writePause(
              activeStream,
              line.replace(name[0], name[0].toLowerCase())+
              "\n"
            );
        }
        else {
          if(activeStream == code && (!/\.import\b/.test(line))) {
            writePause(labels, line+"\n");
          }
          writePause(activeStream, line+"\n");
        }
      //}
    }
  });

  lineReader.on('close', function() {
    data.write(
      ".export _main\n" +
      ".segment	\"CODE\"\n"+
      "_main:\n"+
      "inx"
    );

    j = functionOutputStreams.length;

    data.end(writeFunctionPostfixes);
  });

  function writeFunctionPostfixes() {
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
      compileData();
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

/* compile the data segment to a memory page. (this will be copied to ram
on startup) */
function compileData() {
  console.log('compileData');

  /* compile data, rodata and bss segments. generate an assembly
  language include of all the labels */
  execSync("cl65 -Ln data.lbl -C data.cfg data.s -vm -m data.map");

  var lineReader2 = readline.createInterface({
    input: fs.createReadStream('data.lbl')
  });

  /* read the resultant memory locations an make an assembly include
  containing just their addresses */
  lineReader2.on('line', function(line) {
    var name = line.replace(/^al [0-9A-F]+ \./, "");

    if(name.match(/l[0-9a-f]{4}/)) {
      var address = parseInt(line.match(/[0-9A-F]+/), 16);
     
      code.write(
          name+" = $"+
          ("0000"+address.toString(16).toUpperCase()).substr(-4)+
          "\n"
        );

      labels.write(
          name+" = $"+
          ("0000"+address.toString(16).toUpperCase()).substr(-4)+
          "\n"
        );

      execSync("sed -i 's/"+name.toUpperCase()+"/"+name+"/g' s/*");
    }
    else if(name.match(/\b_[0-9a-zA-Z]/) && name !== '_main') {
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
    code.end(createAddressInclude);
  });
}

function createAddressInclude() {
  console.log("createAddressInclude");

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
    tables.write(
        ".export "+functionsList[i][0]+"\n"+

        functionsList[i][0]+":\n"+
        "  stx xRegBackup\n"+
        "  ldx #$"+("00"+(i.toString(16).toUpperCase())).substr(-2)+"\n"+
        "  jmp "+functionsList[i][3]+"\n"
      );
  }

  tables.write("\nhighAddressTable:\n");
  for(i = 0; i < functionsList.length; i++) {
    tables.write(
        ".byte $"+
        ("0000"+((functionsList[i][2]-1).toString(16).toUpperCase())).
        substr(-4).
        substring(0,2)+
        "\n"
      );
  }

  tables.write("\nlowAddressTable:\n");
  for(i = 0; i < functionsList.length; i++) {
    tables.write(
        ".byte $"+
        ("00"+((functionsList[i][2]-1).toString(16).toUpperCase())).
        substr(-2)+
        "\n"
      );
  }

  tables.write("\nbankTable:\n");
  for(i = 0; i < functionsList.length; i++) {
    tables.write(
        ".byte $"+
        ("00"+(functionsList[i][1].toString(16).toUpperCase())).
        substr(-2)+
        "\n"
      );
  }

  tables.end(compileMain);
}

function compileMain() {
  console.log("compileMain");

  execSync("cl65 -T -t c64 -Ln main.lbl --config main.cfg crt0.s s/_main.s");

  /*read the label file and update each function address */
  var lineReader = readline.createInterface({
    input: fs.createReadStream('main.lbl')
  });

  for(i in defines) {
    labels.write(
        i+" = $"+
        ("0000"+((defines[i]).toString(16).toUpperCase())).substr(-4)+
        "\n"
      );
  }

  labels.write(".export _main\n_main = $0100\n");
  labels.write(".import pushl0\n");
  
  lineReader.on('line', function(line) {
    var name = line.replace(/^al [0-9A-F]+ \./, "");
    var address = parseInt(line.match(/[0-9A-F]+/), 16);
     
    if(name == "farret") {  
      labels.write(
          "farret = $"+
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
    labels.end(packPages);
  });
}

/* pack most function code into a set of 8k memory pages */
function packPages() {
  console.log("packPages");

  /*compile each function separately to identify all their sizes. */
  /*create an include file that defines all functions except the one
  being compiled */

  execSync(
      "pushd s;"+
        "find * -name \"*.s\" -print0|"+
        "sed -z \"s/\\.s$//g\"|"+
        "xargs -0 -I {} sh -c '"+
          "rm ../code2.s;"+
          "egrep -v \"\\b'{}'\\b\" ../labels.s > ../code2.s;"+
          "cl65 -T -t c64 -o ../obj/'{}'.bin -C ../function.cfg '{}'.s;"+
          "rm '{}'.o;"+
        "';"+
      "popd"
    );

  /* use a bin packing algorithm to group the functions and
  produce a binary of each 8k page */
  var list = exec(
      "sh -c '"+
        "pushd obj > /dev/null;"+
          "find * -type f -print0|"+
          "xargs -0 stat --printf=\"%s %n\\n\"|"+
          "sort -rh;"+
        "popd > /dev/null"+
      "'"
    );
  
  var lineReader = readline.createInterface({
    input: list.stdout
  });

  var maxSize = 9205;

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
  var matchOperatorsRe = /[|\\{}()\[\]^$+*?.]/g;

  /*compile each page and then update the addresses and
  page numbers of each function in the table*/
  for(i = 0; i< files.length;i++) {
    var regexes = files[i].
      map(x => x.replace(/\.bin$/g, "")).
      reduce((a,b) => a + (a === "(" ? "" : "|") +
        "(\\b"+b.replace(matchOperatorsRe, '\\$&')+"\\b)"
      , "(")+")";

    var names = files[i].
      map(x => JSON.stringify(x.replace(/\.bin$/g, ".s"))).
      join(" ");

    execSync(
        "pushd s;"+
          "rm ../code2.s;"+
          "egrep -Ev \""+regexes+"\" ../labels.s > ../code2.s;"+
          "cl65 -T -t c64 "+
            "-o ../obj2/page"+(i+1)+".bin "+
            "-Ln ../obj2/page"+(i+1)+".lbl "+
            "-C ../page.cfg "+names+";"+
          "rm *.o;"+
        "popd"
      );
  }

  compileHash1();
}

/*compile hash1 and hash3 (they will fit in a memory page but their
data is quite large so we don't want to store their RODATA in main RAM) */
function compileHash1() {
  compileHash2();
}

function compileHash2() {
  console.log("compileHash2");

  var i;
  
  execSync("node ./generate_hash2.js 390");

  for(i = 0; fs.existsSync('./hash2in'+i+'.c'); i++) {
    execSync(
        "cl65 -T -t c64 "+
        "-o obj2/hash2in"+i+".bin "+
        "-Ln obj2/hash2in"+i+".lbl "+
        "-C rodata-page.cfg "+
        "hash2in"+i+".c labels.s;"+
        "rm *.o"
      );
  }

  execSync(
      "cl65 -T -t c64 "+
      "-o obj2/hash2out.bin "+
      "-Ln obj2/hash2out.lbl "+
      "-C rodata-page.cfg "+
      "hash2out.c labels.s;"+
      "rm *.o"
    );

  /*
  put an entry in the functions list for each file.
  as the code in each of them doesn't need to call any other page
  we can probably use farret2
  */

  compileHash4();
}

function compileHash4() {
  console.log("compileHash4");

  execSync(
      "cl65 -T -t c64 "+
      "-o obj2/hash4a.bin "+
      "-Ln obj2/hash4a.lbl "+
      "-C rodata-page.cfg "+
      "hash4a.c labels.s;"+
      "rm *.o");

  execSync(
      "cl65 -T -t c64 "+
      "-o obj2/hash4b.bin "+
      "-Ln obj2/hash4b.lbl "+
      "-C rodata-page.cfg "+
      "hash4b.c labels.s;"+
      "rm *.o"
    );

  execSync(
      "cl65 -T -t c64 "+
      "-o obj2/hash4c.bin "+
      "-Ln obj2/hash4c.lbl "+
      "-C rodata-page.cfg "+
      "hash4c.c labels.s;"+
      "rm *.o"
    );


  /* what to do about lexer and parser? they're both too big to fit in 8k */
}

/*compile the main page again, using our updated table */

/*glue all the memory bank dumps together */

/*
_printf:
jsr FUNC0
_sprintf:
jsr FUNC0
_getenv:
*/


/*
  update the addresses for functions in the functionslist array with
  their resultant values after compilation
*/
function updateFunctionAddress(line) {
  var name = line.replace(/^al [0-9A-F]+ \./, "");
  var address = parseInt(line.match(/[0-9A-F]+/), 16);

  if(name == "initlib") {
    defines["initlib2"] = address;
  }
  else if(defines.hasOwnProperty(name)) {
    defines[name] = address;
  }
   
  if(
      hashMap.hasOwnProperty(name) &&
      functionsList[hashMap[name]][2] == 1 /*address 1 means it we don't yet
      have the real value*/
  ) {  
    functionsList[hashMap[name]][2] = address;
  }
}
