if (!String.fromCodePoint) {
    String.fromCodePoint = function () {
        var strChars = [], codePoint, offset, codeValues, i;
        for (i = 0; i < arguments.length; ++i) {
            codePoint = arguments[i];
            offset = codePoint - 0x10000;
            if (codePoint > 0xFFFF){
                codeValues = [0xD800 + (offset >> 10), 0xDC00 + (offset & 0x3FF)];
            } else{
                codeValues = [codePoint];
            }
            strChars.push(String.fromCharCode.apply(null, codeValues));
        }
        return strChars.join("");
    };
}

if (!Array.prototype.filter) {
  Array.prototype.filter = function(fun/*, thisArg*/) {
    'use strict';

    if (this === void 0 || this === null) {
      throw new TypeError();
    }

    var t = Object(this);
    var len = t.length >>> 0;
    if (typeof fun !== 'function') {
      throw new TypeError();
    }

    var res = [];
    var thisArg = arguments.length >= 2 ? arguments[1] : void 0;
    for (var i = 0; i < len; i++) {
      if (i in t) {
        var val = t[i];

        /*  NOTE: Technically this should Object.defineProperty at */
        /*        the next index, as push can be affected by */
        /*        properties on Object.prototype and Array.prototype. */
        /*        But that method's new, and collisions should be */
        /*        rare, so use the more-compatible alternative. */
        if (fun.call(thisArg, val, i, t)) {
          res.push(val);
        }
      }
    }

    return res;
  };
}

var child_process = require('child_process');
var execSync = child_process.execSync;
var exec = child_process.exec;
var spawn = child_process.spawn;
var fs = require('graceful-fs');
var readline = require('readline');

var array1 = [];
var array3 = [];
var outer = "";
var inner = "";

/* parse UnicodeData.txt to find characters that have a decomposition string */
var lineReader = readline.createInterface({
  input: fs.createReadStream('UnicodeData.txt')
});

lineReader.on('line', function(line){
  var item = line.split(";");
  if(item[5] !== "") {
    array3.push([item[0], item[5].replace(/<[^>]+> /g, ""), item[1]])
  }
});

lineReader.on('close', function(){
  fs.readFile("./weired.json", 'utf8', weiredRead);
});

function weiredRead(err, data) {
  var array4;
  var array5;
  if(err) {
    console.log(err);
    return;
  }

  array4 = JSON.parse(data);
  var e = 0;
  do {
    array5 = JSON.stringify(array3);
    array3 = expand(array3, array4);
    array4 = JSON.stringify(array3);
  } while(array5!==array4 && (array4 = JSON.parse(array4)));

  fs.readFile("./hash2outT.h", 'utf8', outerRead);
}

function expand(hash2, weired) {
  var i, len1;
  var j, len2;

  // find occurances of an entry in weired in the replacements column of hash2 and replace it with the corresponding weired replacement

  for(i = 0, len1 = weired.length; i< len1;i++) {
    for(j = 0, len2 = hash2.length; j< len2;j++) {
      if(hash2[j][1].indexOf(weired[i][0]) !== -1) {
        hash2[j][1] = hash2[j][1].replace(new RegExp("\\b" + weired[i][0] + "\\b", 'g'), weired[i][1]);
      }
    }
  }

  var hash = {};

  // append the weired to hash 2, sort by the first column and output
  return hash2.
    concat(weired).
    sort(function(a, b) {
      var len1 = a[1].split(" ").length,len2 = b[1].split(" ").length;

      if(len1 !== len2) {
        return len1 > len2?-1:1;
      }

      return parseInt(a[0],16) - parseInt(b[0],16)
    }).
    filter(function(elem) {
      if(!hash.hasOwnProperty(elem[0])) {
        hash[elem[0]] = elem[1].split(" ").length;
        return true;
      }

      return false;
    }).
    sort(function(a, b) {
      return parseInt(a[0],16) - parseInt(b[0],16)
    });
}

function outerRead(err, contents) {
  if(err) {
    console.log(err);
    return;
  }

  outer = contents;

  fs.readFile("./hash2iT.h", 'utf8', innerRead);
}  

// combine this array with our own extra list of replacements and repeat replacing until the source and destination files are the same


// split the array into groups of [n] codepoints, and generate a set of functions that call bsearch for that subset, orchestated by a parent function (part of which is hand written)

function innerRead(err, contents) {
  if(err) {
    console.log(err);
    return;
  }

  inner = contents;

  var i, len;
  var j, len2;

  var data = array3;

  var arrays = [];
  
  var size = process.argv.length > 2 && parseInt(process.argv[2],10)?parseInt(process.argv[2],10):1114112;
  var string;
  var string2;

  while (data.length > 0) {
    arrays.push(data.splice(0, size));
  }

  for(j = 0, len2 = arrays.length; j < len2; j++) {
    string = "/*unicode normalization mapping table*/\n\
  #define HASH2SIZE "+arrays[j].length+"\n\n\
  static const long\n";
    
    // use fromcodepoint to write to the file in utf-8
    for(i = 0, len = arrays[j].length; i < len; i++) {
      string += 'hash2_' +
                (i+1) +
                '[] = {0x' +
                arrays[j][i][1].replace(/ /g, ", 0x") +
                '}' +
                (i==len-1?';':',') +
                '  /* ' +
                arrays[j][i][2] +
                " */ \n"; 
    }

    string += "\nstatic const struct hash2Entry hash2[HASH2SIZE] = {\n";

    for(i = 0, len = arrays[j].length; i < len; i++) {
      string += '{0x' +
        // String.fromCodePoint(parseInt(
        arrays[j][i][0]+/* , 16)) + */
        ', ' +
        arrays[j][i][1].split(" ").length +
        ', hash2_'+(i+1)+ '},  /*'+ arrays[j][i][2] + "*/\n";
    }

    string += '};';
    
    fs.writeFile('./hash2in'+j+'.h', inner.replace(/TABLE/g, string).replace(/NUMBER/g, j), 'utf8');
  }

  string = "";
  string2 = ""

  for(j = 0, len2 = arrays.length; j < len2; j++) {
    string += "  else if(codepoint < 0x"+(j==(len2-1)?"110000":arrays[j+1][0][0]) +") {\n\
    isInHash2_"+j+"();\n\
  }\n\n";

    string2 += "void isInHash2_"+j+"(void);\n"
  }

  fs.writeFile('./hash2out.h', outer.replace(/DEFINES/g, string2).replace(/BLOCKS/g, string), 'utf8');
}
