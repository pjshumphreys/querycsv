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

        // NOTE: Technically this should Object.defineProperty at
        //       the next index, as push can be affected by
        //       properties on Object.prototype and Array.prototype.
        //       But that method's new, and collisions should be
        //       rare, so use the more-compatible alternative.
        if (fun.call(thisArg, val, i, t)) {
          res.push(val);
        }
      }
    }

    return res;
  };
}

//read both files
var fs = require('fs');

//var hash2 = JSON.parse(fs.readFileSync("./decompose.json", 'utf8'));
var hash2 = JSON.parse(fs.readFileSync("./filtered4.json", 'utf8'));

//*
//var weired = JSON.parse(fs.readFileSync("./weired.json", 'utf8'));
var weired = JSON.parse(fs.readFileSync("./filtered4.json", 'utf8'));

var i, len1;
var j, len2;

//find occurances of an entry in weired in the replacements column of hash2 and replace it withe the corresponding weired replacement

for(i = 0, len1 = weired.length; i< len1;i++) {
  for(j = 0, len2 = hash2.length; j< len2;j++) {
    if(hash2[j][1].indexOf(weired[i][0]) !== -1) {
      hash2[j][1] = hash2[j][1].replace(new RegExp("\\b" + weired[i][0] + "\\b", 'g'), weired[i][1]);
      
      //console.log(hash2[j], weired[i]);
    }

  }
}

var hash = {};
//console.log(hash2.length);
//append the weired to hash 2, sort by the first column and output
hash2 = hash2.concat(weired).sort(function(a, b){
  var len1 = a[1].split(" ").length,len2 = b[1].split(" ").length;
  if(len1 !== len2) {
    return len1 > len2? -   1:1;
  }

  return parseInt(a[0],16)-parseInt(b[0],16)

  }).filter(function(elem){
  if(!hash.hasOwnProperty(elem[0])) {
    hash[elem[0]] = elem[1].split(" ").length;
    return true;
  }
  return false;
}).sort(function(a, b){
    return parseInt(a[0],16)-parseInt(b[0],16)
  });
//*/
process.stdout.write('[\n');
for(i = 0, len1 = hash2.length; i < len1;i++) {
  process.stdout.write(JSON.stringify(hash2[i][0], 1)+(i == len1-1?"\n":",\n"));
}
process.stdout.write(']');



