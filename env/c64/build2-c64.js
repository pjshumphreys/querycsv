var i  = 0;

var execSync = require('child_process').execSync;
var spawn = require('child_process').spawn;
var fs = require('fs');
var readline = require('readline');

var hashMap = {};
var functionsList = [
  ["BASIC_FAC_to_string", 0, 0xBDDD, "FUNC0"],   /* in: FAC value   out: str at $0100 a/y ptr to str */
  ["BASIC_string_to_FAC", 0, 0xB7B5, "FUNC0"],   /* in: $22/$23 ptr to str,a=strlen out: FAC value */

  ["BASIC_s8_to_FAC",     0, 0xBC3C, "FUNC0"],	  /* a: value */
  ["BASIC_u8_to_FAC",     0, 0xB3A2, "FUNC0"],	  /* y: value */

  ["BASIC_u16_to_FAC",    0, 0xBC49, "FUNC0"],   /* a/y:lo/hi value (sta $62 sty $63 sec ldx#$90 jsr...) */
  ["BASIC_s16_to_FAC",    0, 0xB395, "FUNC0"],   /* a/y:lo/hi value */
  ["BASIC_FAC_to_u16",    0, 0xBC9B, "FUNC0"],   /* in:FAC out: y/a:lo/hi value */

/*--------------------------------------------------------------------------------------------- */
/* these functions take one arg (in FAC) and return result (in FAC) aswell */
/*--------------------------------------------------------------------------------------------- */

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

/*--------------------------------------------------------------------------------------------- */
/* these functions take two args (in FAC and ARG) and return result (in FAC) */
/*--------------------------------------------------------------------------------------------- */

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

  /*normally these functions are wrapped by more code, but we need to be able to call them in a raw form */
  ["BASIC_FAC_Atn",       0, 0xE30E, "FUNC0"],   /* in/out: FAC */
  ["BASIC_ARG_FAC_Div",   0, 0xBB12, "FUNC0"],   /* in: ARG,FAC out:FAC */
  ["BASIC_ARG_FAC_Add",   0, 0xB86A, "FUNC0"],    /* in: ARG,FAC out:FAC */
];

/* these are unused */
/*BASIC_FAC_Poly2 	= $e043     ; in: FAC x  a/y ptr to poly (1byte grade,5bytes per coefficient) */

/*BASIC_LoadARG		= $babc	    ; a/y:lo/hi ptr to 5-byte float */
/*BASIC_LoadFAC		= $bba2	    ; a/y:lo/hi ptr to 5-byte float */

var functionsList2 = [
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

var functionsList3 = [
  /* standard C library functions */
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
  ["_getenv",             2, 0x0001, "farcall2"],
  ["_ltoa",               2, 0x0001, "farcall2"],
  ["_malloc",             2, 0x0001, "farcall2"],
  //["_memcpy",             2, 0x0001, "farcall2"],
  ["_memmove",            2, 0x0001, "farcall2"],
  ["_memset",             2, 0x0001, "farcall2"],
  ["_open",               2, 0x0001, "farcall2"],
  ["_printf",             2, 0x0001, "farcall2"],
  ["_putenv",             2, 0x0001, "farcall2"],
  ["_read",               2, 0x0001, "farcall2"],
  ["_realloc",            2, 0x0001, "farcall2"],
  ["_setlocale",          2, 0x0001, "farcall2"],
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
  ["_vfprintf",           2, 0x0001, "farcall2"],
  ["_vsnprintf",          2, 0x0001, "farcall2"],
  ["_vsprintf",           2, 0x0001, "farcall2"],
  ["_write",              2, 0x0001, "farcall2"],
  
  ["_normaliseAndGet",    0, 0x0001, "farcall"],
  ["_isInHash2",          0, 0x0001, "farcall"],
  ["_isCombiningChar",    0, 0x0001, "farcall"],
  ["_hash",               0, 0x0001, "farcall"],
  ["_in_word_set",        0, 0x0001, "farcall"],

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

//compile cc65-floatlib. 
////////////////////////

var tables = fs.createWriteStream('floatlib/tables.inc');

for(i = 0; i < functionsList.length; i++) {
  hashMap[functionsList[i][0]] = i;
  
  tables.write(functionsList[i][0]+":\n  stx xRegBackup\n  ldx #$" + ("00"+(i.toString(16).toUpperCase())).substr(-2) + "\n  jmp "+ functionsList[i][3]+"\n");
}

tables.write("\nhighAddressTable:\n");

for(i = 0; i < functionsList.length; i++) {
  tables.write(".byte $"+("0000"+((functionsList[i][2]-1).toString(16).toUpperCase())).substr(-4).substring(0,2)+"\n");
}

tables.write("\nlowAddressTable:\n");

for(i = 0; i < functionsList.length; i++) {
  tables.write(".byte $"+("00"+((functionsList[i][2]-1).toString(16).toUpperCase())).substr(-2)+"\n");
}

for(i = 0; i < functionsList2.length; i++) {
  tables.write(".export "+functionsList2[i][0]+"\n");
}

tables.end(part1);

function part1(){
  console.log(1);

  execSync("cl65 -I ./floatlib -T -t c64 -Ln floatlib.lbl -vm -O -Os --static-locals -W -C floatlib.cfg floatlib/float.s floatlib.c");

  execSync("((echo -n \"ibase=16; \" && grep __RAM2_TOTAL__ floatlib.lbl | sed -n \"s/al \([^ ]*\).*"+"/\1/p\") | bc)|xargs -I {} dd if=/dev/zero bs=1 count={} of=floatlibdata2.bin");

  execSync("dd if=floatlibdata.bin of=floatlibdata2.bin conv=notrunc");

  functionsList2.forEach(function(v) {
    hashMap[v[0]] = this.length;
    this.push(v);
  }, functionsList);

  functionsList3.forEach(function(v) {
    hashMap[v[0]] = this.length;
    this.push(v);
  }, functionsList);

  //read the label file and update each function address
  //////////////////////////////////////////////////////
  var lineReader = readline.createInterface({
    input: fs.createReadStream('floatlib.lbl')
  });

  lineReader.on('line', updateLine);
  lineReader.on('close', part2);
}

function updateLine(line) {
  var name = line.replace(/^al [0-9A-F]+ \./, "");
  var address = parseInt(line.match(/[0-9A-F]+/), 16);

  if(name == "initlib") {
    defines["initlib2"] = address;
  }
  else if(defines.hasOwnProperty(name)) {
    defines[name] = address;
  }
   
  if(hashMap.hasOwnProperty(name) && functionsList[hashMap[name]][2] == 1) {  
    functionsList[hashMap[name]][2] = address;
  }
}

function part2() {
  console.log(2);

  //compile fake program that uses c library
  execSync("cl65 -T -t c64 -Ln libc.lbl -O -Os --static-locals -W -C libc.cfg libc.c initenv.s");

  execSync("((echo -n \"ibase=16; \" && grep __RAM2_TOTAL__ libc.lbl | sed -n \"s/al \([^ ]*\).*"+"/\1/p\") | bc)|xargs -I {} dd if=/dev/zero bs=1 count={} of=padded.bin");

  execSync("dd if=libcdata.bin of=padded.bin conv=notrunc");

  var lineReader = readline.createInterface({
    input: fs.createReadStream('libc.lbl')
  });

  lineReader.on('line', updateLine);
  lineReader.on('close', part3);
}

var code;
function part3() {
  console.log(3);

  //compile functions to assembly language. use our own patched cc65 executable that does "jmp farret" instead of "rts"
  execSync("./cc65 -T -t c64 -O -Os querycsv.c");

  //grep using the .proc directive to find the names of all the functions.
  var lineReader = readline.createInterface({
    input: fs.createReadStream('querycsv.s')
  });

  var data = fs.createWriteStream('data.s');
  code = fs.createWriteStream('code.s');
  var func;
  var active = code;

  lineReader.on('line', function(line) {
    var name;
    if(/^\.segment	"[_0-9A-Z]+"/.test(line)) {
      name = line.replace(/\.segment	"/, "").match(/[_0-9A-Z]+/)[0];
      
      if(name == "CODE") {
        active = func;
      }
      else {
        active = data;
        active.write(line+"\n");
      }
    }
    else if(/\.export/.test(line)) {
      //do nothing
    }
    else if (/^\.proc\s+/.test(line)) {
      if(func) {
        func.end();
      }

      name = (line.replace(/^\.proc\s+/, "")).match(/[^:]+/)[0];
      func = fs.createWriteStream('s/'+name+'.s');
      active = func;
      active.write(".include \"code.s\"\n.export "+name+"\n"+line+"\n");

      //add an entry for each into the mapping table
      if(name !== "_main") {
        hashMap[name] = functionsList.length;
        functionsList.push([name, 0, 0x0001, "farcall"]);
      }
    }
    else if (active === data && /^[a-z0-9A-Z]+:/.test(line)) {
      if(active) {
        name = line.match(/^[a-z0-9A-Z]+/)[0];

        active.write(".export "+name.toLowerCase()+"\n");
        active.write(line.replace(name, name.toLowerCase())+"\n");
      }
    }
    else if (active === data && /^[a-z0-9A-Z]+\s+:=/.test(line)) {
      if(active) {
        name = line.match(/^[a-z0-9A-Z]+/)[0];

        active.write(".export "+name.toLowerCase()+"\n");

        line = line.replace(name, name.toLowerCase())
        var name2 = line.match(/L[0-9A-F]+/);
        if(name2) {
          active.write(line.replace(name2[0], name2[0].toLowerCase())+"\n");
        }
        else {
          active.write(line+"\n");
        }
      }
    }
    else {
      if(active) {
        name = line.match(/L[0-9A-F]+[^:]/);
        if(name && active === data) {
          active.write(line.replace(name[0], name[0].toLowerCase())+"aaa\n");
        }
        else {
          active.write(line+"\n");
        }
      }
    }
  });
  
  lineReader.on('close', function() {
    data.write(".export _main\n\
.segment	\"CODE\"\n\
_main:\n\
inx");

    if(func) {
      func.end();
    }
    
    data.end(part4);
  });
}

function part4() {
  console.log(4);

  //compile data, rodata and bss segments. generate an assembly language include of all the labels
  execSync("cl65 -Ln data.lbl -C data.cfg data.s");

  var lineReader = readline.createInterface({
    input: fs.createReadStream('data.lbl')
  });

  lineReader.on('line', function(line) {
    var name = line.replace(/^al [0-9A-F]+ \./, "");

    if(name.match(/l[0-9a-f]{4}/)) {
      var address = parseInt(line.match(/[0-9A-F]+/), 16);
     
      code.write(name+" = $"+("0000"+address.toString(16).toUpperCase()).substr(-4)+"\n");
      execSync("sed -i 's/"+name.toUpperCase()+"/"+name+"/g' s/*");
    }
  });
  
  lineReader.on('close', function() {
    code.end(part5);
  });
}

function part5() {
  console.log(5);
  var tables = fs.createWriteStream('tables.inc');
  for(i in defines) {
    tables.write(".export "+i+"\n"+i+" = $"+("0000"+((defines[i]).toString(16).toUpperCase())).substr(-4)+"\n");
  }
  
  for(i = 0; i < functionsList.length; i++) {    
    tables.write(".export "+functionsList[i][0]+"\n"+functionsList[i][0]+":\n  stx xRegBackup\n  ldx #$" + ("00"+(i.toString(16).toUpperCase())).substr(-2) + "\n  jmp "+ functionsList[i][3]+"\n");
  }

  tables.write("\nhighAddressTable:\n");
  for(i = 0; i < functionsList.length; i++) {
    tables.write(".byte $"+("0000"+((functionsList[i][2]-1).toString(16).toUpperCase())).substr(-4).substring(0,2)+"\n");
  }

  tables.write("\nlowAddressTable:\n");
  for(i = 0; i < functionsList.length; i++) {
    tables.write(".byte $"+("00"+((functionsList[i][2]-1).toString(16).toUpperCase())).substr(-2)+"\n");
  }

  tables.write("\nbankTable:\n");
  for(i = 0; i < functionsList.length; i++) {
    tables.write(".byte $"+("00"+(functionsList[i][1].toString(16).toUpperCase())).substr(-2)+"\n");
  }

  tables.end(part6);
}

function part6() {
  console.log(6);

  execSync("cl65 -Ln main.lbl -T -t c64 -C linker.cfg crt0.s s/_main.s");

  //compile each function separately to identify all their sizes

  //create an include file that defines all functions except the one being compiled

  //use a bin packing algorithm to group the functions and produce a binary of each 8k page

  //read the addresses and page numbers of each function back into the table

  //hash1, hash2, hash3 and hash4 functions need to be split somehow.

  //the lexer and sql files need to be split somehow and the references to RODATA replaced with paging code (ie. the code is in A000-BFFF and the RODATA is in 8000-BFFF)

  //glue all the memory bank dumps together
}



/*
_printf:
jsr FUNC0
_sprintf:
jsr FUNC0
_getenv:
*/

